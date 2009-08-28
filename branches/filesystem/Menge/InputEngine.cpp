#	include "InputEngine.h"
#	include "Application.h"
#	include "Player.h"
#	include "Arrow.h"

# define WHEEL_DELTA 120

namespace Menge
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	InputEngine::InputEngine( InputSystemInterface * _interface )
		: m_interface( _interface )
		, m_mouseX(0.0f)
		, m_mouseY(0.0f)
		, m_boundX( 1024 )
		, m_boundY( 768 )
		, m_mouseBounded( false )
	{
		m_interface->regHandle( this );
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	InputEngine::~InputEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::initialize( WindowHandle _winHandle )
	{
		bool result = m_interface->initialize( _winHandle );
		//m_interface->captureMouse( 0, 0, 0, 0 );

		return result;
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
	float InputEngine::getMouseX() const
	{
		return m_mouseX;
	}
	//////////////////////////////////////////////////////////////////////////
	float InputEngine::getMouseY() const
	{
		return m_mouseY;
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
	void InputEngine::setMousePos( float _x, float _y )
	{
		m_mouseX = _x;
		m_mouseY = _y;
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

			float rx = static_cast<float>( m_boundX );
			float ry = static_cast<float>( m_boundY );

			m_interface->captureMouse( m_mouseX, m_mouseY, rx, ry );
		}
		else
		{
			//float x = m_interface->getMouseX();
			//float y = m_interface->getMouseY();
			m_interface->releaseMouse();
			//Holder<Player>::hostage()->getArrow()->setLocalPosition( mt::vec2f(m_mouseX, m_mouseY) );
		}

		m_mouseBounded = _bounded;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::getMouseBounded() const
	{
		return m_mouseBounded;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::setResolution( int _x, int _y )
	{
		m_boundX = _x;
		m_boundY = _y;
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
		m_mouseX += _x;
		m_mouseY += _y;

		_whell /= WHEEL_DELTA;

		if( m_mouseX < 0 )
			m_mouseX = 0;
		else if( m_mouseX > m_boundX )
			m_mouseX = m_boundX;
		if( m_mouseY < 0 )
			m_mouseY = 0;
		else if( m_mouseY > m_boundY )
			m_mouseY = m_boundY;

		Holder<Player>::hostage()->getArrow()->setLocalPosition( mt::vec2f(m_mouseX, m_mouseY) );
		return Holder<Application>::hostage()->onMouseMove( _x, _y, _whell );
	}
	//////////////////////////////////////////////////////////////////////////
}
