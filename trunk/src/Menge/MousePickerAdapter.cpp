#	include "MousePickerAdapter.h"

#   include "Interface/PlayerInterface.h"

#	include "MousePickerSystem.h"

#   include "Kernel/Eventable.h"

#	include "pybind/system.hpp"
#	include "pybind/extract.hpp"

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
		eventable->registerEvent( EVENT_KEY2, ("onHandleKeyEvent2"), _listener );
		eventable->registerEvent( EVENT_MOUSE_BUTTON, ("onHandleMouseButtonEvent"), _listener );
		eventable->registerEvent( EVENT_MOUSE_BUTTON2, ("onHandleMouseButtonEvent2"), _listener );
		eventable->registerEvent( EVENT_MOUSE_BUTTON_BEGIN, ("onHandleMouseButtonEventBegin"), _listener );
		eventable->registerEvent( EVENT_MOUSE_BUTTON_END, ("onHandleMouseButtonEventEnd"), _listener );
		eventable->registerEvent( EVENT_MOUSE_MOVE, ("onHandleMouseMove"), _listener );
		eventable->registerEvent( EVENT_MOUSE_WHELL, ("onHandleMouseWhell"), _listener );
		

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
	PickerTrapState * MousePickerAdapter::getPicker() const
	{
		return m_picker;
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

		mousePickerSystem->updateTrap();
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::deactivatePicker()
	{
        MousePickerSystemInterface * mousePickerSystem = this->getMousePickerSystem();
			
        mousePickerSystem->unregTrap( m_picker );
		m_picker = nullptr;

		mousePickerSystem->updateTrap();
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::isActivePicker() const
	{
		return m_picker != nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void MousePickerAdapter::onMouseLeave()
	{
		if( m_onLeaveEvent == true )
		{
            Eventable * eventable = this->getPickerEventable();

			EVENTABLE_CALL(m_serviceProvider, eventable, EVENT_MOUSE_LEAVE)( "(O)", this->getPickerEmbed() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::onMouseEnter( const mt::vec2f & _point )
	{
        (void)_point;

		bool handle = m_defaultHandle;

        if( m_onEnterEvent == true )                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
        {
            Eventable * eventable = this->getPickerEventable();

            EVENTABLE_ASK(m_serviceProvider, eventable, EVENT_MOUSE_ENTER)( handle, m_defaultHandle, "(O)", this->getPickerEmbed() );
        }

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown, bool _repeating )
	{
        (void)_point; //TODO

		bool handle = false;

		if( handle == false )
		{
            Eventable * eventable = this->getPickerEventable();

			EVENTABLE_ASK(m_serviceProvider, eventable, EVENT_KEY)( handle, m_defaultHandle, "(OIIO)", this->getPickerEmbed(), _key, _char, pybind::get_bool(_isDown) );

			if( m_picker == nullptr )
			{
				return handle;
			}

			EVENTABLE_ASK(m_serviceProvider, eventable, EVENT_KEY2)( handle, m_defaultHandle, "(OIIOO)", this->getPickerEmbed(), _key, _char, pybind::get_bool(_isDown), pybind::get_bool(_repeating) );
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
            Eventable * eventable = this->getPickerEventable();

			EVENTABLE_ASK(m_serviceProvider, eventable, EVENT_MOUSE_BUTTON)( handle, m_defaultHandle, "(OIIO)"
				, this->getPickerEmbed()
				, _touchId
				, _button
				, pybind::get_bool(_isDown) 
				);

			if( m_picker == nullptr )
			{
				return handle;
			}

			EVENTABLE_ASK(m_serviceProvider, eventable, EVENT_MOUSE_BUTTON2)( handle, m_defaultHandle, "(OIffIO)"
				, this->getPickerEmbed()
				, _touchId
				, _point.x
				, _point.y
				, _button
				, pybind::get_bool(_isDown) 
				);
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////	
	bool MousePickerAdapter::handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
        (void)_point; //TODO

        Eventable * eventable = this->getPickerEventable();

		EVENTABLE_CALL(m_serviceProvider, eventable, EVENT_MOUSE_BUTTON_BEGIN)( "(OIIO)", this->getPickerEmbed(), _touchId, _button, pybind::get_bool(_isDown) );

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
        (void)_point; //TODO

        Eventable * eventable = this->getPickerEventable();

		EVENTABLE_CALL(m_serviceProvider, eventable, EVENT_MOUSE_BUTTON_END)( "(OIIO)", this->getPickerEmbed(), _touchId, _button, pybind::get_bool(_isDown) );

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y )
	{
        (void)_point; //TODO

		bool handle = false;

		if( handle == false )
		{
            Eventable * eventable = this->getPickerEventable();

			EVENTABLE_ASK(m_serviceProvider, eventable, EVENT_MOUSE_MOVE)( handle, m_defaultHandle, "(OIffff)", this->getPickerEmbed(), _touchId, _point.x, _point.y, _x, _y );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MousePickerAdapter::handleMouseWhell( unsigned int _touchId, const mt::vec2f & _point, int _whell )
	{
		(void)_point; //TODO

		bool handle = false;

		if( handle == false )
		{
			Eventable * eventable = this->getPickerEventable();

			EVENTABLE_ASK(m_serviceProvider, eventable, EVENT_MOUSE_WHELL)( handle, m_defaultHandle, "(OIffi)", this->getPickerEmbed(), _touchId, _point.x, _point.y, _whell );
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
