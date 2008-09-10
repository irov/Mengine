#	include "Eventable.h"

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
	bool Eventable::registerEvent( EEventName _name, const StringA & _method, PyObject * _module )
	{
		TMapEvent::iterator it_find = m_mapEvent.find(_name);

		if( it_find != m_mapEvent.end() )
		{
			return false;
		}

		if( Holder<ScriptEngine>::hostage()
			->hasModuleFunction( _module, _method ) == false )
		{
			return false;
		}

		PyObject * event = Holder<ScriptEngine>::hostage()
			->getModuleFunction( _module, _method );

		if( event == 0 )
		{
			return false;
		}

		ScriptEngine::incref( event );

		m_mapEvent.insert(std::make_pair( _name, event ));

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::registerEvent( EEventName _name, PyObject * _callback )
	{
		TMapEvent::iterator it_find = m_mapEvent.find(_name);

		if( it_find != m_mapEvent.end() )
		{
			return false;
		}

		if( _callback == 0 )
		{
			return false;
		}

		ScriptEngine::incref( _callback );

		m_mapEvent.insert(std::make_pair( _name, _callback ));

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

		Holder<ScriptEngine>::hostage()
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
			Holder<ScriptEngine>::hostage()
			->askFunction( it_find->second, _format, valist );

		va_end( valist );

		if( result == 0 )
		{
			return false;
		}

		if( pybind::convert::is_none( result ) == true )
		{
			MENGE_LOG( MENGE_TEXT("Error: Event '%d' must have return [True/False] value\n")
				, _name
				);

			return false;
		}

		_result = pybind::convert::to_bool( result );

		return _result;
	}
}