#	include "InputEngine.h"
#	include "Application.h"

//# define WHEEL_DELTA 120

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	InputEngine::InputEngine()
		: m_cursorPosition(0.f, 0.f)
		, m_inputScale(0.f, 0.f)
		, m_inputOffset(0.f, 0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	InputEngine::~InputEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::setDimentions( const Resolution & _contentResolution, const Viewport & _viewport )
	{
		float viewport_width = _viewport.getWidth();
		float viewport_height = _viewport.getHeight();

		m_dimentions.x = float(_contentResolution.getWidth());
		m_dimentions.y = float(_contentResolution.getHeight());

		float width_scale = viewport_width / m_dimentions.x;
		float height_scale = viewport_height / m_dimentions.y;

		m_inputOffset = _viewport.begin;

		m_inputScale.x = 1.f / width_scale;
		m_inputScale.y = 1.f / height_scale;
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

		for( TVectorEventType::iterator 
			it = m_events.begin(), 
			it_end = m_events.end();
		it != it_end;
		++it )
		{
			EventType& eventType = (*it);
			if( eventType == ET_KEY )
			{
				const KeyEventParams& keyParams = (*it_keyParams);
				this->keyEvent( keyParams );
				++it_keyParams;
			}
			else if( eventType == ET_MOUSEBUTTON )
			{
				const MouseButtonParams& mouseButtonParams = (*it_mouseButtonParams);
				this->mouseButtonEvent( mouseButtonParams );
				++it_mouseButtonParams;
			}
			else if( eventType == ET_MOUSEMOVE )
			{
				const MouseMoveParams& mouseMoveParams = (*it_mouseMoveParams);
				this->mouseMoveEvent( mouseMoveParams );
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
	bool InputEngine::validCursorPosition( const mt::vec2f & _point ) const
	{
		if( _point.x < 0.f || _point.x > m_dimentions.x )
		{
			return false;
		}

		if( _point.y < 0.f || _point.y > m_dimentions.y )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::applyCursorPosition_( const mt::vec2f & _point, mt::vec2f & _local ) const
	{
		mt::vec2f scale_point;
		mt::vec2f offset_point = _point - m_inputOffset;
		//mt::vec2f offset_point = _point;
		mt::mul_v2_v2( scale_point, offset_point, m_inputScale );

		_local = scale_point;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::setCursorPosition( const mt::vec2f & _point )
	{
		this->applyCursorPosition_( _point, m_cursorPosition );

		for( TVectorCursorPositionProviders::iterator
			it = m_cursorPositionProviders.begin(),
			it_end = m_cursorPositionProviders.end();
		it != it_end;
		++it )
		{
			(*it)->onCursorPositionChange( m_cursorPosition );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & InputEngine::getCursorPosition() const
	{
		return m_cursorPosition;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::addCursorPositionProvider( CursorPositionProvider * _provider )
	{
		m_cursorPositionProviders.push_back( _provider );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::removeCursorPositionProvider( CursorPositionProvider * _provider )
	{
		std::remove( m_cursorPositionProviders.begin(), m_cursorPositionProviders.end(), _provider ); 
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

			mt::vec2f point;
			this->applyCursorPosition_( _keyEventParams.point, point );

			Application::get()
				->onKeyEvent( point, _keyEventParams.key, _keyEventParams.character, _keyEventParams.isDown );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::mouseButtonEvent( const MouseButtonParams& _mouseButtonParams )
	{
		m_mouseBuffer[ _mouseButtonParams.button ] = _mouseButtonParams.isDown;

		mt::vec2f point;
		this->applyCursorPosition_( _mouseButtonParams.point, point );

		Application::get()
			->onMouseButtonEvent( point, _mouseButtonParams.button, _mouseButtonParams.isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::mouseMoveEvent( const MouseMoveParams& _mouseMoveParams )
	{
		mt::vec2f point;
		this->applyCursorPosition_( _mouseMoveParams.point, point );

		Application::get()
			->onMouseMove( point, static_cast<float>( _mouseMoveParams.x), static_cast<float>( _mouseMoveParams.y), _mouseMoveParams.z );
	}
	//////////////////////////////////////////////////////////////////////////
}
