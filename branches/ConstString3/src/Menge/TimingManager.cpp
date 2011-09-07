#	include "TimingManager.h"

#	include <algorithm>

namespace Menge
{
	namespace
	{
		struct FTimingFind
		{
			FTimingFind( std::size_t _id )
				: m_id(_id)
			{
			}

			bool operator()( const TimingManager::TimingEvent & _event ) const
			{
				return _event.id == m_id;
			}

			std::size_t m_id;
		};

		struct FTimingDead
		{
			bool operator ()( const TimingManager::TimingEvent & _event ) const
			{
				if( _event.dead == false )
				{
					return false;
				}
				
				delete _event.listener;
				
				return true;
			}
		};	
	}
	//////////////////////////////////////////////////////////////////////////
	TimingManager::TimingManager()
		: m_enumerator(0)
		, m_freeze(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TimingManager::~TimingManager()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t TimingManager::timing( bool _portions, float _delay, TimingListener * _listener )
	{
		TimingEvent event;

		event.listener = _listener;
		
		event.timing = 0.f;
		event.delay = _delay * 1000.f;

		event.id = ++m_enumerator;
		event.dead = false;
		event.freeze = m_freeze;
		event.portions = _portions;
				
		m_events.push_back(event);

		return event.id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TimingManager::findTimigEvent_( std::size_t _id, const TimingEvent *& _event ) const
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
	bool TimingManager::findTimigEvent_( std::size_t _id, TimingEvent *& _event )
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
	void TimingManager::remove( std::size_t _id )
	{
		TimingEvent * event;
		if( this->findTimigEvent_( _id, event ) == false )
		{
			return;
		}

		event->dead = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TimingManager::removeAll()
	{
		for( TListTimings::iterator 
			it = m_events.begin(), 
			it_end = m_events.end();
		it != it_end;
		++it )
		{
			it->dead = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TimingManager::freeze( std::size_t _id, bool _freeze )
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
	bool TimingManager::isFreeze( std::size_t _id ) const
	{
		const TimingEvent * event;
		if( this->findTimigEvent_( _id, event ) == false )
		{
			return false;
		}

		return event->freeze;
	}
	//////////////////////////////////////////////////////////////////////////
	void TimingManager::update( float _timing )
	{
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

					it->dead = it->listener->update( it->id, it->delay );

					if( it->dead == true )
					{
						break;
					}
				}
			}
			else
			{
				it->dead = it->listener->update( it->id, _timing );
			}
		}

		TListTimings::iterator it_erase = std::remove_if( m_events.begin(), m_events.end(), FTimingDead());
		m_events.erase( it_erase, m_events.end() );
	}
}
