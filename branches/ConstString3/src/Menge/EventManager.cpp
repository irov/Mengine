#	include "EventManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static const int C_EVENT_STACK_SIZE = 100;
	//////////////////////////////////////////////////////////////////////////
	EventManager::EventManager()
	{
		m_events.reserve( C_EVENT_STACK_SIZE );
	}
	//////////////////////////////////////////////////////////////////////////
	EventManager::~EventManager()
	{
		for( TVectorEvents::iterator
			it = m_events.begin(),
			it_end = m_events.end();
		it != it_end;
		++it )
		{
			pybind::decref( it->method );
			pybind::decref( it->args );
		}

		m_events.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void EventManager::addEvent( EEventName _event, PyObject * _method, PyObject * _args )
	{
		Event ev;
		ev.event = _event;
		ev.method = _method;
		ev.args = _args;
		
		m_events.push_back( ev );
	}
	//////////////////////////////////////////////////////////////////////////
	void EventManager::update()
	{
		for( TVectorEvents::iterator
			it = m_events.begin(),
			it_end = m_events.end();
		it != it_end;
		++it )
		{
			pybind::call_native( it->method, it->args );
			pybind::decref( it->method );
			pybind::decref( it->args );
		}

		m_events.clear();
	}
}