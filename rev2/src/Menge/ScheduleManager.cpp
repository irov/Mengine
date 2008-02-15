#	include "ScheduleManager.h"

#	include "ScriptEngine.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ScheduleManager::ScheduleManager()
		: m_schedulesID(0)
		, m_updating(false)
		, m_updatable(true)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	size_t ScheduleManager::schedule( float _timing, PyObject * _func )
	{
		ScheduleEvent event;


		event.dead = false;
		event.updating = m_updating;
		event.timing = _timing;
		event.script = _func;
		event.id = ++m_schedulesID;

		ScriptEngine::incref( _func );

		m_schedules.push_back( event );

		return event.id;
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

			bool operator()( const ScheduleEvent & _event ) const
			{
				return _event.id == m_id;
			}

			size_t m_id;
		};
			
		TListSchedules::iterator it_find = 
			std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind(_id) );

		if( it_find != m_schedules.end() )
		{
			it_find->dead = true;
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
	}
	//////////////////////////////////////////////////////////////////////////
	void ScheduleManager::update( float _timing )
	{
		if( !m_updatable )
		{
			return;
		}

		m_updating = true;

		for( TListSchedules::iterator 
			it = m_schedules.begin(),
			it_end = m_schedules.end();
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
				Holder<ScriptEngine>::hostage()
					->callFunction( it->script, "()" );

				it->dead = true;
			}
			else
			{
				it->timing -= _timing;
			}
		}

		m_updating = false;


		struct FScheduleDead
		{
			bool operator ()( const ScheduleEvent & _event ) const
			{
				if( _event.dead )
				{
					ScriptEngine::decref( _event.script );
					return true;
				}
				
				return false;
			}
		};

		TListSchedules::iterator it_erase = std::remove_if( m_schedules.begin(), m_schedules.end(), FScheduleDead());

		m_schedules.erase( it_erase, m_schedules.end() );

		struct FScheduleUpdating
		{
			void operator ()( ScheduleEvent & _event ) const
			{
				_event.updating = false;
			}
		};

		std::for_each( m_schedules.begin(), m_schedules.end(), FScheduleUpdating() );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScheduleManager::setUpdatable( bool _upatable )
	{
		m_updatable = _upatable;
	}
}