#	include "TimingManager.h"

#   include "Interface/ServiceInterface.h"

#	include "Logger/Logger.h"

#	include <algorithm>

#	include "Math/utils.h"

namespace Menge
{
	namespace
	{
		struct FTimingFind
		{
			FTimingFind( uint32_t _id )
				: m_id(_id)
			{
			}

			bool operator()( const TimingEventDesc & _event ) const
			{
				return _event.id == m_id;
			}

			uint32_t m_id;
		};

		struct FTimingDead
		{
			bool operator ()( const TimingEventDesc & _event ) const
			{
				return _event.dead;				
			}
		};	
	}
	//////////////////////////////////////////////////////////////////////////
	TimingManager::TimingManager()
		: m_serviceProvider(nullptr)
		, m_speedFactor(1.f)
        , m_enumerator(0)
		, m_freeze(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TimingManager::~TimingManager()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void TimingManager::initialize( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	uint32_t TimingManager::timing( float _delay, const TimingListenerInterfacePtr & _listener )
	{
		if( _delay < 0.f || mt::cmp_f_z( _delay ) == true )
		{
			LOGGER_ERROR(m_serviceProvider)("TimingManager::timing invalid delay %f"
				, _delay
				);

			return 0;
		}

		TimingEventDesc desc;

		desc.listener = _listener;
		
		desc.timing = 0.f;
		desc.delay = _delay * 1000.f;

		uint32_t new_id = ++m_enumerator;

		desc.id = new_id;
		desc.dead = false;
		desc.freeze = m_freeze;

		m_timings.push_back(desc);

		return desc.id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TimingManager::findTimigEvent_( uint32_t _id, const TimingEventDesc *& _desc ) const
	{
		TListTimings::const_iterator it_find = 
			std::find_if( m_timings.begin(), m_timings.end(), FTimingFind(_id) );

		if( it_find == m_timings.end() )
		{
			return false;	
		}

		_desc = &*it_find;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TimingManager::findTimigEvent_( uint32_t _id, TimingEventDesc *& desc )
	{
		TListTimings::iterator it_find = 
			std::find_if( m_timings.begin(), m_timings.end(), FTimingFind(_id) );

		if( it_find == m_timings.end() )
		{
			return false;	
		}

		desc = &*it_find;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TimingManager::remove( uint32_t _id )
	{
		TimingEventDesc * event;
		if( this->findTimigEvent_( _id, event ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("TimingManager::remove not found timing %d"
				, _id
				);

			return false;
		}

		this->destroyTiming_( *event );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TimingManager::removeAll()
	{
		for( TListTimings::iterator 
			it = m_timings.begin(), 
			it_end = m_timings.end();
		it != it_end;
		++it )
		{
			this->destroyTiming_( *it );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TimingManager::freeze( uint32_t _id, bool _freeze )
	{
		TimingEventDesc * event;
		if( this->findTimigEvent_( _id, event ) == false )
		{
			return;
		}

		event->freeze = _freeze;
	}
	//////////////////////////////////////////////////////////////////////////
	void TimingManager::freezeAll( bool _freeze )
	{
		m_freeze = _freeze;

		for( TListTimings::iterator 
			it = m_timings.begin(), 
			it_end = m_timings.end();
		it != it_end;
		++it )
		{
			it->freeze = _freeze;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool TimingManager::isFreeze( uint32_t _id ) const
	{
		const TimingEventDesc * event;
		if( this->findTimigEvent_( _id, event ) == false )
		{
			return false;
		}

		return event->freeze;
	}
	//////////////////////////////////////////////////////////////////////////
	void TimingManager::setSpeedFactor( float _factor )
	{
		m_speedFactor = _factor;
	}
	//////////////////////////////////////////////////////////////////////////
	float TimingManager::getSpeedFactor() const
	{
		return m_speedFactor;
	}
	//////////////////////////////////////////////////////////////////////////
	void TimingManager::update( float _current, float _timing )
	{
        (void)_current;

		if( m_timings.empty() == true )
		{
			return;
		}

		float total_timing = _timing * m_speedFactor;

		for( TListTimings::iterator 
			it = m_timings.begin(),
			it_end = m_timings.end();
		it != it_end;
		++it )
		{
			TimingEventDesc & desc = *it;

			if( desc.dead == true )
			{				
				continue;
			}

			if( desc.freeze == true )
			{
				continue;
			}

			desc.timing += total_timing;
			
			while( desc.timing > desc.delay )
			{
				desc.timing -= desc.delay;

				if( desc.listener->onTimingUpdate( desc.id, desc.delay ) == true )
				{
					this->destroyTiming_( desc );

					break;
				}
			}
		}

		TListTimings::iterator it_erase = std::remove_if( m_timings.begin(), m_timings.end(), FTimingDead());
		m_timings.erase( it_erase, m_timings.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void TimingManager::destroyTiming_( TimingEventDesc & _desc )
	{
		if( _desc.dead == false )
		{
			_desc.dead = true;

			TimingListenerInterfacePtr listener = _desc.listener;
			_desc.listener = nullptr;

			listener->onTimingStop( _desc.id );
		}		
	}
}
