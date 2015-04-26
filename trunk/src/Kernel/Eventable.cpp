#	include "Eventable.h"

#	include "Scriptable.h"

#   include "Interface/ScriptSystemInterface.h"

#   include "Logger/Logger.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
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

		this->insertEvent_( _event, ev );

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

		this->insertEvent_( _event, ev );

		if( _exist != nullptr )
		{
			*_exist = true;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::insertEvent_( EEventName _event, PyObject * _cb )
	{ 
		TMapEvent::iterator it_find = m_mapEvent.find( _event );

		if( it_find == m_mapEvent.end() )
		{
			m_mapEvent.insert( std::make_pair( _event, _cb ) );
		}
		else
		{
			PyObject * obj = it_find->second;

			pybind::decref( obj );

			it_find->second = _cb;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::removeEvent_( EEventName _event )
	{
		TMapEvent::iterator it_find = m_mapEvent.find(_event);

		if( it_find != m_mapEvent.end() )
		{
			PyObject * py_event = it_find->second;

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
		TMapEvent::const_iterator it_find = m_mapEvent.find( _event );

		if( it_find == m_mapEvent.end() )
		{
			return nullptr;
		}

		PyObject * cb = it_find->second;

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
			PyObject * cb = it->second;

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
    EventableAskOperator::EventableAskOperator( ServiceProviderInterface * _serviceProvider, EEventName _event, PyObject * _pyevent )
        : m_serviceProvider(_serviceProvider)
        , m_event(_event)
        , m_pyevent(_pyevent)
    {
    }
}
