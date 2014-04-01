#	include "Kernel/GlobalHandleAdapter.h"

#   include "Interface/PlayerInterface.h"

#	include "pybind/system.hpp"

namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	GlobalHandleAdapter::GlobalHandleAdapter()
		: m_serviceProvider(nullptr)
        , m_globalMouseEvent(false)
		, m_globalKeyEvent(false)
        , m_globalMouseId(0)
        , m_globalKeyId(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::setEventListener( PyObject * _listener )
	{
        Eventable * eventable = this->getGlobalHandleEventable();

		eventable->registerEvent( EVENT_GLOBAL_MOUSE_BUTTON, ("onGlobalHandleMouseButtonEvent"), _listener );
		eventable->registerEvent( EVENT_GLOBAL_MOUSE_BUTTON2, ("onGlobalHandleMouseButtonEvent2"), _listener );
		eventable->registerEvent( EVENT_GLOBAL_MOUSE_BUTTON_BEGIN, ("onGlobalHandleMouseButtonEventBegin"), _listener );
		eventable->registerEvent( EVENT_GLOBAL_MOUSE_BUTTON_END, ("onGlobalHandleMouseButtonEventEnd"), _listener );
		eventable->registerEvent( EVENT_GLOBAL_MOUSE_MOVE, ("onGlobalHandleMouseMove"), _listener );
		eventable->registerEvent( EVENT_GLOBAL_MOUSE_WHEEL, ("onGlobalHandleMouseWheel"), _listener );
		eventable->registerEvent( EVENT_GLOBAL_KEY, ("onGlobalHandleKeyEvent"), _listener );
		eventable->registerEvent( EVENT_GLOBAL_KEY2, ("onGlobalHandleKeyEvent2"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleAdapter::activateGlobalHandle()
	{
		if( m_globalKeyEvent == true )
		{
			if( this->applyGlobalKeyEvent_( true ) == false )
			{
				return false;
			}
		}

		if( m_globalMouseEvent == true )
		{
			if( this->applyGlobalMouseEvent_( true ) == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleAdapter::deactivateGlobalHandle()
	{
		if( m_globalKeyEvent == true )
		{
			if( this->applyGlobalKeyEvent_( false ) == false )
			{
				return false;
			}
		}

		if( m_globalMouseEvent == true )
		{
			if( this->applyGlobalMouseEvent_( false ) == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleAdapter::enableGlobalMouseEvent( bool _value )
	{
		if( m_globalMouseEvent == _value )
		{
			return true;
		}

		m_globalMouseEvent = _value;

		bool enable = this->isEnableGlobalHandle();

		if( enable == true )
		{
			if( this->applyGlobalMouseEvent_( m_globalMouseEvent ) == false )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleAdapter::enableGlobalKeyEvent( bool _value )
	{
		if( m_globalKeyEvent == _value )
		{
			return true;
		}

		m_globalKeyEvent = _value;

		bool enable = this->isEnableGlobalHandle();

		if( enable == true )
		{
			if( this->applyGlobalKeyEvent_( m_globalKeyEvent ) == false )
			{
				return false;
			}
		}

		return true;			 
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleAdapter::applyGlobalMouseEvent_( bool _value )
	{
        GlobalHandleSystemInterface * globalHandleSystem = 
            this->getGlobalHandleSystem();

		if( _value == true )
		{
			m_globalMouseId = globalHandleSystem->addGlobalMouseEventable( this );

			if( m_globalMouseId == 0 )
			{
				return false;
			}
		}
		else
		{
			GlobalMouseHandler * handler = globalHandleSystem->removeGlobalMouseEventable( m_globalMouseId );
			m_globalMouseId = 0;
            
			if( handler == nullptr )
			{
				return false;
			}            
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleAdapter::applyGlobalKeyEvent_( bool _value )
	{
        GlobalHandleSystemInterface * globalHandleSystem = 
            this->getGlobalHandleSystem();

		if( _value == true )
		{
			m_globalKeyId = globalHandleSystem->addGlobalKeyEventable( this );

			if( m_globalKeyId == 0 )
			{
				return false;
			}
		}
		else
		{
			GlobalKeyHandler * handler = globalHandleSystem->removeGlobalKeyEventable( m_globalKeyId );
            m_globalKeyId = 0;

			if( handler == nullptr )
			{
				return false;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::handleGlobalMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
        (void)_point; //TODO

        Eventable * eventable = this->getGlobalHandleEventable();
        Scriptable * scriptable = this->getGlobalHandleScriptable();
		
        EVENTABLE_CALL(m_serviceProvider, eventable, EVENT_GLOBAL_MOUSE_BUTTON)( "(OIIO)", scriptable->getEmbed(), _touchId, _button, pybind::get_bool(_isDown) );

		if( m_globalMouseEvent == false )
		{
			return;
		}

		EVENTABLE_CALL(m_serviceProvider, eventable, EVENT_GLOBAL_MOUSE_BUTTON2)( "(OIffIO)", scriptable->getEmbed(), _touchId, _point.x, _point.y, _button, pybind::get_bool(_isDown) );
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::handleGlobalMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
        (void)_point; //TODO

        Eventable * eventable = this->getGlobalHandleEventable();
        Scriptable * scriptable = this->getGlobalHandleScriptable();

		EVENTABLE_CALL(m_serviceProvider, eventable, EVENT_GLOBAL_MOUSE_BUTTON_BEGIN)( "(OIIO)", scriptable->getEmbed(), _touchId, _button, pybind::get_bool(_isDown) );
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::handleGlobalMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
        (void)_point; //TODO

        Eventable * eventable = this->getGlobalHandleEventable();
        Scriptable * scriptable = this->getGlobalHandleScriptable();

		EVENTABLE_CALL(m_serviceProvider, eventable, EVENT_GLOBAL_MOUSE_BUTTON_END)( "(OIIO)", scriptable->getEmbed(), _touchId, _button, pybind::get_bool(_isDown) );
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::handleGlobalMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y )
	{
        (void)_point; //TODO

        Eventable * eventable = this->getGlobalHandleEventable();
        Scriptable * scriptable = this->getGlobalHandleScriptable();

		EVENTABLE_CALL(m_serviceProvider, eventable, EVENT_GLOBAL_MOUSE_MOVE)( "(OIffff)", scriptable->getEmbed(), _touchId, _point.x, _point.y, _x, _y );
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::handleGlobalMouseWheel( unsigned int _touchId, const mt::vec2f & _point, int _wheel )
	{
		Eventable * eventable = this->getGlobalHandleEventable();
		Scriptable * scriptable = this->getGlobalHandleScriptable();

		EVENTABLE_CALL(m_serviceProvider, eventable, EVENT_GLOBAL_MOUSE_WHEEL)( "(OIffi)", scriptable->getEmbed(), _touchId, _point.x, _point.y, _wheel );
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::handleGlobalKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown, bool _repeating )
	{
        (void)_point; //TODO

        Eventable * eventable = this->getGlobalHandleEventable();
        Scriptable * scriptable = this->getGlobalHandleScriptable();

        EVENTABLE_CALL(m_serviceProvider, eventable, EVENT_GLOBAL_KEY)( "(OIIO)", scriptable->getEmbed(), _key, _char, pybind::get_bool(_isDown) );

		if( m_globalKeyEvent == false )
		{
			return;
		}

		EVENTABLE_CALL(m_serviceProvider, eventable, EVENT_GLOBAL_KEY2)( "(OIIOO)", scriptable->getEmbed(), _key, _char, pybind::get_bool(_isDown), pybind::get_bool(_repeating) );
	}
    //////////////////////////////////////////////////////////////////////////
    GlobalHandleSystemInterface * GlobalHandleAdapter::getGlobalHandleSystem() const
    {
        GlobalHandleSystemInterface * globalHandleSystem = PLAYER_SERVICE(m_serviceProvider)
            ->getGlobalHandleSystem();

        return globalHandleSystem;
    }
}
