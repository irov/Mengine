#	include "Eventable.h"

#	include "Scriptable.h"
#	include "ScriptEngine.h"
#	include "LogEngine.h"

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
	bool Eventable::registerEvent( EEventName _name, const String & _method, Scriptable * _scriptable )
	{
		PyObject * obj = _scriptable->getEmbedding();
		pybind::decref( obj );

		bool result = this->registerEvent( _name, _method, obj );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::registerEvent( EEventName _name, const String & _method, PyObject * _module )
	{
		//ScriptEngine::decref( _module );

		TMapEvent::iterator it_find = m_mapEvent.find(_name);

		if( it_find != m_mapEvent.end() )
		{			
			return false;
		}

		if( ScriptEngine::hostage()
			->hasModuleFunction( _module, _method ) == false )
		{
			return false;
		}

		PyObject * ev = ScriptEngine::hostage()
			->getModuleFunction( _module, _method );

		if( ev == 0 )
		{
			return false;
		}

//		ScriptEngine::incref( ev );

		m_mapEvent.insert(std::make_pair( _name, ev ));

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::registerEvent( EEventName _name, PyObject * _event )
	{
		TMapEvent::iterator it_find = m_mapEvent.find(_name);

		if( it_find != m_mapEvent.end() )
		{
			return false;
		}

		if( _event == 0 )
		{
			return false;
		}

		ScriptEngine::incref( _event );

		m_mapEvent.insert(std::make_pair( _name, _event ));

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Eventable::getEvent( EEventName _name )
	{
		TMapEvent::iterator it_find = m_mapEvent.find(_name);

		if( it_find == m_mapEvent.end() )
		{
			return 0;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::callEvent( EEventName _name, const char * _format, ... )
	{
		TMapEvent::iterator it_find = m_mapEvent.find( _name );

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
	bool Eventable::askEvent( bool & _result, EEventName _name, const char * _format, ... )
	{
		TMapEvent::iterator it_find = m_mapEvent.find( _name );

		if( it_find == m_mapEvent.end() )
		{
			return false;
		}

		va_list valist;
		va_start(valist, _format);

		PyObject * result = 
			ScriptEngine::hostage()
			->askFunction( it_find->second, _format, valist );

		va_end( valist );

		if( result == 0 )
		{
			return false;
		}

		if( pybind::is_none( result ) == true )
		{ 
			MENGE_LOG_ERROR( "Error: Event '%s' must have return [True/False] value '%s'"
				, s_eventToString[_name]
				, pybind::object_repr( it_find->second )
				);

			return false;
		}

		_result = pybind::is_true( result );

		pybind::decref( result );

		return _result;
	}
}
