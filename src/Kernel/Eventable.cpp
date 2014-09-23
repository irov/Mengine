#	include "Eventable.h"

#	include "Scriptable.h"

#   include "Interface/ScriptSystemInterface.h"

#   include "Logger/Logger.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        static bool s_callPyEventVaList2( ServiceProviderInterface * _serviceProvider, EEventName _event, PyObject * _pyevent, const char * _format, va_list _va )
        {
			PyObject * py_result = pybind::ask_va( _pyevent, _format, _va );

            if( py_result == nullptr )
            {
                return false;
            }

            if( pybind::is_none( py_result ) == false )
            { 
                const char * envString = eventToString( _event );
                const char * objRepr = pybind::object_repr( py_result );

                LOGGER_ERROR(_serviceProvider)("Eventable: '%s' '%s' must return 'None', but return '%s'"
                    , envString
                    , pybind::object_repr( _pyevent )
                    , objRepr
                    );

				pybind::decref( py_result );

				return false;
            }

            pybind::decref( py_result );

			return true;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool s_askPyEventVaListT( ServiceProviderInterface * _serviceProvider, T & _result, EEventName _event, PyObject * _pyevent, const char * _format, va_list _va )
        {
            PyObject * py_result = pybind::ask_va( _pyevent, _format, _va );

            if( py_result == nullptr )
            {
                return false;
            }

            if( pybind::is_none( py_result ) == true )
            { 
                const char * envString = eventToString( _event );
                const char * typeName = typeid(T).name();
                const char * objRepr = pybind::object_repr( py_result );

                LOGGER_ERROR(_serviceProvider)("Eventable: '%s' '%s' must return '%s', but return '%s'"
                    , envString
                    , pybind::object_repr( _pyevent )
                    , typeName
                    , objRepr
                    );

                pybind::decref( py_result );

                return false;
            }

            _result = pybind::extract<T>( py_result );

            pybind::decref( py_result );

            return true;		
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_askPyEventVaList( ServiceProviderInterface * _serviceProvider, bool & _result, EEventName _event, PyObject * _pyevent, const char * _format, va_list _va )
        {
            pybind::incref( _pyevent );

            bool successful = s_askPyEventVaListT( _serviceProvider, _result, _event, _pyevent, _format, _va );

            pybind::decref( _pyevent );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_askPyEventVaList( ServiceProviderInterface * _serviceProvider, uint32_t & _result, EEventName _event, PyObject * _pyevent, const char * _format, va_list _va )
        {
            pybind::incref( _pyevent );

            bool successful = s_askPyEventVaListT( _serviceProvider, _result, _event, _pyevent, _format, _va );

            pybind::decref( _pyevent );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_askPyEventVaList( ServiceProviderInterface * _serviceProvider, Scriptable *& _result, EEventName _event, PyObject * _pyevent, const char * _format, va_list _va )
        {
            pybind::incref( _pyevent );

            bool successful = s_askPyEventVaListT( _serviceProvider, _result, _event, _pyevent, _format, _va );

            pybind::decref( _pyevent );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_askPyEventVaList( ServiceProviderInterface * _serviceProvider, PyObject *& _result, EEventName _event, PyObject * _pyevent, const char * _format, va_list _va )
        {
            pybind::incref( _pyevent );

            bool successful = s_askPyEventVaListT( _serviceProvider, _result, _event, _pyevent, _format, _va );

            pybind::decref( _pyevent );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_callPyEventVaList( ServiceProviderInterface * _serviceProvider, EEventName _event, PyObject * _pyevent, const char * _format, va_list _va )
        {        
            pybind::incref( _pyevent );

            bool successful = s_callPyEventVaList2( _serviceProvider, _event, _pyevent, _format, _va );

            pybind::decref( _pyevent );

			return successful;
        }
    }    
	//////////////////////////////////////////////////////////////////////////
	Eventable::Eventable()
	{
    }
	//////////////////////////////////////////////////////////////////////////
	Eventable::~Eventable()
	{
		this->unregisterEvents_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::registerEvent( EEventName _event, const char * _method, PyObject * _dict, bool * _exist )
	{
		if( _dict == nullptr )
		{
			this->removeEvent_( _event );

			if( _exist != nullptr )
			{
				*_exist = false;
			}

			return true;
		}

		PyObject * ev = this->getEvent_( _method, _dict );

		if( ev == nullptr )
		{
			return false;
		}

		if( pybind::is_none(ev) == true )
		{
			pybind::decref( ev );

			this->removeEvent_( _event );

			if( _exist != nullptr )
			{
				*_exist = false;
			}

			return true;
		}

		TMapEvent::iterator it_find = m_mapEvent.find(_event);

		if( it_find == m_mapEvent.end() )
		{
			m_mapEvent.insert( _event, ev );
		}
		else
		{
			PyObject * obj = m_mapEvent.get_value( it_find );

			pybind::decref( obj );

			m_mapEvent.set_value( it_find, ev );
		}

		if( _exist != nullptr )
		{
			*_exist = true;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::registerEventMethod( EEventName _event, const char * _method, PyObject * _module, bool * _exist )
	{
		if( _module == nullptr )
		{
			this->removeEvent_( _event );

			if( _exist != nullptr )
			{
				*_exist = false;
			}

			return true;
		}

		PyObject * ev = this->getEventMethod_( _method, _module );

		if( ev == nullptr )
		{
			return false;
		}

		TMapEvent::iterator it_find = m_mapEvent.find(_event);

		if( it_find == m_mapEvent.end() )
		{
			m_mapEvent.insert( _event, ev );
		}
		else
		{
			PyObject * obj = m_mapEvent.get_value( it_find );

			pybind::decref( obj );

			m_mapEvent.set_value( it_find, ev );
		}

		if( _exist != nullptr )
		{
			*_exist = true;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::removeEvent_( EEventName _event )
	{
		TMapEvent::iterator it_find = m_mapEvent.find(_event);

		if( it_find != m_mapEvent.end() )
		{
			PyObject * py_event  = m_mapEvent.get_value( it_find );
			pybind::decref( py_event );

			m_mapEvent.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Eventable::getEvent_( const char * _method, PyObject * _dict ) const
	{
		if( pybind::dict_check( _dict ) == false )
		{
			return nullptr;
		}
		
		if( pybind::dict_contains( _dict, _method ) == false )
		{
			return nullptr;
		}

		PyObject * py_event = pybind::dict_get( _dict, _method );
		
		pybind::incref( py_event );

		pybind::dict_remove( _dict, _method );

		return py_event;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * Eventable::getEventMethod_( const char * _method, PyObject * _module ) const
	{
		if( pybind::has_attr( _module, _method ) == false )
		{
			return nullptr;
		}

		PyObject * py_event = pybind::get_attr( _module, _method );

		return py_event;
	};
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
		PyObject * cb;
		if( m_mapEvent.has( _event, &cb ) == false )
		{
			return nullptr;
		}

		return cb;
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::unregisterEvents_()
	{
		for( TMapEvent::iterator
			it = m_mapEvent.begin(),
			it_end = m_mapEvent.end();
		it != it_end;
		++it)
		{
			PyObject * cb = m_mapEvent.get_value( it );
			pybind::decref( cb );
		}

		m_mapEvent.clear();
	}
    //////////////////////////////////////////////////////////////////////////
    EventableCallOperator::EventableCallOperator( ServiceProviderInterface * _serviceProvider, EEventName _event, PyObject * _pyevent )
        : m_serviceProvider(_serviceProvider)
        , m_event(_event)
        , m_pyevent(_pyevent)
    {
    }
	//////////////////////////////////////////////////////////////////////////
	bool EventableCallOperator::operator () ( const char * _format, ... )
	{
		va_list valist;
		va_start(valist, _format);

        bool successful = Helper::s_callPyEventVaList( m_serviceProvider, m_event, m_pyevent, _format, valist );

		va_end( valist );

		return successful;
	}
    //////////////////////////////////////////////////////////////////////////
    EventableAskOperator::EventableAskOperator( ServiceProviderInterface * _serviceProvider, EEventName _event, PyObject * _pyevent )
        : m_serviceProvider(_serviceProvider)
        , m_event(_event)
        , m_pyevent(_pyevent)
    {
    }
	//////////////////////////////////////////////////////////////////////////
	bool EventableAskOperator::operator () ( bool & _value, bool _default, const char * _format, ... ) const
	{
		va_list valist;
		va_start(valist, _format);

		bool successful = Helper::s_askPyEventVaList( m_serviceProvider, _value, m_event, m_pyevent, _format, valist );

		va_end( valist );

		if( successful == false )
		{
			_value = _default;
		}

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool EventableAskOperator::operator () ( uint32_t & _value, uint32_t _default, const char * _format, ... ) const
	{
		va_list valist;
		va_start(valist, _format);

		bool successful = Helper::s_askPyEventVaList( m_serviceProvider, _value, m_event, m_pyevent, _format, valist );

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

		bool successful = Helper::s_askPyEventVaList( m_serviceProvider, _value, m_event, m_pyevent, _format, valist );

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

        bool successful = Helper::s_askPyEventVaList( m_serviceProvider, _value, m_event, m_pyevent, _format, valist );

		va_end( valist );

		if( successful == false )
		{
			_value = _default;
		}

		return successful;
	}
}
