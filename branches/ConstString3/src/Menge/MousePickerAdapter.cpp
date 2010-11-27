#	include "MousePickerAdapter.h"

#	include "MousePickerSystem.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MousePickerAdapter::MousePickerAdapter( bool _defaultHandle )
		: m_pickerId(0)
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
		this->registerEvent( EVENT_MOUSE_BUTTON_END, ("onHandleMouseButtonEventEnd"), _listener );
		this->registerEvent( EVENT_MOUSE_MOVE, ("onHandleMouseMove"), _listener );

		m_onLeaveEvent = this->registerEvent( EVENT_LEAVE, ("onLeave"), _listener );
		m_onEnterEvent = this->registerEvent( EVENT_ENTER, ("onEnter"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::activatePicker()
	{
		m_pickerId = 
			MousePickerSystem::get()->regTrap( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::deactivatePicker()
	{
		MousePickerSystem::get()->unregTrap( m_pickerId );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::updatePicker()
	{
		if( m_pickerId != 0 )
		{
			MousePickerSystem::get()
				->updateTrap( m_pickerId );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::onLeave()
	{
		if( m_onLeaveEvent )
		{
			this->callEvent( EVENT_LEAVE, "(O)", this->getEmbed() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::onEnter()
	{
		bool handle = false;

		if( m_onEnterEvent )
		{
			if( !handle )
			{
				if( this->askEvent( handle, EVENT_ENTER, "(O)", this->getEmbed() ) == false )
				{
					handle = m_defaultHandle;
				}
			}
		}
		else
		{
			handle = m_defaultHandle;
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			if( this->askEvent( handle, EVENT_KEY, "(OIIb)", this->getEmbed(), _key, _char, _isDown ) == false )
			{
				handle = m_defaultHandle;
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			if( this->askEvent( handle, EVENT_MOUSE_BUTTON, "(OIb)", this->getEmbed(), _button, _isDown ) == false )
			{
				handle = m_defaultHandle;
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleMouseButtonEventEnd( unsigned int _button, bool _isDown )
	{
		this->callEvent( EVENT_MOUSE_BUTTON_END, "(OIb)", this->getEmbed(), _button, _isDown );

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleMouseMove( float _x, float _y, int _whell )
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
