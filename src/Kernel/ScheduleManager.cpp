#	include "ScheduleManager.h"

#   include "Interface/ServiceInterface.h"
#   include "Interface/TimelineInterface.h"

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
        , m_enumerator(0)
		, m_revision(0)
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
	uint32_t ScheduleManager::event( float _delay, const ScheduleEventInterfacePtr & _listener )
    {
		uint32_t new_id = ++m_enumerator;

        ScheduleEventDesc desc;

		desc.id = new_id;

		desc.event = _listener;
		desc.timer = nullptr;
		desc.pipe = nullptr;

		float timeOffset = TIMELINE_SERVICE( m_serviceProvider )
			->getOffset();

		desc.timing = timeOffset;
		desc.delay = _delay;
		desc.iterate = 0;
		desc.revision = 0;

		desc.type = EST_EVENT;
		desc.dead = false;
		desc.freeze = m_freezeAll;

		m_schedulesAdd.push_back( desc );

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

		float timeOffset = TIMELINE_SERVICE( m_serviceProvider )
			->getOffset();

		desc.timing = timeOffset;
		desc.delay = 0.f;
		desc.iterate = 0;
		desc.revision = 0;

		desc.type = EST_TIMER;
		desc.dead = false;
		desc.freeze = m_freezeAll;

		m_schedulesAdd.push_back( desc );

		return new_id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScheduleManager::exist( uint32_t _id ) const 
	{
		TListSchedules::const_iterator it_find =
			std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind( _id ) );

		if( it_find != m_schedules.end() )
		{
			return true;
		}

		TListSchedules::const_iterator it_find_add =
			std::find_if( m_schedulesAdd.begin(), m_schedulesAdd.end(), FScheduleFind( _id ) );

		if( it_find_add != m_schedulesAdd.end() )
		{
			return true;
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScheduleManager::refresh( uint32_t _id, float _timing )
	{
		ScheduleEventDesc * desc;

		if( this->findScheduleEvent_( _id, desc ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("ScheduleManager::remove not found shedule '%d'"
				, _id
				);

			return false;
		}
		
		desc->timing = _timing;

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
        for( TListSchedules::iterator 
			it = m_schedules.begin(),
			it_end = m_schedules.end();
        it != it_end;
        ++it)
        {
			ScheduleEventDesc & event = *it;

            this->removeSchedule_( event );
        }

		TListSchedules schedulesAdd = m_schedulesAdd;

		for( TListSchedules::iterator
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
    void ScheduleManager::update( float _current, float _timing )
    {
        (void)_current;

		float total_timing = _timing * m_speedFactor;

		m_timing += total_timing;

		++m_revision;

		if( m_freezeAll == true )
		{
			return;
		}

		do
		{
			m_schedules.insert( m_schedules.end(), m_schedulesAdd.begin(), m_schedulesAdd.end() );
			m_schedulesAdd.clear();

			for( TListSchedules::iterator
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

				if( desc.revision == m_revision )
				{
					continue;
				}

				desc.revision = m_revision;

				float old_timing = desc.timing;

				desc.timing += total_timing;

				switch( desc.type )
				{
				case EST_EVENT:
					{
						if( desc.timing < desc.delay )
						{
							continue;
						}

						desc.timing -= desc.delay;

						float timeOffset = desc.delay - old_timing;

						TIMELINE_SERVICE( m_serviceProvider )
							->beginOffset( timeOffset );

						desc.dead = true;

						desc.event->onScheduleComplete( desc.id );

						TIMELINE_SERVICE( m_serviceProvider )
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

								if( delay <= 0.f )
								{
									desc.dead = true;

									break;
								}

								desc.iterate_invalide = false;
								desc.delay = delay;
							}

							if( desc.timing < desc.delay )
							{
								break;
							}

							acc_delay += desc.delay;

							float timeOffset = acc_delay - old_timing;

							uint32_t iterate = desc.iterate;

							desc.timing -= desc.delay;

							desc.iterate++;
							desc.iterate_invalide = true;

							TIMELINE_SERVICE( m_serviceProvider )
								->beginOffset( timeOffset );

							desc.timer->onScheduleUpdate( desc.id, iterate, desc.delay );

							TIMELINE_SERVICE( m_serviceProvider )
								->endOffset();
						}
					}break;
				}
			}			
		}
		while( m_schedulesAdd.empty() == false );

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

		if( it_find != m_schedules.end() )
		{
			_desc = &*it_find;

			return true;
		}
				
		TListSchedules::iterator it_find_add =
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
		TListSchedules::const_iterator it_find =
			std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind( _id ) );

		if( it_find != m_schedules.end() )
		{
			_desc = &*it_find;

			return true;
		}

		TListSchedules::const_iterator it_find_add =
			std::find_if( m_schedulesAdd.begin(), m_schedulesAdd.end(), FScheduleFind( _id ) );

		if( it_find_add != m_schedulesAdd.end() )
		{
			_desc = &*it_find_add;

			return true;
		}

		return false;
    }
}
