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
	enum HotSpotEventFlag
	{
		EVENT_KEY = 0,
		EVENT_MOUSE_BUTTON,
		EVENT_MOUSE_BUTTON_BEGIN,
		EVENT_MOUSE_BUTTON_END,
		EVENT_MOUSE_WHEEL,
		EVENT_MOUSE_MOVE,
		EVENT_MOUSE_ENTER,
		EVENT_MOUSE_LEAVE,
		EVENT_MOUSE_OVER_DESTROY,
		EVENT_ACTIVATE,
		EVENT_DEACTIVATE
	};
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::_setEventListener( const pybind::dict & _listener )
	{
		this->registerEvent( EVENT_KEY, ("onHandleKeyEvent"), _listener );
		this->registerEvent( EVENT_MOUSE_BUTTON, ("onHandleMouseButtonEvent"), _listener );
		this->registerEvent( EVENT_MOUSE_BUTTON_BEGIN, ("onHandleMouseButtonEventBegin"), _listener );
		this->registerEvent( EVENT_MOUSE_BUTTON_END, ("onHandleMouseButtonEventEnd"), _listener );
		this->registerEvent( EVENT_MOUSE_WHEEL, ("onHandleMouseWheel"), _listener );

		this->registerEvent( EVENT_MOUSE_MOVE, ("onHandleMouseMove"), _listener );
		this->registerEvent( EVENT_MOUSE_ENTER, ("onHandleMouseEnter"), _listener );
		this->registerEvent( EVENT_MOUSE_LEAVE, ("onHandleMouseLeave"), _listener );		
		this->registerEvent( EVENT_MOUSE_OVER_DESTROY, ("onHandleMouseOverDestroy"), _listener );

		this->registerEvent( EVENT_ACTIVATE, ("onActivate"), _listener );
		this->registerEvent( EVENT_DEACTIVATE, ("onDeactivate"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	PickerTrapState * HotSpot::propagatePickerTrapState() const
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

		mousePickerSystem->updateTraps();

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

		PickerTrapState * picker = m_picker;
		m_picker = nullptr;

		mousePickerSystem->unregTrap( picker );
		
		mousePickerSystem->updateTraps();

		m_debugColor = 0x00000000;

		EVENTABLE_CALL( m_serviceProvider, this, EVENT_DEACTIVATE )();
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::onHandleMouseLeave()
	{
		m_debugColor = 0xFFFFFFFF;

		EVENTABLE_CALL( m_serviceProvider, this, EVENT_MOUSE_LEAVE )(this);
	}
	//////////////////////////////////////////////////////////////////////////
	void HotSpot::onHandleMouseOverDestroy()
	{
		m_debugColor = 0xFFFFFFFF;

		EVENTABLE_CALL( m_serviceProvider, this, EVENT_MOUSE_OVER_DESTROY )(this);
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::onHandleMouseEnter( float _x, float _y )
	{
		m_debugColor = 0xFFFF0000;

		bool handle = m_defaultHandle;

		EVENTABLE_ASK( m_serviceProvider, this, EVENT_MOUSE_ENTER, handle )(this, _x, _y);

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleKeyEvent( const InputKeyEvent & _event )
	{
		bool handle = m_defaultHandle;
				
		EVENTABLE_ASK( m_serviceProvider, this, EVENT_KEY, handle )(this, (uint32_t)_event.key, _event.x, _event.y, _event.code, _event.isDown, _event.isRepeat);

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseButtonEvent( const InputMouseButtonEvent & _event )
	{
		bool handle = m_defaultHandle;

		EVENTABLE_ASK( m_serviceProvider, this, EVENT_MOUSE_BUTTON, handle )(this, _event.touchId, _event.x, _event.y, _event.button, _event.isDown, _event.isPressed);

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////	
	bool HotSpot::handleMouseButtonEventBegin( const InputMouseButtonEvent & _event )
	{
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_MOUSE_BUTTON_BEGIN )(this, _event.touchId, _event.x, _event.y, _event.button, _event.isDown, _event.isPressed);

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseButtonEventEnd( const InputMouseButtonEvent & _event )
	{
		EVENTABLE_CALL( m_serviceProvider, this, EVENT_MOUSE_BUTTON_END )(this, _event.touchId, _event.x, _event.y, _event.button, _event.isDown, _event.isPressed);

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseMove( const InputMouseMoveEvent & _event )
	{
		bool handle = m_defaultHandle;

		EVENTABLE_ASK( m_serviceProvider, this, EVENT_MOUSE_MOVE, handle )(this, _event.touchId, _event.x, _event.y, _event.dx, _event.dy);

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpot::handleMouseWheel( const InputMouseWheelEvent & _event )
	{
		bool handle = m_defaultHandle;
				
		EVENTABLE_ASK( m_serviceProvider, this, EVENT_MOUSE_WHEEL, handle )(this, _event.button, _event.x, _event.y, _event.wheel);

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
	void HotSpot::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		if( m_picker == nullptr )
		{
			return;
		}

		MousePickerSystemInterface * mousePickerSystem = PLAYER_SERVICE( m_serviceProvider )
			->getMousePickerSystem();

		if( mousePickerSystem == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("HotSpot::activatePicker_ '%s' invalid get mouse picker system"
				, this->getName().c_str()
				);

			return;
		}

		mousePickerSystem->invalidateTraps();
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
	bool HotSpot::pick( const mt::vec2f & _point, const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const Resolution & _contentResolution, Arrow * _arrow )
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

		HotspotMousePickerVisitor mp(this, _viewport, _camera, _contentResolution, _point, _arrow);
		bool result = mp.test( layer );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	Scriptable * HotSpot::propagatePickerScriptable()
	{
		return this;
	}
}
