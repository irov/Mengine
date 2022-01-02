#include "Scheduler.h"

#include "Interface/ServiceInterface.h"
#include "Interface/TimelineServiceInterface.h"
#include "Interface/UpdateServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionContainer.h"

#include "math/utils.h"

#include "Config/Algorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Scheduler::Scheduler()
        : m_speedFactor( 1.f )
        , m_time( 0.f )
        , m_freezeAll( false )
        , m_update( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Scheduler::~Scheduler()
    {
        MENGINE_ASSERTION_CONTAINER_EMPTY( m_schedulers );
        MENGINE_ASSERTION_CONTAINER_EMPTY( m_schedulersAdd );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::initialize()
    {
        UpdationInterface * updation = this->getUpdation();

        if( updation->activate( EUM_SERVICE_BEFORE, 0U ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Scheduler::finalize()
    {
        UpdationInterface * updation = this->getUpdation();

        updation->deactivate();

#ifdef MENGINE_DEBUG
        m_schedulers.insert( m_schedulers.end(), m_schedulersAdd.begin(), m_schedulersAdd.end() );
        m_schedulersAdd.clear();

        m_schedulers.erase( Algorithm::remove_if( m_schedulers.begin(), m_schedulers.end(), []( const SchedulerEventDesc & _desc )
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
    UniqueId Scheduler::event( float _delay, const SchedulerEventInterfacePtr & _event, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( _event != nullptr, "scheduler '%s' event delay '%f' is nullptr"
            , this->getName().c_str()
            , _delay
        );

        UniqueId new_id = ENUMERATOR_SERVICE()
            ->generateUniqueIdentity();

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
        desc.iterate_invalid = true;

#if MENGINE_DOCUMENT_ENABLE
        desc.doc = _doc;
#endif

        m_schedulersAdd.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId Scheduler::timing( const SchedulerPipeInterfacePtr & _pipe, const SchedulerTimingInterfacePtr & _timer, const SchedulerEventInterfacePtr & _event, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( _pipe != nullptr, "scheduler '%s' _pipe is nullptr"
            , this->getName().c_str()
        );

        MENGINE_ASSERTION_FATAL( _timer != nullptr, "scheduler '%s' _timer is nullptr"
            , this->getName().c_str()
        );

        UniqueId new_id = ENUMERATOR_SERVICE()
            ->generateUniqueIdentity();

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
        desc.iterate_invalid = true;

#if MENGINE_DOCUMENT_ENABLE
        desc.doc = _doc;
#endif

        m_schedulersAdd.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::exist( UniqueId _id ) const
    {
        VectorSchedulers::const_iterator it_find =
            Algorithm::find_if( m_schedulers.begin(), m_schedulers.end(), [_id]( const SchedulerEventDesc & _event )
        {
            return _event.id == _id;
        } );

        if( it_find != m_schedulers.end() )
        {
            return true;
        }

        VectorSchedulers::const_iterator it_find_add =
            Algorithm::find_if( m_schedulersAdd.begin(), m_schedulersAdd.end(), [_id]( const SchedulerEventDesc & _event )
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
    bool Scheduler::refresh( UniqueId _id )
    {
        SchedulerEventDesc * desc;

        if( this->findSchedulerEvent_( _id, &desc ) == false )
        {
            LOGGER_ERROR( "not found shedule [%u]"
                , _id
            );

            return false;
        }

        desc->time_delay = 0.f;
        desc->iterate = 0;
        desc->iterate_invalid = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::remove( UniqueId _id )
    {
        SchedulerEventDesc * desc;

        if( this->findSchedulerEvent_( _id, &desc ) == false )
        {
            LOGGER_ERROR( "not found shedule [%u]"
                , _id
            );

            return false;
        }

        if( this->removeScheduler_( desc, true ) == false )
        {
            LOGGER_ERROR( "not alredy remove or complete [%u]"
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
                this->removeScheduler_( &event, true );
            }
        }

        if( m_schedulersAdd.empty() == false )
        {
            VectorSchedulers schedulesAdd = std::move( m_schedulersAdd );

            for( SchedulerEventDesc & event : schedulesAdd )
            {
                this->removeScheduler_( &event, true );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::cancel( UniqueId _id )
    {
        SchedulerEventDesc * desc;

        if( this->findSchedulerEvent_( _id, &desc ) == false )
        {
            LOGGER_ERROR( "not found shedule [%u]"
                , _id
            );

            return false;
        }

        if( this->removeScheduler_( desc, false ) == false )
        {
            LOGGER_ERROR( "not alredy remove or complete [%u]"
                , _id
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Scheduler::cancelAll()
    {
        if( m_schedulers.empty() == false )
        {
            VectorSchedulers schedules = std::move( m_schedulers );

            for( SchedulerEventDesc & event : schedules )
            {
                this->removeScheduler_( &event, false );
            }
        }

        if( m_schedulersAdd.empty() == false )
        {
            VectorSchedulers schedulesAdd = std::move( m_schedulersAdd );

            for( SchedulerEventDesc & event : schedulesAdd )
            {
                this->removeScheduler_( &event, false );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::removeScheduler_( SchedulerEventDesc * const _event, bool _callStop )
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

                if( _callStop == true )
                {
                    event->onSchedulerStop( _event->id );
                }
            }break;
        case EST_TIMING:
            {
                SchedulerEventInterfacePtr event = _event->event;

                _event->event = nullptr;
                _event->timer = nullptr;
                _event->pipe = nullptr;

                if( event != nullptr && _callStop == true )
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

        VectorSchedulers::iterator it_erase = Algorithm::remove_if( m_schedulers.begin(), m_schedulers.end(), []( const SchedulerEventDesc & _event )
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

            float old_time_delay = desc.time_delay;

            desc.time_delay += total_time;

            switch( desc.type )
            {
            case EST_EVENT:
                {
                    if( desc.time_delay < desc.delay )
                    {
                        continue;
                    }

                    float timeOffset = desc.delay - old_time_delay;

                    TIMELINE_SERVICE()
                        ->beginOffset( timeOffset, MENGINE_DOCUMENT_VALUE( desc.doc, nullptr ) );

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
                        if( desc.iterate_invalid == true )
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

                            desc.iterate_invalid = false;
                            desc.delay = delay;
                        }

                        if( desc.time_delay < desc.delay )
                        {
                            break;
                        }

                        acc_delay += desc.delay;

                        float timeOffset = acc_delay - old_time_delay;

                        uint32_t iterate = desc.iterate;

                        desc.time_delay -= desc.delay;

                        desc.iterate++;
                        desc.iterate_invalid = true;

                        TIMELINE_SERVICE()
                            ->beginOffset( timeOffset, MENGINE_DOCUMENT_VALUE( desc.doc, nullptr ) );

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
    bool Scheduler::freeze( UniqueId _id, bool _freeze )
    {
        SchedulerEventDesc * event;

        if( this->findSchedulerEvent_( _id, &event ) == false )
        {
            LOGGER_ERROR( "not found shedule [%u]"
                , _id
            );

            return false;
        }

        event->freeze = _freeze;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::isFreeze( UniqueId _id ) const
    {
        const SchedulerEventDesc * event;

        if( this->findSchedulerEvent_( _id, &event ) == false )
        {
            LOGGER_ERROR( "not found shedule [%u]"
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
    float Scheduler::getTimePassed( UniqueId _id ) const
    {
        const SchedulerEventDesc * event;

        if( this->findSchedulerEvent_( _id, &event ) == false )
        {
            LOGGER_ERROR( "not found shedule [%u]"
                , _id
            );

            return 0.f;
        }

        float time = event->time_delay;

        return time;
    }
    //////////////////////////////////////////////////////////////////////////
    float Scheduler::getTimeLeft( UniqueId _id ) const
    {
        const SchedulerEventDesc * event;

        if( this->findSchedulerEvent_( _id, &event ) == false )
        {
            LOGGER_ERROR( "not found shedule [%u]"
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
    bool Scheduler::findSchedulerEvent_( UniqueId _id, SchedulerEventDesc ** const _desc )
    {
        VectorSchedulers::iterator it_find =
            Algorithm::find_if( m_schedulers.begin(), m_schedulers.end(), [_id]( const SchedulerEventDesc & _event )
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
            Algorithm::find_if( m_schedulersAdd.begin(), m_schedulersAdd.end(), [_id]( const SchedulerEventDesc & _event )
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
    bool Scheduler::findSchedulerEvent_( UniqueId _id, const SchedulerEventDesc ** const _desc ) const
    {
        VectorSchedulers::const_iterator it_find =
            Algorithm::find_if( m_schedulers.begin(), m_schedulers.end(), [_id]( const SchedulerEventDesc & _event )
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
            Algorithm::find_if( m_schedulersAdd.begin(), m_schedulersAdd.end(), [_id]( const SchedulerEventDesc & _event )
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
    //////////////////////////////////////////////////////////////////////////
}
