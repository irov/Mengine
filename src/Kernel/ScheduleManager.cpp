#include "ScheduleManager.h"

#include "Interface/ServiceInterface.h"
#include "Interface/TimelineInterface.h"

#include "Kernel/Logger.h"

#include "math/utils.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        struct FScheduleFind
        {
            FScheduleFind( uint32_t _id )
                : m_id( _id )
            {
            }

            bool operator()( const ScheduleEventDesc & _event ) const
            {
                return _event.id == m_id;
            }

            uint32_t m_id;
        };
        //////////////////////////////////////////////////////////////////////////
        struct FScheduleDead
        {
            bool operator ()( const ScheduleEventDesc & _event ) const
            {
                return _event.dead;
            }
        };
    }
    //////////////////////////////////////////////////////////////////////////
    ScheduleManager::ScheduleManager()
        : m_speedFactor( 1.f )
        , m_time( 0.f )
        , m_enumerator( 0 )
        , m_freezeAll( false )
        , m_update( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ScheduleManager::~ScheduleManager()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ScheduleManager::event( float _delay, const ScheduleEventInterfacePtr & _listener )
    {
        uint32_t new_id = ++m_enumerator;

        ScheduleEventDesc desc;

        desc.id = new_id;

        desc.event = _listener;
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
    uint32_t ScheduleManager::timing( const SchedulePipeInterfacePtr & _pipe, const ScheduleTimerInterfacePtr & _listener )
    {
        uint32_t new_id = ++m_enumerator;

        ScheduleEventDesc desc;

        desc.id = new_id;

        desc.event = nullptr;
        desc.timer = _listener;
        desc.pipe = _pipe;

        desc.delay = 0.f;
        desc.time_delay = 0.f;

        desc.iterate = 0;

        desc.type = EST_TIMER;
        desc.dead = false;
        desc.freeze = false;
        desc.iterate_invalide = true;

        m_schedulesAdd.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScheduleManager::exist( uint32_t _id ) const
    {
        ListSchedules::const_iterator it_find =
            std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind( _id ) );

        if( it_find != m_schedules.end() )
        {
            return true;
        }

        ListSchedules::const_iterator it_find_add =
            std::find_if( m_schedulesAdd.begin(), m_schedulesAdd.end(), FScheduleFind( _id ) );

        if( it_find_add != m_schedulesAdd.end() )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScheduleManager::refresh( uint32_t _id )
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
    bool ScheduleManager::remove( uint32_t _id )
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
    void ScheduleManager::removeAll()
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
    bool ScheduleManager::removeSchedule_( ScheduleEventDesc & _event )
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
                ScheduleEventInterfacePtr listener = _event.event;
                _event.event = nullptr;

                listener->onScheduleStop( _event.id );
            }break;
        case EST_TIMER:
            {
                ScheduleTimerInterfacePtr listener = _event.timer;
                _event.timer = nullptr;
                _event.pipe = nullptr;

                listener->onScheduleStop( _event.id );
            }break;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ScheduleManager::update( const UpdateContext * _context )
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

        ListSchedules::iterator it_erase = std::remove_if( m_schedules.begin(), m_schedules.end(), FScheduleDead() );
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

                    desc.event->onScheduleComplete( desc.id );

                    TIMELINE_SERVICE()
                        ->endOffset();
                }break;
            case EST_TIMER:
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

                                desc.timer->onScheduleComplete( desc.id );

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

                        desc.timer->onScheduleUpdate( desc.id, iterate, desc.delay );

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
    bool ScheduleManager::freeze( uint32_t _id, bool _freeze )
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
    bool ScheduleManager::isFreeze( uint32_t _id ) const
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
    void ScheduleManager::freezeAll( bool _freeze )
    {
        m_freezeAll = _freeze;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScheduleManager::isFreezeAll() const
    {
        return m_freezeAll;
    }
    //////////////////////////////////////////////////////////////////////////
    float ScheduleManager::getTimePassed( uint32_t _id ) const
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
    float ScheduleManager::getTimeLeft( uint32_t _id ) const
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
    void ScheduleManager::setSpeedFactor( float _factor )
    {
        m_speedFactor = _factor;
    }
    //////////////////////////////////////////////////////////////////////////
    float ScheduleManager::getSpeedFactor() const
    {
        return m_speedFactor;
    }
    //////////////////////////////////////////////////////////////////////////
    float ScheduleManager::getTime() const
    {
        return m_time;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScheduleManager::findScheduleEvent_( uint32_t _id, ScheduleEventDesc *& _desc )
    {
        ListSchedules::iterator it_find =
            std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind( _id ) );

        if( it_find != m_schedules.end() )
        {
            _desc = &*it_find;

            return true;
        }

        ListSchedules::iterator it_find_add =
            std::find_if( m_schedulesAdd.begin(), m_schedulesAdd.end(), FScheduleFind( _id ) );

        if( it_find_add != m_schedulesAdd.end() )
        {
            _desc = &*it_find_add;

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScheduleManager::findScheduleEvent_( uint32_t _id, const ScheduleEventDesc *& _desc ) const
    {
        ListSchedules::const_iterator it_find =
            std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind( _id ) );

        if( it_find != m_schedules.end() )
        {
            _desc = &*it_find;

            return true;
        }

        ListSchedules::const_iterator it_find_add =
            std::find_if( m_schedulesAdd.begin(), m_schedulesAdd.end(), FScheduleFind( _id ) );

        if( it_find_add != m_schedulesAdd.end() )
        {
            _desc = &*it_find_add;

            return true;
        }

        return false;
    }
}
