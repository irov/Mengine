#	include "Eventable.h"

#	include "Scriptable.h"
#	include "ScriptEngine.h"
#	include "EventManager.h"
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
		this->removeAllEvent();
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

		m_mapEvent.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::registerEvent( EEventName _event, const char * _method, PyObject * _module, bool * _exist )
	{
		if( _module == 0 )
		{
			this->removeEvent_( _event );

			if( _exist != NULL )
			{
				*_exist	= false;
			}
		}

		PyObject * ev = this->getEvent_( _method, _module );

		if( ev == 0 )
		{
			return false;
		}

		if( pybind::is_none(ev) == true )
		{
			pybind::decref( ev );

			this->removeEvent_( _event );

			if( _exist != NULL )
			{
				*_exist	= false;
			}

			return true;
		}

		if( _exist != NULL )
		{
			*_exist	= true;
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
	void Eventable::removeEvent_( EEventName _event )
	{
		TMapEvent::iterator it_find = m_mapEvent.find(_event);

		if( it_find != m_mapEvent.end() )
		{
			ScriptEngine::decref( it_find->second );
			m_mapEvent.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Eventable::getEvent_( const char * _method, PyObject * _dict ) const
	{
		if( _dict == 0 )
		{
			return 0;
		}

		if( pybind::dict_check( _dict ) == true )
		{
			if( pybind::dict_contains( _dict, _method ) == false )
			{
				return 0;
			}

			PyObject * py_event = pybind::dict_get( _dict, _method );
		
			pybind::incref( py_event );

			return py_event;
		}

		if( pybind::has_attr( _dict, _method ) == false )
		{
			return 0;
		}

		PyObject * py_event = pybind::get_attr( _dict, _method );
		
		pybind::incref( py_event );

		return py_event;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::hasEvent( EEventName _event ) const
	{
		TMapEvent::const_iterator it_find = m_mapEvent.find(_event);

		if( it_find == m_mapEvent.end() )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Eventable::getEvent( EEventName _event ) const
	{
		TMapEvent::const_iterator it_find = m_mapEvent.find(_event);

		if( it_find == m_mapEvent.end() )
		{
			return 0;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::callEvent( EEventName _event, const char * _format, ... ) const
	{
		PyObject * pyobj = this->getEvent( _event );

		if( pyobj == 0 )
		{
			return;
		}

		va_list valist;
		va_start(valist, _format);

		PyObject * py = ScriptEngine::get()
			->askFunction( pyobj, _format, valist );

		if( pybind::is_none( py ) == false )
		{ 
			const char * envString = eventToString( _event );
			const char * objRepr = pybind::object_repr( py );

			MENGE_LOG_ERROR( "Warning: Event '%s' must return 'None', but return '%s'"
				, envString
				, objRepr
				);
		}

		pybind::decref( py );

		va_end( valist ); 
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::callEventDeferred( EEventName _event, const char * _format, ... ) const
	{
		PyObject * pyobj = this->getEvent( _event );

		if( pyobj == 0 )
		{
			return;
		}

		va_list valist;
		va_start(valist, _format);

		PyObject * args = pybind::build_value_va( _format, valist );

		EventManager::get()
			->addEvent( _event, pyobj, args );

		pybind::decref( args );

		va_end( valist );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	static bool s_askEventT( T & _result, PyObject * _obj, EEventName _event, const char * _format, va_list _valist )
	{
		PyObject * py = ScriptEngine::get()
			->askFunction( _obj, _format, _valist );

		if( py == 0 )
		{
			return false;
		}

		if( pybind::is_none( py ) == true )
		{ 
			const char * envString = eventToString( _event );
			const char * typeName = typeid(T).name();
			const char * objRepr = pybind::object_repr( py );

			MENGE_LOG_ERROR( "Error: Event '%s' must have return [%s] value '%s'"
				, envString
				, typeName
				, objRepr
				);

			return false;
		}

		_result = pybind::extract<T>( py );

		pybind::decref( py );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::askEvent( bool & _result, EEventName _event, const char * _format, ... ) const
	{
		PyObject * pyobj = this->getEvent( _event );

		if( pyobj == 0 )
		{
			return true;
		}

		va_list valist;
		va_start(valist, _format);

		pybind::incref( pyobj );
		bool successful = s_askEventT( _result, pyobj, _event, _format, valist );
		pybind::decref( pyobj );

		va_end( valist );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::askEvent( Scriptable *& _result, EEventName _event, const char * _format, ... ) const
	{
		PyObject * pyobj = this->getEvent( _event );

		if( pyobj == 0 )
		{
			return true;
		}

		va_list valist;
		va_start(valist, _format);

		bool successful = s_askEventT( _result, pyobj, _event, _format, valist );

		va_end( valist );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::askEvent( PyObject *& _result, EEventName _event, const char * _format, ... ) const
	{
		PyObject * pyobj = this->getEvent( _event );

		if( pyobj == 0 )
		{
			return true;
		}

		va_list valist;
		va_start(valist, _format);

		bool successful = s_askEventT( _result, pyobj, _event, _format, valist );

		va_end( valist );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::askEvent( size_t & _result, EEventName _event, const char * _format, ... ) const
	{
		PyObject * pyobj = this->getEvent( _event );

		if( pyobj == 0 )
		{
			return true;
		}

		va_list valist;
		va_start(valist, _format);

		bool successful = s_askEventT( _result, pyobj, _event, _format, valist );

		va_end( valist );

		return successful;
	}
}
