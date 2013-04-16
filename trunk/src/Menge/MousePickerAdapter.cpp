#	include "MousePickerAdapter.h"

#   include "Interface/PlayerInterface.h"

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
	void MousePickerAdapter::setDefaultHandle( bool _handle )
	{
		m_defaultHandle = _handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::getDefaultHandle() const
	{
		return m_defaultHandle;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::activatePicker()
	{
        MousePickerSystemInterface * mousePickerSystem = this->getMousePickerSystem();
        
		if( mousePickerSystem == NULL )
		{
			return;
		}

		m_picker = mousePickerSystem->regTrap( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::deactivatePicker()
	{
        MousePickerSystemInterface * mousePickerSystem = this->getMousePickerSystem();
			
        mousePickerSystem->unregTrap( m_picker );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::updatePicker()
	{
		if( m_picker == NULL )
		{
			return;
		}
        
		MousePickerSystemInterface * mousePickerSystem = this->getMousePickerSystem();
			
        mousePickerSystem->updateTrap( m_picker );
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::onMouseLeave()
	{
		if( m_onLeaveEvent )
		{
			EVENTABLE_CALL(this, EVENT_MOUSE_LEAVE)( "(O)", this->getEmbed() );
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
				//if( this->askEvent( handle, EVENT_MOUSE_ENTER, "(O)", this->getEmbed() ) == false )
				//{
				//	handle = m_defaultHandle;
				//}

				EVENTABLE_ASK(this, EVENT_MOUSE_ENTER)( handle, m_defaultHandle, "(O)", this->getEmbed() );
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
        (void)_point; //TODO

		bool handle = false;

		if( !handle )
		{
			//if( this->askEvent( handle, EVENT_KEY, "(OIIO)", this->getEmbed(), _key, _char, pybind::get_bool(_isDown) ) == false )
			//{
			//	handle = m_defaultHandle;
			//}

			EVENTABLE_ASK(this, EVENT_KEY)( handle, m_defaultHandle, "(OIIO)", this->getEmbed(), _key, _char, pybind::get_bool(_isDown) );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
        (void)_point; //TODO

		bool handle = false;

		if( !handle )
		{
			//if( this->askEvent( handle, EVENT_MOUSE_BUTTON, "(OIIO)", this->getEmbed(), _touchId, _button, pybind::get_bool(_isDown) ) == false )
			//{
			//	handle = m_defaultHandle;
			//}

			EVENTABLE_ASK(this, EVENT_MOUSE_BUTTON)( handle, m_defaultHandle, "(OIIO)", this->getEmbed(), _touchId, _button, pybind::get_bool(_isDown) );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////	
	bool MousePickerAdapter::handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
        (void)_point; //TODO

		EVENTABLE_CALL(this, EVENT_MOUSE_BUTTON_BEGIN)( "(OIIO)", this->getEmbed(), _touchId, _button, pybind::get_bool(_isDown) );

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
        (void)_point; //TODO

		EVENTABLE_CALL(this, EVENT_MOUSE_BUTTON_END)( "(OIIO)", this->getEmbed(), _touchId, _button, pybind::get_bool(_isDown) );

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell )
	{
        (void)_point; //TODO

		bool handle = false;

		if( !handle )
		{
			//if( this->askEvent( handle, EVENT_MOUSE_MOVE, "(OIffi)", this->getEmbed(), _touchId, _x, _y, _whell ) == false )
			//{
			//	handle = m_defaultHandle;
			//}

			EVENTABLE_ASK(this, EVENT_MOUSE_MOVE)( handle, m_defaultHandle, "(OIffi)", this->getEmbed(), _touchId, _x, _y, _whell );
		}

		return handle;
	}
    //////////////////////////////////////////////////////////////////////////
    MousePickerSystemInterface * MousePickerAdapter::getMousePickerSystem() const
    {
        ServiceProviderInterface * serviceProvider = this->getServiceProvider();

        MousePickerSystemInterface * mousePickerSystem = PLAYER_SERVICE(serviceProvider)
            ->getMousePickerSystem();

        return mousePickerSystem;
    }
}
