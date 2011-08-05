#	include "InputEngine.h"
#	include "Application.h"

//# define WHEEL_DELTA 120

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	InputEngine::InputEngine()
		: m_cursorPosition(0.f, 0.f)
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
		TVectorMouseButtonParams::iterator it_mouseButtonParams = m_mouseButtonEventParams.begin();
		TVectorMouseMoveParams::iterator it_mouseMoveParams = m_mouseMoveEventParams.begin();
		for( TVectorEventType::iterator it = m_events.begin(), it_end = m_events.end();
			it != it_end;
			++it )
		{
			EventType& eventType = (*it);
			if( eventType == ET_KEY )
			{
				const KeyEventParams& keyParams = (*it_keyParams);
				keyEvent( keyParams );
				++it_keyParams;
			}
			else if( eventType == ET_MOUSEBUTTON )
			{
				const MouseButtonParams& mouseButtonParams = (*it_mouseButtonParams);
				mouseButtonEvent( mouseButtonParams );
				++it_mouseButtonParams;
			}
			else if( eventType == ET_MOUSEMOVE )
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
	void InputEngine::setCursorPosition( const mt::vec2f & _point )
	{
		m_cursorPosition = _point;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & InputEngine::getCursorPosition() const
	{
		return m_cursorPosition;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::setResolution( const Resolution & _resolution )
	{
		m_resolution = _resolution;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::pushKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
		m_events.push_back( ET_KEY );
		KeyEventParams keyEventParams = { _point, _key, _char, _isDown };
		m_keyEventParams.push_back( keyEventParams );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::pushMouseButtonEvent( const mt::vec2f & _point, int _button, bool _isDown )
	{
		m_events.push_back( ET_MOUSEBUTTON );
		MouseButtonParams mouseButtonParams = { _point, _button, _isDown };
		m_mouseButtonEventParams.push_back( mouseButtonParams );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::pushMouseMoveEvent( const mt::vec2f & _point, int _x, int _y, int _z )
	{
		m_events.push_back( ET_MOUSEMOVE );
		MouseMoveParams mouseMoveParams = { _point, _x, _y, _z };
		m_mouseMoveEventParams.push_back( mouseMoveParams );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::keyEvent( const KeyEventParams& _keyEventParams )
	{
		unsigned char state = _keyEventParams.isDown ? 1 : 0;
		if( m_keyBuffer[_keyEventParams.key] != state )
		{
			m_keyBuffer[_keyEventParams.key] = state;
			Application::get()
				->onKeyEvent( _keyEventParams.point, _keyEventParams.key, _keyEventParams.character, _keyEventParams.isDown );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::mouseButtonEvent( const MouseButtonParams& _mouseButtonParams )
	{
		m_mouseBuffer[ _mouseButtonParams.button ] = _mouseButtonParams.isDown;
		Application::get()
			->onMouseButtonEvent( _mouseButtonParams.point, _mouseButtonParams.button, _mouseButtonParams.isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::mouseMoveEvent( const MouseMoveParams& _mouseMoveParams )
	{
		Application::get()
			->onMouseMove( _mouseMoveParams.point, static_cast<float>( _mouseMoveParams.x), static_cast<float>( _mouseMoveParams.y), _mouseMoveParams.z );
	}
	//////////////////////////////////////////////////////////////////////////
}
