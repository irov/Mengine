#	include "Eventable.h"

#	include "Scriptable.h"

#   include "Interface/ScriptSystemInterface.h"

#   include "Logger/Logger.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Eventable::Eventable()
	{}
	//////////////////////////////////////////////////////////////////////////
	Eventable::~Eventable()
	{
        for( TMapEvent::iterator
            it = m_mapEvent.begin(),
            it_end = m_mapEvent.end();
        it != it_end;
        ++it)
        {
            pybind::decref( it->second );
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
			pybind::decref( it_find->second );
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
			pybind::decref( it_find->second );
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
		PyObject * pyevent = this->getEvent( _event );

		if( pyevent == 0 )
		{
			return;
		}

		va_list valist;
		va_start(valist, _format);
				
		this->callPyEventVaList( _event, pyevent, _format, valist );
		
		va_end( valist ); 
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::callPyEventVaList( EEventName _event, PyObject * _pyevent, const char * _format, va_list _va ) const
	{
        ServiceProviderInterface * serviceProvider = this->getServiceProvider();

        pybind::incref( _pyevent );

        PyObject * py_result = SCRIPT_SERVICE(serviceProvider)
            ->askFunctionVA( _pyevent, _format, _va );

        pybind::decref( _pyevent );

		if( py_result == 0 )
		{
			return;
		}

		if( pybind::is_none( py_result ) == false )
		{ 
			const char * envString = eventToString( _event );
			const char * objRepr = pybind::object_repr( py_result );

			LOGGER_ERROR(serviceProvider)( "Eventable: Event '%s' must return 'None', but return '%s'"
				, envString
				, objRepr
				);
		}

		pybind::decref( py_result );
	}
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	static bool s_askPyEventVaList( ServiceProviderInterface * _serviceProvider, T & _result, EEventName _event, PyObject * _pyevent, const char * _format, va_list _va )
	{
        pybind::incref( _pyevent );

		PyObject * pyresult = SCRIPT_SERVICE(_serviceProvider)
			->askFunctionVA( _pyevent, _format, _va );

        pybind::decref( _pyevent );

		if( pyresult == 0 )
		{
			return false;
		}

		if( pybind::is_none( pyresult ) == true )
		{ 
			const char * envString = eventToString( _event );
			const char * typeName = typeid(T).name();
			const char * objRepr = pybind::object_repr( pyresult );

			LOGGER_ERROR(_serviceProvider)( "Eventable: '%s' must have return [%s] value '%s'"
				, envString
				, typeName
				, objRepr
				);

			pybind::decref( pyresult );

			return false;
		}

		_result = pybind::extract<T>( pyresult );

		pybind::decref( pyresult );

		return true;		
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::askEvent( bool & _result, EEventName _event, const char * _format, ... ) const
	{
		PyObject * pyevent = this->getEvent( _event );

		if( pyevent == 0 )
		{
			return true;
		}

		va_list valist;
		va_start(valist, _format);

		bool successful = this->askPyEventVaList( _result, _event, pyevent, _format, valist );

		va_end( valist );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::askEvent( Scriptable *& _result, EEventName _event, const char * _format, ... ) const
	{
		PyObject * pyevent = this->getEvent( _event );

		if( pyevent == 0 )
		{
			return true;
		}

		va_list valist;
		va_start(valist, _format);

		bool successful = this->askPyEventVaList( _result, _event, pyevent, _format, valist );

		va_end( valist );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::askEvent( PyObject *& _result, EEventName _event, const char * _format, ... ) const
	{
		PyObject * pyevent = this->getEvent( _event );

		if( pyevent == 0 )
		{
			return true;
		}

		va_list valist;
		va_start(valist, _format);

		bool successful = this->askPyEventVaList( _result, _event, pyevent, _format, valist );

		va_end( valist );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::askEvent( size_t & _result, EEventName _event, const char * _format, ... ) const
	{
		PyObject * pyevent = this->getEvent( _event );

		if( pyevent == 0 )
		{
			return true;
		}

		va_list valist;
		va_start(valist, _format);

		bool successful = this->askPyEventVaList( _result, _event, pyevent, _format, valist );

		va_end( valist );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::askPyEventVaList( bool & _result, EEventName _event, PyObject * _pyevent, const char * _format, va_list _va ) const
	{
        ServiceProviderInterface * serviceProvider = this->getServiceProvider();

		bool successful = s_askPyEventVaList( serviceProvider, _result, _event, _pyevent, _format, _va );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::askPyEventVaList( size_t & _result, EEventName _event, PyObject * _pyevent, const char * _format, va_list _va ) const
	{
        ServiceProviderInterface * serviceProvider = this->getServiceProvider();

		bool successful = s_askPyEventVaList( serviceProvider, _result, _event, _pyevent, _format, _va );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::askPyEventVaList( Scriptable *& _result, EEventName _event, PyObject * _pyevent, const char * _format, va_list _va ) const
	{
        ServiceProviderInterface * serviceProvider = this->getServiceProvider();

		bool successful = s_askPyEventVaList( serviceProvider, _result, _event, _pyevent, _format, _va );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::askPyEventVaList( PyObject *& _result, EEventName _event, PyObject * _pyevent, const char * _format, va_list _va ) const
	{
        ServiceProviderInterface * serviceProvider = this->getServiceProvider();

		bool successful = s_askPyEventVaList( serviceProvider, _result, _event, _pyevent, _format, _va );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	void EventableCallOperator::operator () ( const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

		m_self->callPyEventVaList( m_event, m_pyevent, _format, valist );

		va_end( valist );
	}
	//////////////////////////////////////////////////////////////////////////
	bool EventableAskOperator::operator () ( bool & _value, bool _default, const char * _format, ... ) const
	{
		va_list valist;
		va_start(valist, _format);

		bool successful = m_self->askPyEventVaList( _value, m_event, m_pyevent, _format, valist );

		va_end( valist );

		if( successful == false )
		{
			_value = _default;
		}

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool EventableAskOperator::operator () ( size_t & _value, size_t _default, const char * _format, ... ) const
	{
		va_list valist;
		va_start(valist, _format);

		bool successful = m_self->askPyEventVaList( _value, m_event, m_pyevent, _format, valist );

		va_end( valist );

		if( successful == false )
		{
			_value = _default;
		}

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool EventableAskOperator::operator () ( Scriptable *& _value, Scriptable * _default, const char * _format, ... ) const
	{
		va_list valist;
		va_start(valist, _format);

		bool successful = m_self->askPyEventVaList( _value, m_event, m_pyevent, _format, valist );

		va_end( valist );

		if( successful == false )
		{
			_value = _default;
		}

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool EventableAskOperator::operator () ( PyObject *& _value, PyObject * _default, const char * _format, ... ) const
	{
		va_list valist;
		va_start(valist, _format);

		bool successful = m_self->askPyEventVaList( _value, m_event, m_pyevent, _format, valist );

		va_end( valist );

		if( successful == false )
		{
			_value = _default;
		}

		return successful;
	}
}
