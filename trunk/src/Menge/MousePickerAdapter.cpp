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
	void MousePickerAdapter::regEventListener( PyObject * _listener )
	{
		this->registerEvent( EVENT_KEY, ("onHandleKeyEvent"), _listener );
		this->registerEvent( EVENT_MOUSE_BUTTON, ("onHandleMouseButtonEvent"), _listener );
		this->registerEvent( EVENT_MOUSE_MOVE, ("onHandleMouseMove"), _listener );

		m_onLeaveEvent = this->registerEvent( EVENT_LEAVE, ("onLeave"), _listener );
		m_onEnterEvent = this->registerEvent( EVENT_ENTER, ("onEnter"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::regEventSelf()
	{
		this->registerEvent( EVENT_KEY, ("onHandleKeyEvent"), this );
		this->registerEvent( EVENT_MOUSE_BUTTON, ("onHandleMouseButtonEvent"), this );
		this->registerEvent( EVENT_MOUSE_MOVE, ("onHandleMouseMove"), this );

		m_onLeaveEvent = this->registerEvent( EVENT_LEAVE, ("onLeave"), this );
		m_onEnterEvent = this->registerEvent( EVENT_ENTER, ("onEnter"), this );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::activatePicker()
	{
		m_pickerId = 
			MousePickerSystem::hostage()->regTrap( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::deactivatePicker()
	{
		MousePickerSystem::hostage()->unregTrap( m_pickerId );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::updatePicker()
	{
		if( m_pickerId != 0 )
		{
			MousePickerSystem::hostage()->updateTrap( m_pickerId );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	InputHandler * MousePickerAdapter::getInputHandler()
	{
		return this;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::onLeave()
	{
		if( m_onLeaveEvent )
		{
			this->callEvent( EVENT_LEAVE, "(O)", this->getEmbedding() );
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
				if( this->askEvent( handle, EVENT_ENTER, "(O)", this->getEmbedding() ) == false )
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
			if( this->askEvent( handle, EVENT_KEY, "(OIIb)", this->getEmbedding(), _key, _char, _isDown ) == false )
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
			if( this->askEvent( handle, EVENT_MOUSE_BUTTON, "(OIb)", this->getEmbedding(), _button, _isDown ) == false )
			{
				handle = m_defaultHandle;
			}
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleMouseButtonEventEnd( unsigned int _button, bool _isDown )
	{
		this->callEvent( EVENT_MOUSE_BUTTON_END, "(OIb)", this->getEmbedding(), _button, _isDown );

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleMouseMove( float _x, float _y, int _whell )
	{
		bool handle = false;

		if( !handle )
		{
			if( this->askEvent( handle, EVENT_MOUSE_MOVE, "(Offi)", this->getEmbedding(), _x, _y, _whell ) == false )
			{
				handle = m_defaultHandle;
			}
		}

		return handle;
	}

}