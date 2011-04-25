#	include "EventManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static const int C_EVENT_STACK_SIZE = 100;
	//////////////////////////////////////////////////////////////////////////
	EventManager::EventManager()
	{
		m_proccessEvents.reserve( C_EVENT_STACK_SIZE );
		m_addEvents.reserve( C_EVENT_STACK_SIZE );
	}
	//////////////////////////////////////////////////////////////////////////
	EventManager::~EventManager()
	{
		for( TVectorEvents::iterator
			it = m_addEvents.begin(),
			it_end = m_addEvents.end();
		it != it_end;
		++it )
		{
			pybind::decref( it->method );
			pybind::decref( it->args );
		}
		
		m_addEvents.clear();

		for( TVectorEvents::iterator
			it = m_proccessEvents.begin(),
			it_end = m_proccessEvents.end();
		it != it_end;
		++it )
		{
			pybind::decref( it->method );
			pybind::decref( it->args );
		}

		m_proccessEvents.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void EventManager::addEventFormat( EEventName _event, PyObject * _method, const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		PyObject * args = pybind::build_value_va( _format, valist );

		this->addEvent( _event, _method, args );

		pybind::decref( args );

		va_end( valist );
	}
	//////////////////////////////////////////////////////////////////////////
	void EventManager::addEvent( EEventName _event, PyObject * _method, PyObject * _args )
	{
		Event ev;
		ev.event = _event;
		ev.method = _method;
		ev.args = _args;

		pybind::incref( ev.method );
		pybind::incref( ev.args );
		
		m_addEvents.push_back( ev );
	}
	//////////////////////////////////////////////////////////////////////////
	void EventManager::update()
	{
		std::swap( m_proccessEvents, m_addEvents );

		for( TVectorEvents::iterator
			it = m_proccessEvents.begin(),
			it_end = m_proccessEvents.end();
		it != it_end;
		++it )
		{
			pybind::call_native( it->method, it->args );

			pybind::decref( it->method );
			pybind::decref( it->args );
		}

		m_proccessEvents.clear();
	}
}