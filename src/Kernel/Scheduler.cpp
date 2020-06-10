#include "Scheduler.h"

#include "Interface/ServiceInterface.h"
#include "Interface/TimelineServiceInterface.h"
#include "Interface/UpdateServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"

#include "math/utils.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Scheduler::Scheduler()
        : m_updataterId( INVALID_UPDATABLE_ID )
        , m_speedFactor( 1.f )
        , m_time( 0.f )
        , m_freezeAll( false )
        , m_update( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Scheduler::~Scheduler()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::initialize()
    {
        UpdationInterface * updation = this->getUpdation();

        uint32_t updataterId = UPDATE_SERVICE()
            ->createUpdatater( EUM_SERVICE_BEFORE, 0U, UpdationInterfacePtr( updation ) );

        if( updataterId == INVALID_UPDATABLE_ID )
        {
            return false;
        }

        m_updataterId = updataterId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Scheduler::finalize()
    {
        UPDATE_SERVICE()
            ->removeUpdatater( m_updataterId );

        m_updataterId = INVALID_UPDATABLE_ID;

#ifdef MENGINE_DEBUG
        m_schedulers.insert( m_schedulers.end(), m_schedulersAdd.begin(), m_schedulersAdd.end() );
        m_schedulersAdd.clear();
        
        m_schedulers.erase( std::remove_if( m_schedulers.begin(), m_schedulers.end(), []( const SchedulerEventDesc & _desc )
        {
            return _desc.dead;
        } ), m_schedulers.end() );

        for( const SchedulerEventDesc & desc : m_schedulers )
        {
            LOGGER_ERROR( "Forgot remove event '%u' from scheduler '%s' (doc: %s)"
                , desc.id
                , this->getName().c_str()
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#endif

        m_schedulers.clear();
        m_schedulersAdd.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void Scheduler::setName( const ConstString & _name )
    {
        m_name = _name;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Scheduler::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Scheduler::event( float _delay, const SchedulerEventInterfacePtr & _event, const DocumentPtr & _doc )
    {
        MENGINE_ASSERTION( _event != nullptr, "scheduler '%s' event delay '%f' is nullptr"
            , this->getName().c_str()
            , _delay
        );

        uint32_t new_id = GENERATE_UNIQUE_IDENTITY();

        SchedulerEventDesc desc;

        desc.id = new_id;

        desc.event = _event;
        desc.timer = nullptr;
        desc.pipe = nullptr;

        desc.delay = _delay;

        desc.time_delay = 0.f;

        desc.iterate = 0;

        desc.type = EST_EVENT;
        desc.dead = false;
        desc.freeze = false;
        desc.iterate_invalide = true;

        desc.doc = _doc;

        m_schedulersAdd.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Scheduler::timing( const SchedulerPipeInterfacePtr & _pipe, const SchedulerTimingInterfacePtr & _timer, const SchedulerEventInterfacePtr & _event, const DocumentPtr & _doc )
    {
        uint32_t new_id = GENERATE_UNIQUE_IDENTITY();

        SchedulerEventDesc desc;

        desc.id = new_id;

        desc.event = _event;
        desc.timer = _timer;
        desc.pipe = _pipe;

        desc.delay = 0.f;
        desc.time_delay = 0.f;

        desc.iterate = 0;

        desc.type = EST_TIMING;
        desc.dead = false;
        desc.freeze = false;
        desc.iterate_invalide = true;

        desc.doc = _doc;

        m_schedulersAdd.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::exist( uint32_t _id ) const
    {
        VectorSchedulers::const_iterator it_find =
            std::find_if( m_schedulers.begin(), m_schedulers.end(), [_id]( const SchedulerEventDesc & _event )
        {
            return _event.id == _id;
        } );

        if( it_find != m_schedulers.end() )
        {
            return true;
        }

        VectorSchedulers::const_iterator it_find_add =
            std::find_if( m_schedulersAdd.begin(), m_schedulersAdd.end(), [_id]( const SchedulerEventDesc & _event )
        {
            return _event.id == _id;
        } );

        if( it_find_add != m_schedulersAdd.end() )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::refresh( uint32_t _id )
    {
        SchedulerEventDesc * desc;

        if( this->findSchedulerEvent_( _id, &desc ) == false )
        {
            LOGGER_ERROR( "not found shedule '%d'"
                , _id
            );

            return false;
        }

        desc->time_delay = 0.f;
        desc->iterate = 0;
        desc->iterate_invalide = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::remove( uint32_t _id )
    {
        SchedulerEventDesc * desc;

        if( this->findSchedulerEvent_( _id, &desc ) == false )
        {
            LOGGER_ERROR( "not found shedule '%d'"
                , _id
            );

            return false;
        }

        if( this->removeScheduler_( desc ) == false )
        {
            LOGGER_ERROR( "not alredy remove or complete '%d'"
                , _id
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Scheduler::removeAll()
    {
        if( m_schedulers.empty() == false )
        {
            VectorSchedulers schedules = std::move( m_schedulers );

            for( SchedulerEventDesc & event : schedules )
            {
                this->removeScheduler_( &event );
            }
        }

        if( m_schedulersAdd.empty() == false )
        {
            VectorSchedulers schedulesAdd = std::move( m_schedulersAdd );

            for( SchedulerEventDesc & event : schedulesAdd )
            {
                this->removeScheduler_( &event );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::removeScheduler_( SchedulerEventDesc * const _event )
    {
        if( _event->dead == true )
        {
            return false;
        }

        _event->dead = true;

        switch( _event->type )
        {
        case EST_EVENT:
            {
                SchedulerEventInterfacePtr event = _event->event;

                _event->event = nullptr;

                event->onSchedulerStop( _event->id );
            }break;
        case EST_TIMING:
            {
                SchedulerEventInterfacePtr event = _event->event;

                _event->event = nullptr;
                _event->timer = nullptr;
                _event->pipe = nullptr;

                if( event != nullptr )
                {
                    event->onSchedulerStop( _event->id );
                }
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Scheduler::update( const UpdateContext * _context )
    {
        float total_time = _context->time * m_speedFactor;

        m_time += total_time;

        if( m_freezeAll == true )
        {
            return;
        }

        m_update = true;

        if( m_schedulersAdd.empty() == false )
        {
            m_schedulers.insert( m_schedulers.end(), m_schedulersAdd.begin(), m_schedulersAdd.end() );
            m_schedulersAdd.clear();
        }

        VectorSchedulers::iterator it_erase = std::remove_if( m_schedulers.begin(), m_schedulers.end(), []( const SchedulerEventDesc & _event )
        {
            return _event.dead;
        } );
        m_schedulers.erase( it_erase, m_schedulers.end() );

        for( SchedulerEventDesc & desc : m_schedulers )
        {
            if( desc.dead == true )
            {
                continue;
            }

            if( desc.freeze == true )
            {
                continue;
            }

            float old_timing = desc.time_delay;

            desc.time_delay += total_time;

            switch( desc.type )
            {
            case EST_EVENT:
                {
                    if( desc.time_delay < desc.delay )
                    {
                        continue;
                    }

                    float timeOffset = desc.delay - old_timing;

                    TIMELINE_SERVICE()
                        ->beginOffset( timeOffset, desc.doc );

                    desc.dead = true;

                    if( desc.event != nullptr )
                    {
                        SchedulerEventInterfacePtr event = desc.event;

                        event->onSchedulerComplete( desc.id );
                    }

                    TIMELINE_SERVICE()
                        ->endOffset();
                }break;
            case EST_TIMING:
                {
                    float acc_delay = 0.f;

                    for( ;; )
                    {
                        if( desc.iterate_invalide == true )
                        {
                            float delay = desc.pipe->onSchedulerPipe( desc.id, desc.iterate );

                            if( delay < 0.f )
                            {
                                desc.dead = true;

                                if( desc.event != nullptr )
                                {
                                    SchedulerEventInterfacePtr event = desc.event;

                                    event->onSchedulerComplete( desc.id );
                                }

                                break;
                            }

                            desc.iterate_invalide = false;
                            desc.delay = delay;
                        }

                        if( desc.time_delay < desc.delay )
                        {
                            break;
                        }

                        acc_delay += desc.delay;

                        float timeOffset = acc_delay - old_timing;

                        uint32_t iterate = desc.iterate;

                        desc.time_delay -= desc.delay;

                        desc.iterate++;
                        desc.iterate_invalide = true;

                        TIMELINE_SERVICE()
                            ->beginOffset( timeOffset, desc.doc );

                        desc.timer->onSchedulerTiming( desc.id, iterate, desc.delay );

                        TIMELINE_SERVICE()
                            ->endOffset();

                        if( desc.dead == true )
                        {
                            break;
                        }
                    }
                }break;
            }
        }

        m_update = false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::freeze( uint32_t _id, bool _freeze )
    {
        SchedulerEventDesc * event;

        if( this->findSchedulerEvent_( _id, &event ) == false )
        {
            LOGGER_ERROR( "not found shedule '%d'"
                , _id
            );

            return false;
        }

        event->freeze = _freeze;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::isFreeze( uint32_t _id ) const
    {
        const SchedulerEventDesc * event;

        if( this->findSchedulerEvent_( _id, &event ) == false )
        {
            LOGGER_ERROR( "not found shedule '%d'"
                , _id
            );

            return false;
        }

        return event->freeze;
    }
    //////////////////////////////////////////////////////////////////////////
    void Scheduler::freezeAll( bool _freeze )
    {
        m_freezeAll = _freeze;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::isFreezeAll() const
    {
        return m_freezeAll;
    }
    //////////////////////////////////////////////////////////////////////////
    float Scheduler::getTimePassed( uint32_t _id ) const
    {
        const SchedulerEventDesc * event;

        if( this->findSchedulerEvent_( _id, &event ) == false )
        {
            LOGGER_ERROR( "not found shedule '%d'"
                , _id
            );

            return 0.f;
        }

        float time = event->time_delay;

        return time;
    }
    //////////////////////////////////////////////////////////////////////////
    float Scheduler::getTimeLeft( uint32_t _id ) const
    {
        const SchedulerEventDesc * event;

        if( this->findSchedulerEvent_( _id, &event ) == false )
        {
            LOGGER_ERROR( "not found shedule '%d'"
                , _id
            );

            return 0.f;
        }

        float time = event->delay - event->time_delay;

        return time;
    }
    //////////////////////////////////////////////////////////////////////////
    void Scheduler::setSpeedFactor( float _factor )
    {
        m_speedFactor = _factor;
    }
    //////////////////////////////////////////////////////////////////////////
    float Scheduler::getSpeedFactor() const
    {
        return m_speedFactor;
    }
    //////////////////////////////////////////////////////////////////////////
    float Scheduler::getTime() const
    {
        return m_time;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::findSchedulerEvent_( uint32_t _id, SchedulerEventDesc ** _desc )
    {
        VectorSchedulers::iterator it_find =
            std::find_if( m_schedulers.begin(), m_schedulers.end(), [_id]( const SchedulerEventDesc & _event )
        {
            return _event.id == _id;
        } );

        if( it_find != m_schedulers.end() )
        {
            SchedulerEventDesc & desc = *it_find;

            *_desc = &desc;

            return true;
        }

        VectorSchedulers::iterator it_find_add =
            std::find_if( m_schedulersAdd.begin(), m_schedulersAdd.end(), [_id]( const SchedulerEventDesc & _event )
        {
            return _event.id == _id;
        } );

        if( it_find_add != m_schedulersAdd.end() )
        {
            SchedulerEventDesc & desc = *it_find_add;

            *_desc = &desc;

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::findSchedulerEvent_( uint32_t _id, const SchedulerEventDesc ** _desc ) const
    {
        VectorSchedulers::const_iterator it_find =
            std::find_if( m_schedulers.begin(), m_schedulers.end(), [_id]( const SchedulerEventDesc & _event )
        {
            return _event.id == _id;
        } );

        if( it_find != m_schedulers.end() )
        {
            const SchedulerEventDesc & desc = *it_find;

            *_desc = &desc;

            return true;
        }

        VectorSchedulers::const_iterator it_find_add =
            std::find_if( m_schedulersAdd.begin(), m_schedulersAdd.end(), [_id]( const SchedulerEventDesc & _event )
        {
            return _event.id == _id;
        } );

        if( it_find_add != m_schedulersAdd.end() )
        {
            const SchedulerEventDesc & desc = *it_find_add;

            *_desc = &desc;

            return true;
        }

        return false;
    }
}
