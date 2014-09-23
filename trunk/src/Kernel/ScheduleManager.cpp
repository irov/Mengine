#	include "ScheduleManager.h"

#   include "Interface/ServiceInterface.h"

#	include "Logger/Logger.h"

#	include <algorithm>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        struct FScheduleFind
        {
            FScheduleFind( uint32_t _id )
                : m_id(_id)
            {
            }

            bool operator()( const ScheduleManager::ScheduleEvent & _event ) const
            {
                return _event.id == m_id;
            }

            uint32_t m_id;
        };
        //////////////////////////////////////////////////////////////////////////
        struct FScheduleDead
        {
            bool operator ()( const ScheduleManager::ScheduleEvent & _event ) const
            {
				return _event.dead;
            }
        };	
    }	
    //////////////////////////////////////////////////////////////////////////
    ScheduleManager::ScheduleManager()
        : m_serviceProvider(nullptr)
        , m_enumeratorSchedule(0)
        , m_freezeAll(false)
    {

    }
    //////////////////////////////////////////////////////////////////////////
    ScheduleManager::~ScheduleManager()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ScheduleManager::initialize( ServiceProviderInterface * _serviceProvider ) 
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ScheduleManager::schedule( float _timing, const ScheduleListenerPtr & _listener )
    {
        ScheduleEvent event;

        event.dead = false;
        event.timing = _timing * 1000.f;
        event.listener = _listener;
        event.id = ++m_enumeratorSchedule;
        event.freeze = false;

        m_schedules.push_back( event );

        return event.id;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScheduleManager::remove( uint32_t _id )
    {
        const ScheduleEvent * event = this->findEvent_( _id );

        if( event == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("ScheduleManager::remove not found shedule '%d'"
                , _id
                );

            return false;
        }

        if( this->removeEvent_( *event ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ScheduleManager::remove not alredy remove or complete '%d'"
				, _id
				);

			return false;
		}

		return true;
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
    bool ScheduleManager::removeEvent_( const ScheduleEvent & _event )
    {
        if( _event.dead == true )
        {
            return false;
        }

        _event.dead = true;

        _event.listener->onScheduleStop( _event.id );

		return true;
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

            if( event.freeze == true )
            {
                continue;
            }

			if( m_freezeAll == true )
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
    bool ScheduleManager::freeze( uint32_t _id, bool _freeze )
    {
        const ScheduleEvent * event = this->findEvent_( _id );

        if( event == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("ScheduleManager::freeze not found shedule '%d'"
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
        const ScheduleEvent * event = this->findEvent_( _id );

        if( event == nullptr )
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
        m_freezeAll = _freeze;
    }
	//////////////////////////////////////////////////////////////////////////
	bool ScheduleManager::isFreezeAll() const
	{
		return m_freezeAll;
	}
    //////////////////////////////////////////////////////////////////////////
    float ScheduleManager::time( uint32_t _id ) const
    {
        const ScheduleEvent * event = this->findEvent_( _id );

        if( event == nullptr )
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
    const ScheduleManager::ScheduleEvent * ScheduleManager::findEvent_( uint32_t _id ) const
    {
        TListSchedules::const_iterator it_find = 
            std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind(_id) );

        if( it_find == m_schedules.end() )
        {
            return nullptr;
        }

        return &*it_find;
    }
}
