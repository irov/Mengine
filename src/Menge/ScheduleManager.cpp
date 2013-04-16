#	include "ScheduleManager.h"

#   include "Interface/ServiceInterface.h"

#	include "Logger/Logger.h"

#	include <algorithm>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        struct FScheduleFind
        {
            FScheduleFind( size_t _id )
                : m_id(_id)
            {
            }

            bool operator()( const ScheduleManager::ScheduleEvent & _event ) const
            {
                return _event.id == m_id;
            }

            size_t m_id;
        };

        struct FScheduleDead
        {
            bool operator ()( const ScheduleManager::ScheduleEvent & _event ) const
            {
                if( _event.dead )
                {
                    _event.listener->destroyScheduleListener();

                    return true;
                }

                return false;
            }
        };	
    }	
    //////////////////////////////////////////////////////////////////////////
    ScheduleManager::ScheduleManager()
        : m_serviceProvider(NULL)
        , m_enumerator(0)
        , m_freeze(false)
    {

    }
    //////////////////////////////////////////////////////////////////////////
    ScheduleManager::~ScheduleManager()
    {
        for( TListSchedules::const_iterator
            it = m_schedules.begin(),
            it_end = m_schedules.end();
        it != it_end;
        ++it )
        {
            ScheduleListener * listener = it->listener;

            listener->destroyScheduleListener();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ScheduleManager::initialize( ServiceProviderInterface * _serviceProvider ) 
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ScheduleManager::schedule( float _timing, ScheduleListener * _listener )
    {
        ScheduleEvent event;

        event.dead = false;
        event.timing = _timing * 1000.f;
        event.listener = _listener;
        event.id = ++m_enumerator;
        event.freeze = m_freeze;

        m_schedules.push_back( event );

        return event.id;
    }
    //////////////////////////////////////////////////////////////////////////
    void ScheduleManager::remove( size_t _id )
    {
        const ScheduleEvent * event = this->findEvent_( _id );

        if( event == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("ScheduleManager::remove not found shedule '%d'"
                , _id
                );

            return;
        }

        this->removeEvent_( *event );
    }
    //////////////////////////////////////////////////////////////////////////
    void ScheduleManager::removeAll()
    {
        TListSchedules schedules = m_schedules;

        for( TListSchedules::iterator 
            it = schedules.begin(),
            it_end = schedules.end();
        it != it_end;
        ++it)
        {
            this->removeEvent_( *it );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ScheduleManager::removeEvent_( const ScheduleEvent & _event )
    {
        if( _event.dead == true )
        {
            return;
        }

        _event.dead = true;

        _event.listener->onScheduleStop( _event.id );
    }
    //////////////////////////////////////////////////////////////////////////
    void ScheduleManager::update( float _current, float _timing )
    {
        (void)_current;

        if( m_schedules.empty() == true )
        {
            return;
        }

        for( TListSchedules::size_type 
            it = 0,
            it_end = m_schedules.size();
        it != it_end;
        ++it )
        {
            ScheduleEvent & event = m_schedules[it];

            if( event.dead == true )
            {
                continue;
            }

            if( event.freeze )
            {
                continue;
            }

            if( event.timing < _timing )
            {
                event.dead = true;

                event.listener->onScheduleComplete( event.id );
            }
            else
            {
                event.timing -= _timing;
            }
        }

        TListSchedules::iterator it_erase = std::remove_if( m_schedules.begin(), m_schedules.end(), FScheduleDead());
        m_schedules.erase( it_erase, m_schedules.end() );
    }
    //////////////////////////////////////////////////////////////////////////
    void ScheduleManager::freeze( size_t _id, bool _freeze )
    {
        const ScheduleEvent * event = this->findEvent_( _id );

        if( event == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("ScheduleManager::freeze not found shedule '%d'"
                , _id
                );

            return;	
        }

        event->freeze = _freeze;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScheduleManager::isFreeze( size_t _id ) const
    {
        const ScheduleEvent * event = this->findEvent_( _id );

        if( event == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("ScheduleManager::isFreeze not found shedule '%d'"
                , _id
                );

            return false;
        }

        return event->freeze;
    }
    //////////////////////////////////////////////////////////////////////////
    void ScheduleManager::freezeAll( bool _freeze )
    {
        m_freeze = _freeze;

        for( TListSchedules::iterator 
            it = m_schedules.begin(), 
            it_end = m_schedules.end();
        it != it_end;
        ++it )
        {
            it->freeze = _freeze;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float ScheduleManager::time( size_t _id ) const
    {
        const ScheduleEvent * event = this->findEvent_( _id );

        if( event == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("ScheduleManager::time not found shedule '%d'"
                , _id
                );

            return 0.f;
        }

        float adapt_timing = event->timing * 0.001f; //Bullshit

        return adapt_timing;
    }
    //////////////////////////////////////////////////////////////////////////
    const ScheduleManager::ScheduleEvent * ScheduleManager::findEvent_( size_t _id ) const
    {
        TListSchedules::const_iterator it_find = 
            std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind(_id) );

        if( it_find == m_schedules.end() )
        {
            return NULL;
        }

        return &*it_find;
    }
}
