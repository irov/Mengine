#include "Scheduler.h"

#include "Interface/ServiceInterface.h"
#include "Interface/TimelineServiceInterface.h"
#include "Interface/UpdateServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionContainer.h"
#include "Kernel/EnumeratorHelper.h"
#include "Kernel/UniqueHelper.h"

#include "Config/StdAlgorithm.h"

#include "math/utils.h"

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

        updation->activate( EUM_SERVICE_BEFORE, MENGINE_UINT32_C(0) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Scheduler::finalize()
    {
        UpdationInterface * updation = this->getUpdation();

        updation->deactivate();

#if defined(MENGINE_DEBUG)
        m_schedulers.insert( m_schedulers.end(), m_schedulersAdd.begin(), m_schedulersAdd.end() );
        m_schedulersAdd.clear();

        StdAlgorithm::erase_if( m_schedulers, []( const SchedulerEventDesc & _desc )
        {
            return _desc.dead;
        } );

        for( const SchedulerEventDesc & desc : m_schedulers )
        {
            LOGGER_ASSERTION( "forgot remove event '%u' from scheduler (doc: %s)"
                , desc.id
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#endif

        m_schedulers.clear();
        m_schedulersAdd.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId Scheduler::event( float _delay, const SchedulerEventInterfacePtr & _event, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( _event != nullptr, "scheduler event delay '%f' is nullptr (doc: %s)"
            , _delay
            , MENGINE_DOCUMENT_STR( _doc )
        );

        UniqueId new_id = Helper::generateUniqueIdentity();

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

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_schedulersAdd.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId Scheduler::timing( const SchedulerPipeInterfacePtr & _pipe, const SchedulerTimingInterfacePtr & _timer, const SchedulerEventInterfacePtr & _event, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_FATAL( _pipe != nullptr, "scheduler _pipe is nullptr (doc: %s)"
            , MENGINE_DOCUMENT_STR( _doc )
        );

        MENGINE_ASSERTION_FATAL( _timer != nullptr, "scheduler _timer is nullptr (doc: %s)"
            , MENGINE_DOCUMENT_STR( _doc )
        );

        UniqueId new_id = Helper::generateUniqueIdentity();

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

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_schedulersAdd.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::exist( UniqueId _id ) const
    {
        const SchedulerEventDesc * desc;

        if( this->findSchedulerEvent_( _id, &desc ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::refresh( UniqueId _id )
    {
        SchedulerEventDesc * desc;

        if( this->findSchedulerEvent_( _id, &desc ) == false )
        {
            LOGGER_ERROR( "not found scheduler event [%u]"
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
            LOGGER_ERROR( "not found scheduler event [%u]"
                , _id
            );

            return false;
        }

        this->removeScheduler_( desc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Scheduler::removeAll()
    {
        for( SchedulerEventDesc & desc : m_schedulers )
        {
            this->removeScheduler_( &desc );
        }

        VectorSchedulers schedulersAdd;
        m_schedulersAdd.swap( schedulersAdd );

        for( SchedulerEventDesc & desc : schedulersAdd )
        {
            this->removeScheduler_( &desc );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::cancel( UniqueId _id )
    {
        SchedulerEventDesc * desc;

        if( this->findSchedulerEvent_( _id, &desc ) == false )
        {
            LOGGER_ERROR( "not found scheduler event [%u]"
                , _id
            );

            return false;
        }

        this->cancelScheduler_( desc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Scheduler::cancelAll()
    {
        for( SchedulerEventDesc & desc : m_schedulers )
        {
            this->cancelScheduler_( &desc );
        }

        for( SchedulerEventDesc & desc : m_schedulersAdd )
        {
            this->cancelScheduler_( &desc );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Scheduler::removeScheduler_( SchedulerEventDesc * const _event )
    {
        if( _event->dead == true )
        {
            return;
        }

        SchedulerEventInterfacePtr event = _event->event;

        _event->dead = true;

        _event->event = nullptr;
        _event->timer = nullptr;
        _event->pipe = nullptr;

        if( event != nullptr )
        {
            event->onSchedulerStop( _event->id );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Scheduler::cancelScheduler_( SchedulerEventDesc * const _event )
    {
        if( _event->dead == true )
        {
            return;
        }

        _event->dead = true;

        _event->event = nullptr;
        _event->timer = nullptr;
        _event->pipe = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Scheduler::update( const UpdateContext * _context )
    {
        MENGINE_ASSERTION_FATAL( m_update == false, "scheduler update is reentrant" );

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

        StdAlgorithm::erase_if( m_schedulers, []( const SchedulerEventDesc & _event )
        {
            return _event.dead;
        } );

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
                            SchedulerPipeInterfacePtr pipe = desc.pipe;

                            float delay = pipe->onSchedulerPipe( desc.id, desc.iterate );

                            if( desc.dead == true )
                            {
                                break;
                            }

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

                        SchedulerTimingInterfacePtr timer = desc.timer;

                        timer->onSchedulerTiming( desc.id, iterate, desc.delay );

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
            LOGGER_ERROR( "not found scheduler event [%u]"
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
            LOGGER_ERROR( "not found scheduler event [%u]"
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
            LOGGER_ERROR( "not found scheduler event [%u]"
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
            LOGGER_ERROR( "not found scheduler event [%u]"
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
        VectorSchedulers::iterator it_find = Helper::findUnique( m_schedulers, _id );

        if( it_find != m_schedulers.end() )
        {
            SchedulerEventDesc & desc = *it_find;

            if( desc.dead == true )
            {
                return false;
            }

            *_desc = &desc;

            return true;
        }

        VectorSchedulers::iterator it_find_add = Helper::findUnique( m_schedulersAdd, _id );

        if( it_find_add != m_schedulersAdd.end() )
        {
            SchedulerEventDesc & desc = *it_find_add;

            if( desc.dead == true )
            {
                return false;
            }

            *_desc = &desc;

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::findSchedulerEvent_( UniqueId _id, const SchedulerEventDesc ** const _desc ) const
    {
        VectorSchedulers::const_iterator it_find = Helper::findUnique( m_schedulers, _id );

        if( it_find != m_schedulers.end() )
        {
            const SchedulerEventDesc & desc = *it_find;

            if( desc.dead == true )
            {
                return false;
            }

            *_desc = &desc;

            return true;
        }

        VectorSchedulers::const_iterator it_find_add = Helper::findUnique( m_schedulersAdd, _id );

        if( it_find_add != m_schedulersAdd.end() )
        {
            const SchedulerEventDesc & desc = *it_find_add;

            if( desc.dead == true )
            {
                return false;
            }

            *_desc = &desc;

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
