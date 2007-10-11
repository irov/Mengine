#	include "ScheduleManager.h"

#	include "ScriptEngine.h"

#	include <algorithm>

namespace Menge
{
	struct ScheduleEvent
	{
		bool dead;
		size_t id;
		size_t timing;
		PyObject * script;
	};

	//////////////////////////////////////////////////////////////////////////
	ScheduleManager::ScheduleManager()
		: m_schedulesID(0)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	size_t ScheduleManager::schedule( size_t _timing, PyObject * _func )
	{
		ScheduleEvent * event = new ScheduleEvent;

		event->dead = false;
		event->timing = _timing;
		event->script = _func;
		event->id = ++m_schedulesID;

		ScriptEngine::incref( _func );

		m_schedules.push_back( event );

		return event->id;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScheduleManager::remove( size_t _id )
	{
		struct FScheduleFind
		{
			FScheduleFind( size_t _id )
				: m_id(_id)
			{
			}

			bool operator()( ScheduleEvent * _event )
			{
				return _event->id == m_id;
			}

			size_t m_id;
		};
			
		TSchedules::iterator it_find = 
			std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind(_id) );

		if( it_find != m_schedules.end() )
		{
			(*it_find)->dead = true;
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void ScheduleManager::update( size_t _timing )
	{
		for each( ScheduleEvent * _event in m_schedules )
		{
			if( _event->dead )
			{
				continue;
			}

			if( _event->timing < _timing )
			{
				Holder<ScriptEngine>::hostage()
					->callFunction( _event->script, "()" );

				_event->dead = true;
			}
			else
			{
				_event->timing -= _timing;
			}
		}

		struct FScheduleDead
		{
			bool operator ()( ScheduleEvent * _event )
			{
				return _event->dead;
			}
		};

		TSchedules::iterator it_erase = std::remove_if( m_schedules.begin(), m_schedules.end(), FScheduleDead());

		for( TSchedules::iterator 
			it = it_erase,
			it_end = m_schedules.end();
		it != it_end;
		++it)
		{
			ScriptEngine::decref( (*it)->script );
		}

		m_schedules.erase( it_erase, m_schedules.end() );
	}
}