#	include "Eventable.h"

#	include "Scriptable.h"

#   include "Interface/ScriptSystemInterface.h"

#   include "Logger/Logger.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Eventable::Eventable()
		: m_flag(0)
	{
    }
	//////////////////////////////////////////////////////////////////////////
	Eventable::~Eventable()
	{
		this->unregisterEvents_();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::registerEvent( uint32_t _event, const char * _method, const pybind::dict & _dict )
	{
		if( _event > 32 )
		{
			return false;
		}

		if( _dict.is_invalid() == true )
		{
			this->removeEvent_( _event );

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

			return true;
		}

		this->insertEvent_( _event, ev );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Eventable::registerEventMethod( uint32_t _event, const char * _method, const pybind::object & _module )
	{
		if( _event > 32 )
		{
			return false;
		}

		if( _module.is_invalid() == true )
		{
			this->removeEvent_( _event );

			return true;
		}

		pybind::object ev = this->getEventMethod_( _module, _method );

		if( ev.is_invalid() == true )
		{
			return false;
		}

		this->insertEvent_( _event, ev );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::insertEvent_( uint32_t _event, const pybind::object & _cb )
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

		m_flag |= (1 << _event);
	}
	//////////////////////////////////////////////////////////////////////////
	void Eventable::removeEvent_( uint32_t _event )
	{
		TMapEvent::iterator it_find = m_mapEvent.find(_event);

		if( it_find != m_mapEvent.end() )
		{
			m_mapEvent.erase( it_find );
		}

		m_flag &= ~ (1 << _event);
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
	bool Eventable::hasEvent( uint32_t _event ) const
	{
		return (m_flag & (1 << _event)) != 0;
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object Eventable::getEvent( uint32_t _event ) const
	{
		if( (m_flag & (1 << _event)) == 0 )
		{
			return pybind::make_invalid_object_t();
		}

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
