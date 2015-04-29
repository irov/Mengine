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
	bool Eventable::registerEvent( EEventName _event, const char * _method, const pybind::dict & _dict, bool * _exist )
	{
		if( _dict.is_invalid() == true )
		{
			this->removeEvent_( _event );

			if( _exist != nullptr )
			{
				*_exist = false;
			}

			return true;
		}

		pybind::object ev = this->getEvent_( _dict, _method );

		if( ev.is_invalid() == true )
		{
			return false;
		}

		if( ev.is_none() == true )
		{			
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
	bool Eventable::registerEventMethod( EEventName _event, const char * _method, const pybind::object & _module, bool * _exist )
	{
		if( _module.is_invalid() == true )
		{
			this->removeEvent_( _event );

			if( _exist != nullptr )
			{
				*_exist = false;
			}

			return true;
		}

		const pybind::object & ev = this->getEventMethod_( _module, _method );

		if( ev.is_invalid() == true )
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
	void Eventable::insertEvent_( EEventName _event, const pybind::object & _cb )
	{ 
		TMapEvent::iterator it_find = m_mapEvent.find( _event );

		if( it_find == m_mapEvent.end() )
		{
			m_mapEvent.insert( std::make_pair( _event, _cb ) );
		}
		else
		{
			it_find->second = _cb;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::removeEvent_( EEventName _event )
	{
		TMapEvent::iterator it_find = m_mapEvent.find(_event);

		if( it_find != m_mapEvent.end() )
		{
			m_mapEvent.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object Eventable::getEvent_( const pybind::dict & _dict, const char * _method ) const
	{		
		if( _dict.contains( _method ) == false )
		{
			return pybind::make_invalid_object_t();
		}

		pybind::object py_event = _dict[_method];

#	ifdef _DEBUG
		_dict.remove( _method );
#	endif

		return py_event;
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object Eventable::getEventMethod_( const pybind::object & _module, const char * _method ) const
	{
		if( _module.has_attr( _method ) == false )
		{
			return pybind::make_invalid_object_t();
		}

		pybind::object py_event = _module.get_attr( _method );

		return py_event;
	};
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::hasEvent( EEventName _event ) const
	{
		TMapEvent::const_iterator it_find = m_mapEvent.find( _event );

		if( it_find == m_mapEvent.end() )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object Eventable::getEvent( EEventName _event ) const
	{
		TMapEvent::const_iterator it_find = m_mapEvent.find( _event );

		if( it_find == m_mapEvent.end() )
		{
			return pybind::make_invalid_object_t();
		}

		const pybind::object & cb = it_find->second;

		return cb;
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::unregisterEvents_()
	{
		m_mapEvent.clear();
	}

    //////////////////////////////////////////////////////////////////////////
}
