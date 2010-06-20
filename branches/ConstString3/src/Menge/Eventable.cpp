#	include "Eventable.h"

#	include "Scriptable.h"
#	include "ScriptEngine.h"
#	include "Logger/Logger.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Eventable::Eventable()
	{}
	//////////////////////////////////////////////////////////////////////////
	Eventable::~Eventable()
	{
		removeAllEvent();
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::removeAllEvent()
	{
		for( TMapEvent::iterator
			it = m_mapEvent.begin(),
			it_end = m_mapEvent.end();
		it != it_end;
		++it)
		{
			ScriptEngine::decref( it->second );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::registerEvent( EEventName _event, const char * _method, PyObject * _module )
	{
		PyObject * ev = getEvent_( _method, _module );

		if( ev == 0 )
		{
			TMapEvent::iterator it_find = m_mapEvent.find(_event);

			if( it_find != m_mapEvent.end() )
			{
				ScriptEngine::decref( it_find->second );
				m_mapEvent.erase( it_find );
			}

			return false;
		}

		TMapEvent::iterator it_find = m_mapEvent.find(_event);

		if( it_find == m_mapEvent.end() )
		{
			m_mapEvent.insert(std::make_pair( _event, ev ));
		}
		else
		{
			ScriptEngine::decref( it_find->second );
			it_find->second = ev;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Eventable::getEvent_( const char * _method, PyObject * _module )
	{
		if( ScriptEngine::hostage()
			->hasModuleFunction( _module, _method ) == false )
		{
			return 0;
		}

		PyObject * ev = ScriptEngine::hostage()
			->getModuleFunction( _module, _method.str() );

		return ev;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Eventable::getEvent( EEventName _event )
	{
		TMapEvent::iterator it_find = m_mapEvent.find(_event);

		if( it_find == m_mapEvent.end() )
		{
			return 0;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::callEvent( EEventName _event, const char * _format, ... )
	{
		TMapEvent::iterator it_find = m_mapEvent.find( _event );

		if( it_find == m_mapEvent.end() )
		{
			return;
		}

		va_list valist;
		va_start(valist, _format);

		ScriptEngine::hostage()
			->callFunction( it_find->second, _format, valist );

		va_end( valist ); 
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	static bool s_askEventT( T & _result, PyObject * _obj, EEventName _event, const char * _format, va_list _valist )
	{
		PyObject * py = 
			ScriptEngine::hostage()
			->askFunction( _obj, _format, _valist );

		if( py == 0 )
		{
			return false;
		}

		if( pybind::convert::is_none( py ) == true )
		{ 
			MENGE_LOG_ERROR( "Error: Event '%s' must have return [%s] value '%s'"				
				, eventToString( _event )
				, typeid(T).name()
				, pybind::object_to_string( _obj )
				);

			return false;
		}

		_result = pybind::extract<T>( py );

		pybind::decref( py );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::askEvent( bool & _result, EEventName _event, const char * _format, ... )
	{
		PyObject * pyobj = this->getEvent( _event );

		if( pyobj == 0 )
		{
			return false;
		}

		va_list valist;
		va_start(valist, _format);

		bool successful = s_askEventT( _result, pyobj, _event, _format, valist );

		va_end( valist );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::askEvent( std::size_t & _result, EEventName _event, const char * _format, ... )
	{
		PyObject * pyobj = this->getEvent( _event );

		if( pyobj == 0 )
		{
			return false;
		}

		va_list valist;
		va_start(valist, _format);

		bool successful = s_askEventT( _result, pyobj, _event, _format, valist );

		va_end( valist );

		return successful;
	}
}
