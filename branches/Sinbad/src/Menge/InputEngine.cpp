#	include "InputEngine.h"
#	include "Application.h"

# define WHEEL_DELTA 120

namespace Menge
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	InputEngine::InputEngine()
		: m_interface( NULL )
		, m_mousePos(0.f, 0.f)
		, m_resolution( 1024, 768 )
		, m_mouseBounded( false )
	{
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	InputEngine::~InputEngine()
	{
		if( m_interface != NULL )
		{
			releaseInterfaceSystem( m_interface );
			m_interface = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::initialize( WindowHandle _winHandle )
	{
		bool result = initInterfaceSystem( &m_interface );
		if( ( result == false ) || ( m_interface == NULL ) )
		{
			return false;
		}

		m_interface->regHandle( this );

		if( m_interface->initialize( _winHandle ) == false )
		{
			return false;
		}

		//m_interface->captureMouse( 0, 0, 0, 0 );

		return true;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	void InputEngine::update()
	{
		m_interface->update();	
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	bool InputEngine::isKeyDown( int index )
	{
		return m_interface->isKeyDown( index );
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	bool InputEngine::isModifierDown( int _modifier )
	{
		return m_interface->isModifierDown( _modifier );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & InputEngine::getMousePosition() const
	{
		return m_mousePos;
	}
	//////////////////////////////////////////////////////////////////////////
	int InputEngine::getMouseWhell() const
	{
		return m_interface->getMouseWhell();
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::isAnyButtonDown() const
	{
		return m_interface->isAnyButtonDown();
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::isButtonDown( int _button ) const
	{
		return m_interface->isButtonDown( _button );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::setMousePosition( float _x, float _y )
	{
		m_mousePos.x = _x;
		m_mousePos.y = _y;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::setMouseBounded( bool _bounded )
	{
		//return;
		if( m_mouseBounded == _bounded )
		{
			return;
		}

		if( _bounded )
		{
			//const Resolution & resolution = Holder<Application>::hostage()->getCurrentResolution();

			float rx = static_cast<float>( m_resolution[0] );
			float ry = static_cast<float>( m_resolution[1] );

			m_interface->captureMouse( m_mousePos.x, m_mousePos.y, rx, ry );
		}
		else
		{
			//float x = m_interface->getMouseX();
			//float y = m_interface->getMouseY();
			m_interface->releaseMouse();
		}

		m_mouseBounded = _bounded;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::getMouseBounded() const
	{
		return m_mouseBounded;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::setResolution( const Resolution & _resolution )
	{
		m_resolution = _resolution;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		return Holder<Application>::hostage()->onKeyEvent( _key, _char, _isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::handleMouseButtonEvent( unsigned int _button, bool _isDown )
	{
		return Holder<Application>::hostage()->onMouseButtonEvent( _button, _isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::handleMouseButtonEventEnd( unsigned int _button, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::handleMouseMove( float _x, float _y, int _whell )
	{
		_whell /= WHEEL_DELTA;

		return Holder<Application>::hostage()->onMouseMove( _x, _y, _whell );
	}
	//////////////////////////////////////////////////////////////////////////
}
