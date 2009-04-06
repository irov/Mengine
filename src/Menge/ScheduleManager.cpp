#	include "ScheduleManager.h"

#	include "ScriptEngine.h"

#	include <algorithm>

namespace Menge
{
	struct FScheduleFind
	{
		FScheduleFind( std::size_t _id )
			: m_id(_id)
		{
		}

		bool operator()( const ScheduleManager::ScheduleEvent & _event ) const
		{
			return _event.id == m_id;
		}

		std::size_t m_id;
	};
	
	struct FScheduleDead
	{
		bool operator ()( const ScheduleManager::ScheduleEvent & _event ) const
		{
			if( _event.dead )
			{
				ScriptEngine::decref( _event.script );
				return true;
			}
			
			return false;
		}
	};	
	
	struct FScheduleUpdating
	{
		void operator ()( ScheduleManager::ScheduleEvent & _event ) const
		{
			_event.updating = false;
		}
	};
	
	//////////////////////////////////////////////////////////////////////////
	ScheduleManager::ScheduleManager()
		: m_schedulesID(0)
		, m_updating(false)
		, m_updatable(true)
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
			ScriptEngine::decref( it->script );
		}

		for( TListSchedules::const_iterator
			it = m_timerSchedules.begin(),
			it_end = m_timerSchedules.end();
		it != it_end;
		++it )
		{
			ScriptEngine::decref( it->script );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ScheduleManager::schedule( float _timing, PyObject * _func )
	{
		ScheduleEvent event_;

		event_.dead = false;
		event_.updating = m_updating;
		event_.timing = _timing;
		event_.script = _func;
		event_.id = ++m_schedulesID;
		event_.paused = !m_updatable;

		ScriptEngine::incref( _func );

		m_schedules.push_back( event_ );

		return event_.id;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScheduleManager::remove( std::size_t _id )
	{
			
		TListSchedules::iterator it_find = 
			std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind(_id) );

		if( it_find != m_schedules.end() )
		{
			it_find->dead = true;
		}
		else
		{
			it_find = std::find_if( m_timerSchedules.begin(), m_timerSchedules.end(), FScheduleFind(_id) );

			if( it_find != m_timerSchedules.end() )
			{
				it_find->dead = true;
			}
		}
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
			it->dead = true;
		}

		for( TListSchedules::iterator 
			it = m_timerSchedules.begin(),
			it_end = m_timerSchedules.end();
		it != it_end;
		++it)
		{
			it->dead = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ScheduleManager::update( float _timing )
	{
		m_updating = true;

		for( TListSchedules::iterator 
			it = m_schedules.begin(),
			it_end = m_schedules.end();
		it != it_end;
		++it)
		{
			if( it->dead || it->updating || it->paused )
			{
				continue;
			}
			
			if( it->timing < _timing )
			{
				Holder<ScriptEngine>::hostage()
					->callFunction( it->script, "()" );

				it->dead = true;
			}
			else
			{
				it->timing -= _timing;
			}
		}

		for( TListSchedules::iterator 
			it = m_timerSchedules.begin(),
			it_end = m_timerSchedules.end();
		it != it_end;
		++it)
		{
			if( it->dead )
			{
				continue;
			}

			if( it->updating )
			{
				continue;
			}

			if( it->timing < _timing )
			{
				float time = _timing;
				while( time > it->timing )
				{
					Holder<ScriptEngine>::hostage()
						->callFunction( it->script, "()" );
					time -= it->timing;
				}
				it->dead = true;
			}
			else
			{
				it->timing -= _timing;
			}
		}

		m_updating = false;

		TListSchedules::iterator it_erase = std::remove_if( m_schedules.begin(), m_schedules.end(), FScheduleDead());

		m_schedules.erase( it_erase, m_schedules.end() );

		std::for_each( m_schedules.begin(), m_schedules.end(), FScheduleUpdating() );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScheduleManager::setUpdatable( bool _upatable )
	{

		m_updatable = _upatable;
		for( TListSchedules::iterator it = m_schedules.begin(), it_end = m_schedules.end();
			it != it_end;
			it++ )
		{
			it->paused = !_upatable;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ScheduleManager::timerSchedule( float _timing, PyObject* _func )
	{
		ScheduleEvent event;

		event.dead = false;
		event.updating = m_updating;
		event.timing = _timing;
		event.script = _func;
		event.id = ++m_schedulesID;

		ScriptEngine::incref( _func );

		m_timerSchedules.push_back( event );

		return event.id;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScheduleManager::freeze( std::size_t _id, bool _freeze )
	{
		/*struct FScheduleFind
		{
			FScheduleFind( std::size_t _id )
				: m_id(_id)
			{
			}

			bool operator()( const ScheduleEvent & _event ) const
			{
				return _event.id == m_id;
			}

			std::size_t m_id;
		};*/

		TListSchedules::iterator it_find = 
			std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind(_id) );

		if( it_find != m_schedules.end() )
		{
			it_find->paused = _freeze;
		}
		else
		{
			it_find = std::find_if( m_timerSchedules.begin(), m_timerSchedules.end(), FScheduleFind(_id) );

			if( it_find != m_timerSchedules.end() )
			{
				it_find->paused = _freeze;
			}
		}
	}
}
