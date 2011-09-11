#	include "GlobalHandleAdapter.h"

#	include "GlobalHandleSystem.h"

#	include "pybind/system.hpp"

namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	GlobalHandleAdapter::GlobalHandleAdapter()
		: m_globalMouseEvent(false)
		, m_globalKeyEvent(false)
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
			applyGlobalKeyEvent_( true );
		}

		if( m_globalMouseEvent == true )
		{
			applyGlobalMouseEvent_( true );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::deactivateGlobalHandle()
	{
		if( m_globalKeyEvent == true )
		{
			applyGlobalKeyEvent_( false );
		}

		if( m_globalMouseEvent == true )
		{
			applyGlobalMouseEvent_( false );
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

		if( this->isEnableGlobalHandle() )
		{
			this->applyGlobalKeyEvent_( m_globalKeyEvent );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::applyGlobalMouseEvent_( bool _value )
	{
		if( _value )
		{
			GlobalHandleSystem::get()->regGlobalMouseEventable( this );
		}
		else
		{
			GlobalHandleSystem::get()->unregGlobalMouseEventable( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::applyGlobalKeyEvent_( bool _value )
	{
		if( _value )
		{
			GlobalHandleSystem::get()->regGlobalKeyEventable( this );
		}
		else
		{
			GlobalHandleSystem::get()->unregGlobalKeyEventable( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleAdapter::handleGlobalMouseButtonEvent( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			this->askEvent( handle, EVENT_GLOBAL_MOUSE_BUTTON, "(OIO)", this->getEmbed(), _button, pybind::ret_bool(_isDown) );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleAdapter::handleGlobalMouseButtonEventBegin( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			this->askEvent( handle, EVENT_GLOBAL_MOUSE_BUTTON_BEGIN, "(OIO)", this->getEmbed(), _button, pybind::get_bool(_isDown) );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleAdapter::handleGlobalMouseButtonEventEnd( const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			this->askEvent( handle, EVENT_GLOBAL_MOUSE_BUTTON_END, "(OIO)", this->getEmbed(), _button, pybind::get_bool(_isDown) );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleAdapter::handleGlobalMouseMove( const mt::vec2f & _point, float _x, float _y, int _whell )
	{
		bool handle = false;

		if( !handle )
		{
			this->askEvent( handle, EVENT_GLOBAL_MOUSE_MOVE, "(Offi)", this->getEmbed(), _x, _y, _whell );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleAdapter::handleGlobalKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			this->askEvent( handle, EVENT_GLOBAL_KEY, "(OIIO)", this->getEmbed(), _key, _char, pybind::get_bool(_isDown) );
		}

		return handle;
	}
}
