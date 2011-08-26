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
				
				delete _event.timing;
				
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
	std::size_t TimingManager::add( Timing * _timing )
	{
		TimingEvent event;

		event.timing = _timing;
		event.id = ++m_enumerator;
		event.dead = false;
		event.freeze = m_freeze;
		
		m_timings.push_back(event);

		return event.id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TimingManager::findTimigEvent_( std::size_t _id, const TimingEvent *& _event ) const
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
	bool TimingManager::findTimigEvent_( std::size_t _id, TimingEvent *& _event )
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
			it = m_timings.begin(), 
			it_end = m_timings.end();
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
			it = m_timings.begin(), 
			it_end = m_timings.end();
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
		if( m_timings.empty() == true )
		{
			return;
		}

		for( TListTimings::iterator 
			it = m_timings.begin(),
			it_end = m_timings.end();
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

			it->dead = it->timing->update( it->id, _timing );
		}

		TListTimings::iterator it_erase = std::remove_if( m_timings.begin(), m_timings.end(), FTimingDead());
		m_timings.erase( it_erase, m_timings.end() );
	}
}
