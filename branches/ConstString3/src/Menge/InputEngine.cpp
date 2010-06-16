#	include "InputEngine.h"
#	include "Application.h"

//# define WHEEL_DELTA 120

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	InputEngine::InputEngine()
		: m_mousePos(0.f, 0.f)
		, m_resolution( 1024, 768 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	InputEngine::~InputEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::initialize()
	{
		std::fill( m_keyBuffer, m_keyBuffer + sizeof(m_keyBuffer), 0x00 );
		m_mouseBuffer[0] = false;
		m_mouseBuffer[1] = false;
		m_mouseBuffer[2] = false;
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::update()
	{
		TVectorKeyEventParams::iterator it_keyParams = m_keyEventParams.begin();
		TVectorKeyEventParams::iterator it_keyParams_end = m_keyEventParams.end();
		TVectorMouseButtonParams::iterator it_mouseButtonParams = m_mouseButtonEventParams.begin();
		TVectorMouseButtonParams::iterator it_mouseButtonParams_end = m_mouseButtonEventParams.end();
		TVectorMouseMoveParams::iterator it_mouseMoveParams = m_mouseMoveEventParams.begin();
		TVectorMouseMoveParams::iterator it_mouseMoveParams_end = m_mouseMoveEventParams.end();
		for( TVectorEventType::iterator it = m_events.begin(), it_end = m_events.end();
			it != it_end;
			++it )
		{
			EventType& eventType = (*it);
			if( eventType == ET_KEY && (it_keyParams != it_keyParams_end) )
			{
				const KeyEventParams& keyParams = (*it_keyParams);
				keyEvent( keyParams );
				++it_keyParams;
			}
			else if( eventType == ET_MOUSEBUTTON && (it_mouseButtonParams != it_mouseButtonParams_end) )
			{
				const MouseButtonParams& mouseButtonParams = (*it_mouseButtonParams);
				mouseButtonEvent( mouseButtonParams );
				++it_mouseButtonParams;
			}
			else if( eventType == ET_MOUSEMOVE && (it_mouseMoveParams != it_mouseMoveParams_end) )
			{
				const MouseMoveParams& mouseMoveParams = (*it_mouseMoveParams);
				mouseMoveEvent( mouseMoveParams );
				++it_mouseMoveParams;
			}
		}
		m_events.clear();
		m_keyEventParams.clear();
		m_mouseButtonEventParams.clear();
		m_mouseMoveEventParams.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::isKeyDown( KeyCode _keyCode )
	{
		return m_keyBuffer[ static_cast<int>( _keyCode ) ] == 1;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::isModifierDown( KeyModifier _modifier )
	{
		bool isDown = false;
		switch( _modifier )
		{
		case KM_CTRL:
			isDown = isKeyDown( KC_LCONTROL ) || isKeyDown( KC_RCONTROL );
			break;
		case KM_SHIFT:
			isDown = isKeyDown( KC_LSHIFT ) || isKeyDown( KC_RSHIFT );
			break;
		case KM_ALT:
			isDown = isKeyDown( KC_LMENU ) || isKeyDown( KC_RMENU );
			break;
		}
		return isDown;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & InputEngine::getMousePosition() const
	{
		return m_mousePos;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::isAnyMouseButtonDown() const
	{
		return m_mouseBuffer[0] || m_mouseBuffer[1] || m_mouseBuffer[2];
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::isMouseButtonDown( int _button ) const
	{
		return m_mouseBuffer[ _button ];
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::setMousePosition( float _x, float _y )
	{
		m_mousePos.x = _x;
		m_mousePos.y = _y;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::setResolution( const Resolution & _resolution )
	{
		m_resolution = _resolution;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::pushKeyEvent( unsigned int _key, unsigned int _char, bool _isDown )
	{
		m_events.push_back( ET_KEY );
		KeyEventParams keyEventParams = { _key, _char, _isDown };
		m_keyEventParams.push_back( keyEventParams );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::pushMouseButtonEvent( int _button, bool _isDown )
	{
		m_events.push_back( ET_MOUSEBUTTON );
		MouseButtonParams mouseButtonParams = { _button, _isDown };
		m_mouseButtonEventParams.push_back( mouseButtonParams );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::pushMouseMoveEvent( int _x, int _y, int _z )
	{
		m_events.push_back( ET_MOUSEMOVE );
		MouseMoveParams mouseMoveParams = { _x, _y, _z };
		m_mouseMoveEventParams.push_back( mouseMoveParams );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::keyEvent( const KeyEventParams& _keyEventParams )
	{
		unsigned char state = _keyEventParams.isDown ? 1 : 0;
		if( m_keyBuffer[_keyEventParams.key] != state )
		{
			m_keyBuffer[_keyEventParams.key] = state;
			Application::hostage()
				->onKeyEvent( _keyEventParams.key, _keyEventParams.character, _keyEventParams.isDown );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::mouseButtonEvent( const MouseButtonParams& _mouseButtonParams )
	{
		m_mouseBuffer[ _mouseButtonParams.button ] = _mouseButtonParams.isDown;
		Application::hostage()
			->onMouseButtonEvent( _mouseButtonParams.button, _mouseButtonParams.isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::mouseMoveEvent( const MouseMoveParams& _mouseMoveParams )
	{
		Application::hostage()
			->onMouseMove( static_cast<float>( _mouseMoveParams.x),
							static_cast<float>( _mouseMoveParams.y),
							_mouseMoveParams.z );
	}
	//////////////////////////////////////////////////////////////////////////
}
