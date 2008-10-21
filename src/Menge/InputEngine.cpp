#	include "InputEngine.h"
#	include "InputHandler.h"
#	include "Application.h"
#	include "Player.h"
#	include "Arrow.h"
#	include "ProfilerEngine.h"

namespace Menge
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	InputEngine::InputEngine( InputSystemInterface * _interface )
		: m_interface( _interface )
		, m_mouseX(-1)
		, m_mouseY(-1)
		, m_mouseBounded( false )
		, m_isActive(true)
	{
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	InputEngine::~InputEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::initialize( WindowHandle _winHandle )
	{
		// хак для запуска SDL. если хэндл нулевой то OIS не инитить. 
		m_isActive = _winHandle ? true : false;

		bool result = true;
		
		if(m_isActive)
		{
			m_interface->initialize( _winHandle );
		}

		return result;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////
	void InputEngine::update()
	{
		if(m_isActive)
		{
			m_interface->update();	
		}
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
		//return;
		if( m_mouseBounded == _bounded )
		{
			return;
		}

		if( _bounded )
		{
			const Resolution & resolution = Holder<Application>::hostage()->getCurrentResolution();

			float rx = float( resolution[0] );
			float ry = float( resolution[1] );

			m_interface->captureMouse( m_mouseX, m_mouseY, rx, ry );
		}
		else
		{
			//float x = m_interface->getMouseX();
			//float y = m_interface->getMouseY();
			m_interface->releaseMouse();
			Holder<Player>::hostage()->getArrow()->setLocalPosition( mt::vec2f(m_mouseX, m_mouseY) );
		}

		m_mouseBounded = _bounded;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::getMouseBounded() const
	{
		return m_mouseBounded;
	}
}