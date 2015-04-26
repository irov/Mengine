#	include "HotSpot.h"

#   include "HotspotMousePickerVisitor.h"

#	include "Interface/MousePickerSystemInterface.h"

#	include "Kernel/Scene.h"
#	include "Layer2D.h"

#	include "Player.h"
#	include "Arrow.h"

#	include "pybind/system.hpp"
#	include "pybind/extract.hpp"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	HotSpot::HotSpot()
		: m_debugColor(0x00000000)		
		, m_outward(false)
		, m_global(false)
		, m_picker(nullptr)
		, m_defaultHandle(true)		
		, m_onMouseMoveEvent(false)
		, m_onMouseEnterEvent(false)
		, m_onMouseLeaveEvent(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	HotSpot::~HotSpot()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::setOutward( bool _value )
	{
		m_outward = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::getOutward() const
	{
		return m_outward;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::isMousePickerOver() const
	{
		if( m_picker == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("HotSpot::isMousePickerOver %s not activate picker"
				, this->getName().c_str()
				);

			return false;
		}
		
		bool picked = m_picker->picked;

		return picked;
	}
	//////////////////////////////////////////////////////////////////////////
	MousePickerTrapInterface * HotSpot::getPickerTrap()
	{
		return this;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::setDefaultHandle( bool _handle )
	{
		m_defaultHandle = _handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::getDefaultHandle() const
	{
		return m_defaultHandle;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::setGlobal( bool _value )
	{
		m_global = _value;		
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::getGlobal() const
	{
		return m_global;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_setEventListener( PyObject * _listener )
	{
		Node::_setEventListener( _listener );

		this->registerEvent( EVENT_KEY, ("onHandleKeyEvent"), _listener );
		this->registerEvent( EVENT_MOUSE_BUTTON, ("onHandleMouseButtonEvent"), _listener );
		this->registerEvent( EVENT_MOUSE_BUTTON_BEGIN, ("onHandleMouseButtonEventBegin"), _listener );
		this->registerEvent( EVENT_MOUSE_BUTTON_END, ("onHandleMouseButtonEventEnd"), _listener );
		this->registerEvent( EVENT_MOUSE_WHEEL, ("onHandleMouseWheel"), _listener );

		this->registerEvent( EVENT_MOUSE_MOVE, ("onHandleMouseMove"), _listener, &m_onMouseMoveEvent );
		this->registerEvent( EVENT_MOUSE_LEAVE, ("onHandleMouseLeave"), _listener, &m_onMouseLeaveEvent );
		this->registerEvent( EVENT_MOUSE_ENTER, ("onHandleMouseEnter"), _listener, &m_onMouseEnterEvent );

		this->registerEvent( EVENT_ACTIVATE, ("onActivate"), _listener );
		this->registerEvent( EVENT_DEACTIVATE, ("onDeactivate"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	PickerTrapState * HotSpot::getPicker() const
	{
		return m_picker;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::activatePicker_()
	{
		if( m_picker != nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("HotSpot::activatePicker_ '%s' alredy activate picker"
				, this->getName().c_str()
				);

			return;
		}

		MousePickerSystemInterface * mousePickerSystem = PLAYER_SERVICE(m_serviceProvider)
			->getMousePickerSystem();

		if( mousePickerSystem == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("HotSpot::activatePicker_ '%s' invalid get mouse picker system"
				, this->getName().c_str()
				);

			return;
		}

		m_debugColor = 0xFFFFFFFF;

		m_picker = mousePickerSystem->regTrap( this );

		mousePickerSystem->updateTrap();

		EVENTABLE_CALL(m_serviceProvider, this, EVENT_ACTIVATE)();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::deactivatePicker_()
	{
		if( m_picker == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("HotSpot::activatePicker_ '%s' alredy deactivate picker"
				, this->getName().c_str()
				);

			return;
		}

		MousePickerSystemInterface * mousePickerSystem = PLAYER_SERVICE(m_serviceProvider)
			->getMousePickerSystem();

		if( mousePickerSystem == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("HotSpot::activatePicker_ '%s' invalid get mouse picker system"
				, this->getName().c_str()
				);

			return;
		}

		mousePickerSystem->unregTrap( m_picker );
		m_picker = nullptr;

		mousePickerSystem->updateTrap();

		m_debugColor = 0x00000000;

		EVENTABLE_CALL( m_serviceProvider, this, EVENT_DEACTIVATE )();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::onMouseLeave()
	{
		m_debugColor = 0xFFFFFFFF;

		if( m_onMouseLeaveEvent == true )
		{
			EVENTABLE_CALL( m_serviceProvider, this, EVENT_MOUSE_LEAVE )(this);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::onMouseEnter( const mt::vec2f & _point )
	{
		m_debugColor = 0xFFFF0000;

		bool handle = m_defaultHandle;

		if( m_onMouseEnterEvent == true )                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
		{
			EVENTABLE_ASK( m_serviceProvider, this, EVENT_MOUSE_ENTER, handle )( this, _point.x, _point.y );
		}		

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown, bool _repeating )
	{
		bool handle = false;
				
		EVENTABLE_ASK( m_serviceProvider, this, EVENT_KEY, handle )(this, _key, _point.x, _point.y, _char, _isDown, _repeating);

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handle = m_defaultHandle;

		EVENTABLE_ASK( m_serviceProvider, this, EVENT_MOUSE_BUTTON, handle )(this, _touchId, _point.x, _point.y, _button, _isDown);

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////	
	bool HotSpot::handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_MOUSE_BUTTON_BEGIN )(this, _touchId, _point.x, _point.y, _button, _isDown);

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{		
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_MOUSE_BUTTON_END )(this, _touchId, _point.x, _point.y, _button, _isDown);

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y )
	{
		bool handle = m_defaultHandle;

		if( m_onMouseMoveEvent == true )
		{			
			EVENTABLE_ASK( m_serviceProvider, this, EVENT_MOUSE_MOVE, handle )(this, _touchId, _point.x, _point.y, _x, _y);
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseWheel( unsigned int _touchId, const mt::vec2f & _point, int _wheel )
	{
		bool handle = false;
				
		EVENTABLE_ASK( m_serviceProvider, this, EVENT_MOUSE_WHEEL, handle )(this, _touchId, _point.x, _point.y, _wheel);

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_localHide( bool _value )
	{
		Node::_localHide( _value );

		if( _value == true )
		{
			if( this->isAfterActive() == true && this->isFreeze() == false )
			{
				this->deactivatePicker_();
			}
		}
		else
		{
			if( this->isAfterActive() == true && this->isFreeze() == false )
			{
				this->activatePicker_();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_freeze( bool _value )
	{
		Node::_freeze( _value );

		if( _value == true )
		{
			if( this->isAfterActive() == true && this->isLocalHide() == false )
			{
				this->deactivatePicker_();
			}
		}
		else
		{
			if( this->isAfterActive() == true && this->isLocalHide() == false )
			{
				this->activatePicker_();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_afterActivate()
	{
		if( this->isLocalHide() == false && this->isFreeze() == false )
		{
			this->activatePicker_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_deactivate()
	{
		if( this->isLocalHide() == false && this->isFreeze() == false )
		{
			this->deactivatePicker_();
		}
		
		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::pick( const mt::vec2f & _point, const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, Arrow * _arrow )
	{
		if( this->isActivate() == false )
		{
			return false;
		}

		Layer * layer = this->getLayer();

		if( layer == nullptr )
		{
			return false;
		}

		HotspotMousePickerVisitor mp(this, _viewport, _camera, _point, _arrow);
		bool result = mp.test( layer );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * HotSpot::getPickerEmbed()
	{
		PyObject * embed = this->getEmbed();

		return embed;
	}
}
