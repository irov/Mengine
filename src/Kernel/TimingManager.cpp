#	include "TimingManager.h"

#   include "Interface/ServiceInterface.h"

#	include "Logger/Logger.h"

#	include <algorithm>

namespace Menge
{
	namespace
	{
		struct FTimingFind
		{
			FTimingFind( size_t _id )
				: m_id(_id)
			{
			}

			bool operator()( const TimingEventDesc & _event ) const
			{
				return _event.id == m_id;
			}

			size_t m_id;
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
        , m_enumerator(0)
		, m_freeze(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TimingManager::~TimingManager()
	{
        for( TListTimings::const_iterator
            it = m_timings.begin(),
            it_end = m_timings.end();
        it != it_end;
        ++it )
        {
            TimingListenerInterface * listener = it->listener;

			if( listener == nullptr )
			{
				continue;
			}

            listener->deleteTimingListener();
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void TimingManager::initialize( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	size_t TimingManager::timing( bool _portions, bool _global, float _delay, TimingListenerInterface * _listener )
	{
		TimingEventDesc desc;

		desc.listener = _listener;
		
		desc.timing = 0.f;
		desc.delay = _delay * 1000.f;

		size_t new_id = ++m_enumerator;

		desc.id = new_id;
		desc.dead = false;
		desc.freeze = m_freeze;
		desc.portions = _portions;
		desc.global = _global;
				
		m_timings.push_back(desc);

		return desc.id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TimingManager::findTimigEvent_( size_t _id, const TimingEventDesc *& _desc ) const
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
	bool TimingManager::findTimigEvent_( size_t _id, TimingEventDesc *& desc )
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
	bool TimingManager::remove( size_t _id )
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
	void TimingManager::removeAll( bool _global )
	{
		for( TListTimings::iterator 
			it = m_timings.begin(), 
			it_end = m_timings.end();
		it != it_end;
		++it )
		{
			if( _global == false && it->global == true )
			{
				continue;
			}

			this->destroyTiming_( *it );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TimingManager::freeze( size_t _id, bool _freeze )
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
	bool TimingManager::isFreeze( size_t _id ) const
	{
		const TimingEventDesc * event;
		if( this->findTimigEvent_( _id, event ) == false )
		{
			return false;
		}

		return event->freeze;
	}
	//////////////////////////////////////////////////////////////////////////
	void TimingManager::update( float _current, float _timing )
	{
        (void)_current;

		if( m_timings.empty() == true )
		{
			return;
		}

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

			desc.timing += _timing;
			
			if( desc.portions == true )
			{
				while( desc.timing > desc.delay )
				{
					desc.timing -= desc.delay;

					if( desc.listener->updateTiming( desc.id, desc.delay ) == true )
					{
						this->destroyTiming_( desc );

						break;
					}
				}
			}
			else
			{
				if( desc.listener->updateTiming( desc.id, _timing ) == true )
				{
					this->destroyTiming_( desc );
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

			TimingListenerInterface * listener = _desc.listener;
			_desc.listener = nullptr;

			listener->removeTiming( _desc.id );
			listener->deleteTimingListener();
		}		
	}
}
