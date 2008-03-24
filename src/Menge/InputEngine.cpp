#	include "InputEngine.h"
#	include "InputHandler.h"
#	include "Application.h"

namespace Menge
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	InputEngine::InputEngine( InputSystemInterface * _interface )
		: m_interface( _interface )
		, m_mouseX(-1)
		, m_mouseY(-1)
		, m_mouseBounded( false )
	{
		Holder<InputEngine>::keep(this);
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
	int InputEngine::getMouseX() const
	{
		if( m_mouseBounded )
		{
			return m_interface->getMouseX();
		}
		return m_mouseX;
	}
	//////////////////////////////////////////////////////////////////////////
	int InputEngine::getMouseY() const
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
	void InputEngine::setMousePos( int _x, int _y )
	{
		m_mouseX = _x;
		m_mouseY = _y;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::setMouseBounded( bool _bounded )
	{
		if( m_mouseBounded == _bounded ) return;

		m_mouseBounded = _bounded;

		if( m_mouseBounded )
		{
			const mt::vec2f& res = Holder<Application>::hostage()->getCurrentResolution();
			m_interface->captureMouse( m_mouseX, m_mouseY, res.x, res.y );
		}
		else
		{
			m_interface->releaseMouse();
		}

	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::getMouseBounded() const
	{
		return m_mouseBounded;
	}
}