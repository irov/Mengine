#include "Scheduler.h"

#include "Interface/ServiceInterface.h"
#include "Interface/TimelineInterface.h"
#include "Interface/UpdateServiceInterface.h"

#include "Kernel/Logger.h"

#include "math/utils.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Scheduler::Scheduler()
        : m_updataterId( INVALID_UPDATABLE_ID )
        , m_speedFactor( 1.f )
        , m_time( 0.f )
        , m_enumerator( 0 )
        , m_freezeAll( false )
        , m_update( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Scheduler::~Scheduler()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::initialize( const ConstString & _name )
    {
        m_name = _name;

        UpdationInterfacePtr updation = this->getUpdation();

        m_updataterId = UPDATE_SERVICE()
            ->createUpdatater( 3U, 0U, updation );

        if( m_updataterId == INVALID_UPDATABLE_ID )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Scheduler::finalize()
    {
        UPDATE_SERVICE()
            ->removeUpdatater( m_updataterId );

        m_updataterId = INVALID_UPDATABLE_ID;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Scheduler::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Scheduler::event( float _delay, const ScheduleEventInterfacePtr & _event )
    {
        uint32_t new_id = ++m_enumerator;

        ScheduleEventDesc desc;

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

        m_schedulesAdd.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Scheduler::timing( const SchedulePipeInterfacePtr & _pipe, const ScheduleTimingInterfacePtr & _timer, const ScheduleEventInterfacePtr & _event )
    {
        uint32_t new_id = ++m_enumerator;

        ScheduleEventDesc desc;

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

        m_schedulesAdd.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::exist( uint32_t _id ) const
    {
        ListSchedules::const_iterator it_find =
            std::find_if( m_schedules.begin(), m_schedules.end(), [_id]( const ScheduleEventDesc & _event ) { return _event.id == _id; } );

        if( it_find != m_schedules.end() )
        {
            return true;
        }

        ListSchedules::const_iterator it_find_add =
            std::find_if( m_schedulesAdd.begin(), m_schedulesAdd.end(), [_id]( const ScheduleEventDesc & _event ) { return _event.id == _id; } );

        if( it_find_add != m_schedulesAdd.end() )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::refresh( uint32_t _id )
    {
        ScheduleEventDesc * desc;

        if( this->findScheduleEvent_( _id, desc ) == false )
        {
            LOGGER_ERROR( "ScheduleManager::remove not found shedule '%d'"
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
        ScheduleEventDesc * desc;

        if( this->findScheduleEvent_( _id, desc ) == false )
        {
            LOGGER_ERROR( "ScheduleManager::remove not found shedule '%d'"
                , _id
            );

            return false;
        }

        if( this->removeSchedule_( *desc ) == false )
        {
            LOGGER_ERROR( "ScheduleManager::remove not alredy remove or complete '%d'"
                , _id
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Scheduler::removeAll()
    {
        for( ListSchedules::iterator
            it = m_schedules.begin(),
            it_end = m_schedules.end();
            it != it_end;
            ++it )
        {
            ScheduleEventDesc & event = *it;

            this->removeSchedule_( event );
        }

        ListSchedules schedulesAdd = m_schedulesAdd;

        for( ListSchedules::iterator
            it = schedulesAdd.begin(),
            it_end = schedulesAdd.end();
            it != it_end;
            ++it )
        {
            ScheduleEventDesc & event = *it;

            this->removeSchedule_( event );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::removeSchedule_( ScheduleEventDesc & _event )
    {
        if( _event.dead == true )
        {
            return false;
        }

        _event.dead = true;

        switch( _event.type )
        {
        case EST_EVENT:
            {
                ScheduleEventInterfacePtr event = _event.event;
                _event.event = nullptr;

                event->onScheduleStop( _event.id );
            }break;
        case EST_TIMING:
            {
                ScheduleEventInterfacePtr event = _event.event;
                _event.event = nullptr;
                _event.timer = nullptr;
                _event.pipe = nullptr;

                event->onScheduleStop( _event.id );
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Scheduler::_update( const UpdateContext * _context )
    {
        float total_time = _context->time * m_speedFactor;

        m_time += total_time;

        if( m_freezeAll == true )
        {
            return;
        }

        m_update = true;

        m_schedules.insert( m_schedules.end(), m_schedulesAdd.begin(), m_schedulesAdd.end() );
        m_schedulesAdd.clear();

        ListSchedules::iterator it_erase = std::remove_if( m_schedules.begin(), m_schedules.end(), []( const ScheduleEventDesc & _event ) { return _event.dead; } );
        m_schedules.erase( it_erase, m_schedules.end() );

        for( ListSchedules::iterator
            it = m_schedules.begin(),
            it_end = m_schedules.end();
            it != it_end;
            ++it )
        {
            ScheduleEventDesc & desc = *it;

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
                        ->beginOffset( timeOffset );

                    desc.dead = true;

                    if( desc.event != nullptr )
                    {
                        ScheduleEventInterfacePtr event = desc.event;

                        event->onScheduleComplete( desc.id );
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
                            float delay = desc.pipe->onSchedulePipe( desc.id, desc.iterate );

                            if( delay < 0.f )
                            {
                                desc.dead = true;

                                if( desc.event != nullptr )
                                {
                                    ScheduleEventInterfacePtr event = desc.event;

                                    event->onScheduleComplete( desc.id );
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
                            ->beginOffset( timeOffset );

                        desc.timer->onScheduleTiming( desc.id, iterate, desc.delay );

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
        ScheduleEventDesc * event;

        if( this->findScheduleEvent_( _id, event ) == false )
        {
            LOGGER_ERROR( "ScheduleManager::freeze not found shedule '%d'"
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
        const ScheduleEventDesc * event;

        if( this->findScheduleEvent_( _id, event ) == false )
        {
            LOGGER_ERROR( "ScheduleManager::isFreeze not found shedule '%d'"
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
        const ScheduleEventDesc * event;

        if( this->findScheduleEvent_( _id, event ) == false )
        {
            LOGGER_ERROR( "ScheduleManager::time not found shedule '%d'"
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
        const ScheduleEventDesc * event;

        if( this->findScheduleEvent_( _id, event ) == false )
        {
            LOGGER_ERROR( "ScheduleManager::time not found shedule '%d'"
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
    bool Scheduler::findScheduleEvent_( uint32_t _id, ScheduleEventDesc *& _desc )
    {
        ListSchedules::iterator it_find =
            std::find_if( m_schedules.begin(), m_schedules.end(), [_id]( const ScheduleEventDesc & _event ) { return _event.id == _id; } );

        if( it_find != m_schedules.end() )
        {
            _desc = &*it_find;

            return true;
        }

        ListSchedules::iterator it_find_add =
            std::find_if( m_schedulesAdd.begin(), m_schedulesAdd.end(), [_id]( const ScheduleEventDesc & _event ) { return _event.id == _id; } );

        if( it_find_add != m_schedulesAdd.end() )
        {
            _desc = &*it_find_add;

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Scheduler::findScheduleEvent_( uint32_t _id, const ScheduleEventDesc *& _desc ) const
    {
        ListSchedules::const_iterator it_find =
            std::find_if( m_schedules.begin(), m_schedules.end(), [_id]( const ScheduleEventDesc & _event ) { return _event.id == _id; } );

        if( it_find != m_schedules.end() )
        {
            _desc = &*it_find;

            return true;
        }

        ListSchedules::const_iterator it_find_add =
            std::find_if( m_schedulesAdd.begin(), m_schedulesAdd.end(), [_id]( const ScheduleEventDesc & _event ) { return _event.id == _id; } );

        if( it_find_add != m_schedulesAdd.end() )
        {
            _desc = &*it_find_add;

            return true;
        }

        return false;
    }
}
