#	include "TimingManager.h"

#   include "Interface/ServiceInterface.h"

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

			bool operator()( const TimingManager::TimingEvent & _event ) const
			{
				return _event.id == m_id;
			}

			size_t m_id;
		};

		struct FTimingDead
		{
			bool operator ()( const TimingManager::TimingEvent & _event ) const
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
            TimingListener * listener = it->listener;

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
	size_t TimingManager::timing( bool _portions, bool _global, float _delay, TimingListener * _listener )
	{
		TimingEvent event;

		event.listener = _listener;
		
		event.timing = 0.f;
		event.delay = _delay * 1000.f;

		size_t new_id = ++m_enumerator;

		event.id = new_id;
		event.dead = false;
		event.freeze = m_freeze;
		event.portions = _portions;
		event.global = _global;
				
		m_timings.push_back(event);

		return event.id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TimingManager::findTimigEvent_( size_t _id, const TimingEvent *& _event ) const
	{
		TListTimings::const_iterator it_find = 
			std::find_if( m_timings.begin(), m_timings.end(), FTimingFind(_id) );

		if( it_find == m_timings.end() )
		{
			return false;	
		}

		_event = &*it_find;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TimingManager::findTimigEvent_( size_t _id, TimingEvent *& _event )
	{
		TListTimings::iterator it_find = 
			std::find_if( m_timings.begin(), m_timings.end(), FTimingFind(_id) );

		if( it_find == m_timings.end() )
		{
			return false;	
		}

		_event = &*it_find;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TimingManager::remove( size_t _id )
	{
		TimingEvent * event;
		if( this->findTimigEvent_( _id, event ) == false )
		{
			return;
		}

		this->destroyTiming_( *event );
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
		TimingEvent * event;
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
		const TimingEvent * event;
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

		for( TListTimings::size_type 
			it = 0,
			it_end = m_timings.size();
		it != it_end;
		++it )
		{
			TimingEvent event = m_timings[it];

			if( event.dead == true )
			{				
				continue;
			}

			if( event.freeze == true )
			{
				continue;
			}

			event.timing += _timing;
			
			if( event.portions == true )
			{
				while( event.timing > event.delay )
				{
					event.timing -= event.delay;

					if( event.listener->updateTiming( event.id, event.delay ) == true )
					{
						this->destroyTiming_( event );

						break;
					}
				}
			}
			else
			{
				if( event.listener->updateTiming( event.id, _timing ) == true )
				{
					this->destroyTiming_( event );
				}
			}
		}

		TListTimings::iterator it_erase = std::remove_if( m_timings.begin(), m_timings.end(), FTimingDead());
		m_timings.erase( it_erase, m_timings.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void TimingManager::destroyTiming_( TimingEvent & _event )
	{
		if( _event.dead == false )
		{
			_event.dead = true;

			TimingListener * listener = _event.listener;
			_event.listener = nullptr;

			listener->removeTiming( _event.id );
			listener->deleteTimingListener();			
		}		
	}
}
