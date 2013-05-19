#	include "MousePickerAdapter.h"

#   include "Interface/PlayerInterface.h"

#	include "MousePickerSystem.h"

#   include "Kernel/Eventable.h"

#	include "pybind/system.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MousePickerAdapter::MousePickerAdapter()
		: m_serviceProvider(nullptr)        
		, m_defaultHandle(false)
        , m_picker(nullptr)
		, m_onEnterEvent(false)
		, m_onLeaveEvent(false)
	{		
	}
    //////////////////////////////////////////////////////////////////////////
    void MousePickerAdapter::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::setEventListener( PyObject * _listener )
	{
        Eventable * eventable = this->getPickerEventable();

		eventable->registerEvent( EVENT_KEY, ("onHandleKeyEvent"), _listener );
		eventable->registerEvent( EVENT_MOUSE_BUTTON, ("onHandleMouseButtonEvent"), _listener );		
		eventable->registerEvent( EVENT_MOUSE_BUTTON_BEGIN, ("onHandleMouseButtonEventBegin"), _listener );
		eventable->registerEvent( EVENT_MOUSE_BUTTON_END, ("onHandleMouseButtonEventEnd"), _listener );
		eventable->registerEvent( EVENT_MOUSE_MOVE, ("onHandleMouseMove"), _listener );

		eventable->registerEvent( EVENT_MOUSE_LEAVE, ("onHandleMouseLeave"), _listener, &m_onLeaveEvent );
		eventable->registerEvent( EVENT_MOUSE_ENTER, ("onHandleMouseEnter"), _listener, &m_onEnterEvent );
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
        
		if( mousePickerSystem == nullptr )
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
		if( m_picker == nullptr )
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
            Eventable * evantable = this->getPickerEventable();

			EVENTABLE_CALL(m_serviceProvider, evantable, EVENT_MOUSE_LEAVE)( "(O)", this->getPickerEmbed() );
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
                Eventable * evantable = this->getPickerEventable();

				EVENTABLE_ASK(m_serviceProvider, evantable, EVENT_MOUSE_ENTER)( handle, m_defaultHandle, "(O)", this->getPickerEmbed() );
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

		if( handle == false )
		{
            Eventable * evantable = this->getPickerEventable();

			EVENTABLE_ASK(m_serviceProvider, evantable, EVENT_KEY)( handle, m_defaultHandle, "(OIIO)", this->getPickerEmbed(), _key, _char, pybind::get_bool(_isDown) );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
        (void)_point; //TODO

		bool handle = false;

		if( handle == false )
		{
            Eventable * evantable = this->getPickerEventable();

			EVENTABLE_ASK(m_serviceProvider, evantable, EVENT_MOUSE_BUTTON)( handle, m_defaultHandle, "(OIIO)", this->getPickerEmbed(), _touchId, _button, pybind::get_bool(_isDown) );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////	
	bool MousePickerAdapter::handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
        (void)_point; //TODO

        Eventable * evantable = this->getPickerEventable();

		EVENTABLE_CALL(m_serviceProvider, evantable, EVENT_MOUSE_BUTTON_BEGIN)( "(OIIO)", this->getPickerEmbed(), _touchId, _button, pybind::get_bool(_isDown) );

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
        (void)_point; //TODO

        Eventable * evantable = this->getPickerEventable();

		EVENTABLE_CALL(m_serviceProvider, evantable, EVENT_MOUSE_BUTTON_END)( "(OIIO)", this->getPickerEmbed(), _touchId, _button, pybind::get_bool(_isDown) );

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell )
	{
        (void)_point; //TODO

		bool handle = false;

		if( handle == false )
		{
            Eventable * evantable = this->getPickerEventable();

			EVENTABLE_ASK(m_serviceProvider, evantable, EVENT_MOUSE_MOVE)( handle, m_defaultHandle, "(OIffi)", this->getPickerEmbed(), _touchId, _x, _y, _whell );
		}

		return handle;
	}
    //////////////////////////////////////////////////////////////////////////
    MousePickerSystemInterface * MousePickerAdapter::getMousePickerSystem() const
    {
        MousePickerSystemInterface * mousePickerSystem = PLAYER_SERVICE(m_serviceProvider)
            ->getMousePickerSystem();

        return mousePickerSystem;
    }
}
