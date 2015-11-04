#	include "ScheduleManager.h"

#   include "Interface/ServiceInterface.h"

#	include "Logger/Logger.h"

#	include "Math/utils.h"

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
        : m_serviceProvider(nullptr)
		, m_speedFactor(1.f)
		, m_timing(0.f)
        , m_enumeratorSchedule(0)
        , m_freezeAll(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ScheduleManager::~ScheduleManager()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	void ScheduleManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * ScheduleManager::getServiceProvider() const
	{
		return m_serviceProvider;
	}
    //////////////////////////////////////////////////////////////////////////
    uint32_t ScheduleManager::schedule( float _timing, const ScheduleListenerInterfacePtr & _listener )
    {
		uint32_t new_id = ++m_enumeratorSchedule;

        ScheduleEventDesc desc;

		desc.listener = _listener;

		desc.timing = _timing;
		desc.delay = 0.f;
		desc.id = new_id;

		desc.update = false;
		desc.dead = false;
		desc.freeze = m_freezeAll;

        m_schedules.push_back( desc );

		return new_id;
    }
	//////////////////////////////////////////////////////////////////////////
	uint32_t ScheduleManager::timing( float _delay, const ScheduleListenerInterfacePtr & _listener )
	{
		if( _delay < 0.f || mt::equal_f_z( _delay ) == true )
		{
			LOGGER_ERROR( m_serviceProvider )("ScheduleManager::timing invalid delay %f"
				, _delay
				);

			return 0;
		}

		uint32_t new_id = ++m_enumeratorSchedule;

		ScheduleEventDesc desc;

		desc.listener = _listener;

		desc.timing = 0.f;
		desc.delay = _delay;
		desc.id = new_id;

		desc.update = true;
		desc.dead = false;
		desc.freeze = m_freezeAll;

		m_schedules.push_back( desc );

		return new_id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScheduleManager::exist( uint32_t _id ) const 
	{
		TListSchedules::const_iterator it_find =
			std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind( _id ) );

		if( it_find == m_schedules.end() )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScheduleManager::refresh( uint32_t _id, float _timing )
	{
		TListSchedules::iterator it_find =
			std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind( _id ) );

		if( it_find == m_schedules.end() )
		{
			return false;
		}

		ScheduleEventDesc & desc = *it_find;

		desc.timing = _timing;

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool ScheduleManager::remove( uint32_t _id )
    {
        ScheduleEventDesc * desc;

        if( this->findScheduleEvent_( _id, desc ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ScheduleManager::remove not found shedule '%d'"
                , _id
                );

            return false;
        }

        if( this->removeSchedule_( *desc ) == false )
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

		ScheduleListenerInterfacePtr listener = _event.listener;
		_event.listener = nullptr;

		listener->onScheduleStop( _event.id );

		return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ScheduleManager::update( float _current, float _timing )
    {
        (void)_current;

		float total_timing = _timing * m_speedFactor;

		m_timing += total_timing;

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
            ScheduleEventDesc & desc = m_schedules[it];

            if( desc.dead == true )
            {
                continue;
            }

            if( desc.freeze == true )
            {
                continue;
            }

			if( m_freezeAll == true )
			{
				continue;
			}

			if( desc.update == false )
			{
				if( desc.timing > total_timing )
				{
					desc.timing -= total_timing;

					continue;
				}
								
				desc.dead = true;

				desc.listener->onScheduleComplete( desc.id );
			}
			else
			{
				desc.timing += total_timing;

				while( desc.timing > desc.delay )
				{
					desc.timing -= desc.delay;

					if( desc.listener->onScheduleUpdate( desc.id, desc.delay ) == true )
					{
						this->removeSchedule_( desc );

						break;
					}
				}
			}
        }

        TListSchedules::iterator it_erase = std::remove_if( m_schedules.begin(), m_schedules.end(), FScheduleDead());
        m_schedules.erase( it_erase, m_schedules.end() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ScheduleManager::freeze( uint32_t _id, bool _freeze )
    {
        ScheduleEventDesc * event;

        if( this->findScheduleEvent_( _id, event ) == false )
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
        const ScheduleEventDesc * event;

        if( this->findScheduleEvent_( _id, event ) == false )
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
		const ScheduleEventDesc * event;

		if( this->findScheduleEvent_( _id, event ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("ScheduleManager::time not found shedule '%d'"
                , _id
                );

            return 0.f;
        }

        float adapt_timing = event->timing;

        return adapt_timing;
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
	float ScheduleManager::getTiming() const
	{
		return m_timing;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScheduleManager::findScheduleEvent_( uint32_t _id, ScheduleEventDesc *& _desc )
	{
		TListSchedules::iterator it_find = 
			std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind(_id) );

		if( it_find == m_schedules.end() )
		{
			return false;
		}

		_desc = &*it_find;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
    bool ScheduleManager::findScheduleEvent_( uint32_t _id, const ScheduleEventDesc *& _desc ) const
    {
        TListSchedules::const_iterator it_find = 
            std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind(_id) );

        if( it_find == m_schedules.end() )
        {
            return false;
        }

		_desc = &*it_find;

        return true;
    }
}
