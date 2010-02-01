#	include "GlobalHandleAdapter.h"

#	include "GlobalHandleSystem.h"

namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	GlobalHandleAdapter::GlobalHandleAdapter()
		: m_globalMouseEvent(false)
		, m_globalKeyEvent(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::regEvent( PyObject * _listener )
	{
		this->registerEvent( EVENT_GLOBAL_MOUSE_BUTTON, ("onGlobalHandleMouseButtonEvent"), _listener );
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
			GlobalHandleSystem::hostage()->regGlobalMouseEventable( this );
		}
		else
		{
			GlobalHandleSystem::hostage()->unregGlobalMouseEventable( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void GlobalHandleAdapter::applyGlobalKeyEvent_( bool _value )
	{
		if( _value )
		{
			GlobalHandleSystem::hostage()->regGlobalKeyEventable( this );
		}
		else
		{
			GlobalHandleSystem::hostage()->unregGlobalKeyEventable( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleAdapter::handleGlobalMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			this->askEvent( handle, EVENT_GLOBAL_MOUSE_BUTTON, "(OIb)", this->getEmbedding(), _button, _isDown );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleAdapter::handleGlobalMouseMove( float _x, float _y, int _whell )
	{
		bool handle = false;

		if( !handle )
		{
			this->askEvent( handle, EVENT_GLOBAL_MOUSE_MOVE, "(Offi)", this->getEmbedding(), _x, _y, _whell );
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool GlobalHandleAdapter::handleGlobalKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		bool handle = false;

		if( !handle )
		{
			this->askEvent( handle, EVENT_GLOBAL_KEY, "(OIIb)", this->getEmbedding(), _key, _char, _isDown );
		}

		return handle;
	}
}