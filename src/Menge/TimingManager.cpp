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
				if( _event.dead == false )
				{
					return false;
				}

				if( _event.remove == true )
				{
					_event.listener->removeTiming( _event.id );
				}
				
				_event.listener->deleteTimingListener();
				
				return true;
			}
		};	
	}
	//////////////////////////////////////////////////////////////////////////
	TimingManager::TimingManager()
		: m_serviceProvider(NULL)
        , m_enumerator(0)
		, m_freeze(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TimingManager::~TimingManager()
	{
        for( TListTimings::const_iterator
            it = m_events.begin(),
            it_end = m_events.end();
        it != it_end;
        ++it )
        {
            TimingListener * listener = it->listener;

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
		event.remove = false;
		event.dead = false;
		event.freeze = m_freeze;
		event.portions = _portions;
		event.global = _global;
				
		m_events.push_back(event);

		return event.id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TimingManager::findTimigEvent_( size_t _id, const TimingEvent *& _event ) const
	{
		TListTimings::const_iterator it_find = 
			std::find_if( m_events.begin(), m_events.end(), FTimingFind(_id) );

		if( it_find == m_events.end() )
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
			std::find_if( m_events.begin(), m_events.end(), FTimingFind(_id) );

		if( it_find == m_events.end() )
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

		event->remove = true;
		event->dead = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TimingManager::removeAll( bool _global )
	{
		for( TListTimings::iterator 
			it = m_events.begin(), 
			it_end = m_events.end();
		it != it_end;
		++it )
		{
			if( _global == false && it->global == true )
			{
				continue;
			}

			it->remove = true;
			it->dead = true;
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
			it = m_events.begin(), 
			it_end = m_events.end();
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

		if( m_events.empty() == true )
		{
			return;
		}

		for( TListTimings::iterator 
			it = m_events.begin(),
			it_end = m_events.end();
		it != it_end;
		++it)
		{
			if( it->dead == true )
			{
				continue;
			}

			if( it->freeze )
			{
				continue;
			}

			it->timing += _timing;
			
			if( it->portions == true )
			{
				while( it->timing > it->delay )
				{
					it->timing -= it->delay;

					it->dead = it->listener->updateTiming( it->id, it->delay );

					if( it->dead == true )
					{
						break;
					}
				}
			}
			else
			{
				it->dead = it->listener->updateTiming( it->id, _timing );
			}
		}

		TListTimings::iterator it_erase = std::remove_if( m_events.begin(), m_events.end(), FTimingDead());
		m_events.erase( it_erase, m_events.end() );
	}
}
