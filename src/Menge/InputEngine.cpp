#	include "InputEngine.h"
#	include "InputHandler.h"
#	include "Application.h"
#	include "Player.h"
#	include "Arrow.h"

namespace Menge
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	InputEngine::InputEngine( InputSystemInterface * _interface )
		: m_interface( _interface )
		, m_mouseX(-1)
		, m_mouseY(-1)
		, m_mouseBounded( false )
	{
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	InputEngine::~InputEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::initialize( WINDOW_HANDLE _winHandle )
	{
		return m_interface->initialize( _winHandle );
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
		if( m_mouseBounded )
		{
			return m_interface->getMouseX();
		}
		return m_mouseX;
	}
	//////////////////////////////////////////////////////////////////////////
	float InputEngine::getMouseY() const
	{
		if( m_mouseBounded )
		{
			return m_interface->getMouseY();
		}
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
	void InputEngine::regHandle( InputHandler * _handle )
	{
		return m_interface->regHandle( _handle );
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
		m_mouseBounded = _bounded;

		if( m_mouseBounded )
		{
			const mt::vec2f& res = Holder<Application>::hostage()->getCurrentResolution();
			m_interface->captureMouse( m_mouseX, m_mouseY, res.x, res.y );
		}
		else
		{
			//float x = m_interface->getMouseX();
			//float y = m_interface->getMouseY();
			m_interface->releaseMouse();
			Holder<Player>::hostage()->getArrow()->setLocalPosition( mt::vec2f(m_mouseX, m_mouseY) );
		}

	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::getMouseBounded() const
	{
		return m_mouseBounded;
	}
}