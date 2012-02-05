#	include "ScheduleManager.h"

#	include "EventManager.h"
#	include "ScriptEngine.h"
#	include "LogEngine.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		struct FScheduleFind
		{
			FScheduleFind( size_t _id )
				: m_id(_id)
			{
			}

			bool operator()( const ScheduleManager::ScheduleEvent & _event ) const
			{
				return _event.id == m_id;
			}

			size_t m_id;
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
	}	
	//////////////////////////////////////////////////////////////////////////
	ScheduleManager::ScheduleManager()
		: m_enumerator(0)
		, m_freeze(false)
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
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ScheduleManager::schedule( float _timing, PyObject * _func )
	{
		ScheduleEvent event_;

		event_.dead = false;
		event_.timing = _timing * 1000.f;
		event_.script = _func;
		event_.id = ++m_enumerator;
		event_.freeze = m_freeze;
		event_.skip = false;

		ScriptEngine::incref( _func );

		m_schedules.push_back( event_ );

		return event_.id;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScheduleManager::skip( size_t _id )
	{
		TListSchedules::iterator it_find = 
			std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind(_id) );

		if( it_find == m_schedules.end() )
		{
			return;
		}
		
		it_find->skip = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScheduleManager::remove( size_t _id )
	{
		TListSchedules::iterator it_find = 
			std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind(_id) );

		if( it_find == m_schedules.end() )
		{
			return;
		}
		
		it_find->dead = true;
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
		if( m_schedules.empty() == true )
		{
			return;
		}

		for( TListSchedules::iterator 
			it = m_schedules.begin(),
			it_end = m_schedules.end();
		it != it_end;
		++it)
		{
			if( it->dead == true )
			{
				this->callEvent_( *it, true );

				continue;
			}

			if( it->freeze )
			{
				continue;
			}

			if( it->timing < _timing )
			{
				it->dead = true;

				this->callEvent_( *it, false );
			}
			else
			{
				it->timing -= _timing;
			}
		}

		TListSchedules::iterator it_erase = std::remove_if( m_schedules.begin(), m_schedules.end(), FScheduleDead());
		m_schedules.erase( it_erase, m_schedules.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void ScheduleManager::freeze( size_t _id, bool _freeze )
	{
		TListSchedules::iterator it_find = 
			std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind(_id) );

		if( it_find == m_schedules.end() )
		{
			return;	
		}
		
		it_find->freeze = _freeze;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ScheduleManager::isFreeze( size_t _id ) const
	{
		TListSchedules::const_iterator it_find = 
			std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind(_id) );

		if( it_find == m_schedules.end() )
		{
			MENGE_LOG_ERROR("ScheduleManager: isFreeze invalid id - '%d'"
				, _id 
				);

			return false;
		}

		return it_find->freeze;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScheduleManager::freezeAll( bool _freeze )
	{
		m_freeze = _freeze;

		for( TListSchedules::iterator 
			it = m_schedules.begin(), 
			it_end = m_schedules.end();
		it != it_end;
		++it )
		{
			it->freeze = _freeze;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float ScheduleManager::time( size_t _id ) const
	{
		TListSchedules::const_iterator it_find = 
			std::find_if( m_schedules.begin(), m_schedules.end(), FScheduleFind(_id) );

		if( it_find == m_schedules.end() )
		{
			return 0.f;
		}

		return it_find->timing * 0.001f;
	}
	//////////////////////////////////////////////////////////////////////////
	void ScheduleManager::callEvent_( const ScheduleEvent & _event, bool _isEnd )
	{
		EventManager::get()
			->addEventFormat( EVENT_SCHEDULE, _event.script, "(iO)", _event.id, pybind::get_bool(_isEnd) );		
	}
}
