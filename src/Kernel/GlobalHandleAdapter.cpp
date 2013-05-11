#	include "Kernel/GlobalHandleAdapter.h"

#   include "Interface/PlayerInterface.h"

#	include "pybind/system.hpp"

namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	GlobalHandleAdapter::GlobalHandleAdapter()
		: m_globalMouseEvent(false)
		, m_globalKeyEvent(false)
        , m_globalMouseId(0)
        , m_globalKeyId(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::setEventListener( PyObject * _listener )
	{
		this->registerEvent( EVENT_GLOBAL_MOUSE_BUTTON, ("onGlobalHandleMouseButtonEvent"), _listener );
		this->registerEvent( EVENT_GLOBAL_MOUSE_BUTTON_BEGIN, ("onGlobalHandleMouseButtonEventBegin"), _listener );
		this->registerEvent( EVENT_GLOBAL_MOUSE_BUTTON_END, ("onGlobalHandleMouseButtonEventEnd"), _listener );
		this->registerEvent( EVENT_GLOBAL_MOUSE_MOVE, ("onGlobalHandleMouseMove"), _listener );
		this->registerEvent( EVENT_GLOBAL_KEY, ("onGlobalHandleKeyEvent"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::activateGlobalHandle()
	{
		if( m_globalKeyEvent == true )
		{
			this->applyGlobalKeyEvent_( true );
		}

		if( m_globalMouseEvent == true )
		{
			this->applyGlobalMouseEvent_( true );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::deactivateGlobalHandle()
	{
		if( m_globalKeyEvent == true )
		{
			this->applyGlobalKeyEvent_( false );
		}

		if( m_globalMouseEvent == true )
		{
			this->applyGlobalMouseEvent_( false );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::enableGlobalMouseEvent( bool _value )
	{
		if( m_globalMouseEvent == _value )
		{
			return;
		}

		m_globalMouseEvent = _value; 

		if( this->isEnableGlobalHandle() )
		{
			this->applyGlobalMouseEvent_( m_globalMouseEvent );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::enableGlobalKeyEvent( bool _value )
	{
		if( m_globalKeyEvent == _value )
		{
			return;
		}

		m_globalKeyEvent = _value;

		if( this->isEnableGlobalHandle() == true )
		{
			this->applyGlobalKeyEvent_( m_globalKeyEvent );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::applyGlobalMouseEvent_( bool _value )
	{
        GlobalHandleSystemInterface * globalHandleSystem = 
            this->getGlobalHandleSystem();

		if( _value == true )
		{
			m_globalMouseId = globalHandleSystem->addGlobalMouseEventable( this );
		}
		else
		{
			GlobalMouseHandler * handler = globalHandleSystem->removeGlobalMouseEventable( m_globalMouseId );
            (void)handler;

            m_globalMouseId = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::applyGlobalKeyEvent_( bool _value )
	{
        GlobalHandleSystemInterface * globalHandleSystem = 
            this->getGlobalHandleSystem();

		if( _value == true )
		{
			m_globalKeyId = globalHandleSystem->addGlobalKeyEventable( this );
		}
		else
		{
			GlobalKeyHandler * handler = globalHandleSystem->removeGlobalKeyEventable( m_globalKeyId );
            (void)handler;

            m_globalKeyId = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::handleGlobalMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
        (void)_point; //TODO
		
        EVENTABLE_CALL(this, EVENT_GLOBAL_MOUSE_BUTTON)( "(OIIO)", this->getEmbed(), _touchId, _button, pybind::ret_bool(_isDown) );
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::handleGlobalMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
        (void)_point; //TODO

		EVENTABLE_CALL(this, EVENT_GLOBAL_MOUSE_BUTTON_BEGIN)( "(OIIO)", this->getEmbed(), _touchId, _button, pybind::get_bool(_isDown) );
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::handleGlobalMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
        (void)_point; //TODO

		EVENTABLE_CALL(this, EVENT_GLOBAL_MOUSE_BUTTON_END)( "(OIIO)", this->getEmbed(), _touchId, _button, pybind::get_bool(_isDown) );
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::handleGlobalMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell )
	{
        (void)_point; //TODO

		EVENTABLE_CALL(this, EVENT_GLOBAL_MOUSE_MOVE)( "(OIffi)", this->getEmbed(), _touchId, _x, _y, _whell );
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::handleGlobalKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
        (void)_point; //TODO

        EVENTABLE_CALL(this, EVENT_GLOBAL_KEY)( "(OIIO)", this->getEmbed(), _key, _char, pybind::get_bool(_isDown) );
	}
    //////////////////////////////////////////////////////////////////////////
    GlobalHandleSystemInterface * GlobalHandleAdapter::getGlobalHandleSystem() const
    {
        ServiceProviderInterface * serviceProvider = this->getServiceProvider();
        GlobalHandleSystemInterface * globalHandleSystem = PLAYER_SERVICE(serviceProvider)
            ->getGlobalHandleSystem();

        return globalHandleSystem;
    }
}
