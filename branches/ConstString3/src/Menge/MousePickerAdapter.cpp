#	include "MousePickerAdapter.h"

#	include "MousePickerSystem.h"

#	include "pybind/system.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MousePickerAdapter::MousePickerAdapter( bool _defaultHandle )
		: m_picker(NULL)
		, m_defaultHandle(_defaultHandle)
		, m_onEnterEvent(false)
		, m_onLeaveEvent(false)
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::setEventListener( PyObject * _listener )
	{
		this->registerEvent( EVENT_KEY, ("onHandleKeyEvent"), _listener );
		this->registerEvent( EVENT_MOUSE_BUTTON, ("onHandleMouseButtonEvent"), _listener );		
		this->registerEvent( EVENT_MOUSE_BUTTON_BEGIN, ("onHandleMouseButtonEventBegin"), _listener );
		this->registerEvent( EVENT_MOUSE_BUTTON_END, ("onHandleMouseButtonEventEnd"), _listener );
		this->registerEvent( EVENT_MOUSE_MOVE, ("onHandleMouseMove"), _listener );

		this->registerEvent( EVENT_MOUSE_LEAVE, ("onHandleMouseLeave"), _listener, &m_onLeaveEvent );
		this->registerEvent( EVENT_MOUSE_ENTER, ("onHandleMouseEnter"), _listener, &m_onEnterEvent );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::activatePicker()
	{
		m_picker = MousePickerSystem::get()
			->regTrap( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::deactivatePicker()
	{
		MousePickerSystem::get()
			->unregTrap( m_picker );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::updatePicker()
	{
		if( m_picker == NULL )
		{
			return;
		}

		MousePickerSystem::get()
			->updateTrap( m_picker );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::onMouseLeave()
	{
		if( m_onLeaveEvent )
		{
			this->callEvent( EVENT_MOUSE_LEAVE, "(O)", this->getEmbed() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::onMouseEnter()
	{
		bool handle = false;

		if( !handle )
		{
			if( m_onEnterEvent )                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
			{
				if( this->askEvent( handle, EVENT_MOUSE_ENTER, "(O)", this->getEmbed() ) == false )
				{
					handle = m_defaultHandle;
				}
			}
			else
			{
				handle = m_defaultHandle;
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			if( this->askEvent( handle, EVENT_KEY, "(OIIO)", this->getEmbed(), _key, _char, pybind::get_bool(_isDown) ) == false )
			{
				handle = m_defaultHandle;
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleMouseButtonEvent( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			if( this->askEvent( handle, EVENT_MOUSE_BUTTON, "(OIO)", this->getEmbed(), _button, pybind::get_bool(_isDown) ) == false )
			{
				handle = m_defaultHandle;
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////	
	bool MousePickerAdapter::handleMouseButtonEventBegin( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		this->callEvent( EVENT_MOUSE_BUTTON_BEGIN, "(OIO)", this->getEmbed(), _button, pybind::get_bool(_isDown) );

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleMouseButtonEventEnd( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		this->callEvent( EVENT_MOUSE_BUTTON_END, "(OIO)", this->getEmbed(), _button, pybind::get_bool(_isDown) );

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleMouseMove( const mt::vec2f & _point, float _x, float _y, int _whell )
	{
		bool handle = false;

		if( !handle )
		{
			if( this->askEvent( handle, EVENT_MOUSE_MOVE, "(Offi)", this->getEmbed(), _x, _y, _whell ) == false )
			{
				handle = m_defaultHandle;
			}
		}

		return handle;
	}
}
