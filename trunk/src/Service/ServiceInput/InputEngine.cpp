#	include "InputEngine.h"

#	include "Interface/NotificationServiceInterface.h"
#	include "Interface/ApplicationInterface.h"

#	include "Logger/Logger.h"

#   include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( InputService, Menge::InputServiceInterface, Menge::InputEngine );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	InputEngine::InputEngine()
		: m_serviceProvider(nullptr)
		, m_cursorPosition(0.f, 0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	InputEngine::~InputEngine()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void InputEngine::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * InputEngine::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::initialize()
	{
		std::fill( m_keyBuffer, m_keyBuffer + sizeof(m_keyBuffer), false );

		m_mouseBuffer[0] = false;
		m_mouseBuffer[1] = false;
		m_mouseBuffer[2] = false;

        m_eventsAdd.reserve(16);
        m_events.reserve(16);
        m_keyEventParams.reserve(16);
        m_mouseButtonEventParams.reserve(16);
        m_mouseMoveEventParams.reserve(16);
        m_mousePositionEventParams.reserve(16);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::finalize()
	{
        m_events.clear();
        m_eventsAdd.clear();

		m_keyEventParams.clear();
		m_mouseButtonEventParams.clear();
		m_mouseMoveEventParams.clear();
		m_mousePositionEventParams.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::update()
	{
		TVectorKeyEventParams::iterator it_keyParams = m_keyEventParams.begin();
		TVectorMouseButtonParams::iterator it_mouseButtonParams = m_mouseButtonEventParams.begin();
		TVectorMouseMoveParams::iterator it_mouseMoveParams = m_mouseMoveEventParams.begin();
		TVectorMousePositionParams::iterator it_mousePositionParams = m_mousePositionEventParams.begin();

        m_events = m_eventsAdd;
        
		for( TVectorEventType::const_iterator 
			it = m_events.begin(), 
			it_end = m_events.end();
		it != it_end;
		++it )
		{
			const EventType & eventType = (*it);

            switch( eventType )
            {
            case ET_KEY:
                {
                    const KeyEventParams& params = (*it_keyParams);
                    this->keyEvent_( params );
                    ++it_keyParams;
                }break;
            case ET_MOUSEBUTTON:
                {
                    const MouseButtonParams& params = (*it_mouseButtonParams);
                    this->mouseButtonEvent_( params );
                    ++it_mouseButtonParams;
                }break;
            case ET_MOUSEMOVE:
                {
                    const MouseMoveParams& params = (*it_mouseMoveParams);
                    this->mouseMoveEvent_( params );
                    ++it_mouseMoveParams;
                }break;
			case ET_MOUSEWHEEL:
				{
					const MouseMoveParams& params = (*it_mouseMoveParams);
					this->mouseWheelEvent_( params );
					++it_mouseMoveParams;
				}break;				
            case ET_MOUSEPOSITION:
                {
                    const MousePositionParams & params = (*it_mousePositionParams);
                    this->mousePositionEvent_( params );
                    ++it_mousePositionParams;
                }break;
            case ET_MOUSEENTER:
                {
                    const MousePositionParams & params = (*it_mousePositionParams);
                    this->mouseEnterEvent_(params);
                    ++it_mousePositionParams;
                }break;
            case ET_MOUSELEAVE:
                {
                    const MousePositionParams & params = (*it_mousePositionParams);
                    this->mouseLeaveEvent_(params);
                    ++it_mousePositionParams;
                }
            }
		}

		m_events.clear();
        m_eventsAdd.clear();

		m_keyEventParams.clear();
		m_mouseButtonEventParams.clear();
		m_mouseMoveEventParams.clear();
        m_mousePositionEventParams.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::isAltDown() const
	{
		if( this->isKeyDown( KC_MENU ) == true )
		{
			return true;
		}

		if( this->isKeyDown( KC_LMENU ) == true )
		{
			return true;
		}

		if( this->isKeyDown( KC_RMENU ) == true )
		{
			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::isShiftDown() const
	{
		if( this->isKeyDown( KC_SHIFT ) == true )
		{
			return true;
		}

		if( this->isKeyDown( KC_LSHIFT ) == true )
		{
			return true;
		}

		if( this->isKeyDown( KC_RSHIFT ) == true )
		{
			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::isCtrlDown() const
	{
		if( this->isKeyDown( KC_CONTROL ) == true )
		{
			return true;
		}

		if( this->isKeyDown( KC_LCONTROL ) == true )
		{
			return true;
		}

		if( this->isKeyDown( KC_RCONTROL ) == true )
		{
			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::isKeyDown( uint32_t _keyCode ) const
	{
		bool isDown = m_keyBuffer[_keyCode];

		return isDown;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::isExclusiveKeyDown( uint32_t _keyCode ) const
	{
		bool isDown = m_keyBuffer[_keyCode];

		if( isDown == false )
		{
			return false;
		}

		uint32_t keyDownCount = std::count( m_keyBuffer, m_keyBuffer + 256, true );
		
		if( keyDownCount != 1 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::isAnyKeyDown() const
	{
		uint32_t keyDownCount = std::count( m_keyBuffer, m_keyBuffer + 256, true );

		return keyDownCount != 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::isAnyMouseButtonDown() const
	{
		return m_mouseBuffer[0] || m_mouseBuffer[1] || m_mouseBuffer[2];
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::isMouseButtonDown( uint32_t _button ) const
	{
		return m_mouseBuffer[_button];
	}
	//////////////////////////////////////////////////////////////////////////
	bool InputEngine::validCursorPosition( const mt::vec2f & _point ) const
	{
		if( _point.x < 0.f || _point.x > 1.f )
		{
			return false;
		}

		if( _point.y < 0.f || _point.y > 1.f )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::applyCursorPosition_( const mt::vec2f & _point )
	{		       
		bool change = false;
		if( mt::cmp_v2_v2( m_cursorPosition, _point ) == false )
		{
			change = true;
		}

		m_cursorPosition = _point;
		
		if( change == true )
		{
			for( TVectorMousePositionProviders::iterator
				it = m_mousePositionProviders.begin(),
				it_end = m_mousePositionProviders.end();
			it != it_end;
			++it )
			{
				(*it)->onMousePositionChange( m_cursorPosition );
			}
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void InputEngine::setCursorPosition( const mt::vec2f & _point )
    {
        this->applyCursorPosition_( _point );
    }
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & InputEngine::getCursorPosition() const
	{
		return m_cursorPosition;
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::addMousePositionProvider( InputMousePositionProvider * _provider )
	{
		m_mousePositionProviders.push_back( _provider );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::removeMousePositionProvider( InputMousePositionProvider * _provider )
	{
        TVectorMousePositionProviders::iterator it_found = 
            std::find( m_mousePositionProviders.begin(), m_mousePositionProviders.end(), _provider );

        if( it_found == m_mousePositionProviders.end() )
        {
            LOGGER_ERROR(m_serviceProvider)("InputEngine::removeMousePositionProvider not found provider"
                );

            return;
        }

        m_mousePositionProviders.erase( it_found );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::onFocus( bool _focus )
	{
		(void)_focus;

		std::fill( m_keyBuffer, m_keyBuffer + sizeof(m_keyBuffer), false );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::onKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown )
	{
		m_eventsAdd.push_back( ET_KEY );
		KeyEventParams params = { _point, _key, _char, _isDown };
		m_keyEventParams.push_back( params );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::onMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown )
	{
		m_eventsAdd.push_back( ET_MOUSEBUTTON );
		MouseButtonParams params = { _touchId, _point, _button, _isDown };
		m_mouseButtonEventParams.push_back( params );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::onMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y )
	{
		m_eventsAdd.push_back( ET_MOUSEMOVE );
		MouseMoveParams params = { _touchId, _point, _x, _y, 0 };
		m_mouseMoveEventParams.push_back( params );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::onMouseWheel( unsigned int _touchId, const mt::vec2f & _point, int _wheel )
	{
		m_eventsAdd.push_back( ET_MOUSEWHEEL );
		MouseMoveParams params = { _touchId, _point, 0.f, 0.f, _wheel };
		m_mouseMoveEventParams.push_back( params );
	}	
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::onMousePosition( unsigned int _touchId, const mt::vec2f & _point )
	{
		m_eventsAdd.push_back( ET_MOUSEPOSITION );
		MousePositionParams params = { _touchId, _point };
		m_mousePositionEventParams.push_back( params );
	}
    //////////////////////////////////////////////////////////////////////////
    void InputEngine::onMouseEnter( unsigned int _touchId, const mt::vec2f & _point )
    {
        m_eventsAdd.push_back( ET_MOUSEENTER );
        MousePositionParams params = { _touchId, _point };
        m_mousePositionEventParams.push_back( params );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputEngine::onMouseLeave( unsigned int _touchId, const mt::vec2f & _point )
    {
        m_eventsAdd.push_back( ET_MOUSELEAVE );
        MousePositionParams params = { _touchId, _point };
        m_mousePositionEventParams.push_back( params );
    }
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::keyEvent_( const KeyEventParams& _params )
	{
		bool repeating = ( m_keyBuffer[_params.key] == true && _params.isDown == true );
		
		m_keyBuffer[_params.key] = _params.isDown;

		this->applyCursorPosition_( _params.point );

		APPLICATION_SERVICE(m_serviceProvider)
			->keyEvent( _params.point, _params.key, _params.character, _params.isDown, repeating );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::mouseButtonEvent_( const MouseButtonParams & _params )
	{
		m_mouseBuffer[ _params.button ] = _params.isDown;

		this->applyCursorPosition_( _params.point );

		APPLICATION_SERVICE(m_serviceProvider)
			->mouseButtonEvent( _params.touchId, _params.point, _params.button, _params.isDown );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::mouseMoveEvent_( const MouseMoveParams& _params )
	{
		this->applyCursorPosition_( _params.point );

		APPLICATION_SERVICE(m_serviceProvider)
			->mouseMove( _params.touchId, _params.point, _params.x, _params.y );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::mouseWheelEvent_( const MouseMoveParams& _params )
	{
		this->applyCursorPosition_( _params.point );

		APPLICATION_SERVICE(m_serviceProvider)
			->mouseWheel( _params.touchId, _params.point, _params.wheel );
	}
	//////////////////////////////////////////////////////////////////////////
	void InputEngine::mousePositionEvent_( const MousePositionParams& _params )
	{
		this->applyCursorPosition_( _params.point );

		APPLICATION_SERVICE(m_serviceProvider)
			->mousePosition( _params.touchId, _params.point );
	}
    //////////////////////////////////////////////////////////////////////////
    void InputEngine::mouseEnterEvent_( const MousePositionParams& _params )
    {
        this->applyCursorPosition_( _params.point );

        APPLICATION_SERVICE(m_serviceProvider)
            ->mouseEnter( _params.point );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputEngine::mouseLeaveEvent_( const MousePositionParams& _params )
    {
        this->applyCursorPosition_( _params.point );

        APPLICATION_SERVICE(m_serviceProvider)
            ->mouseLeave();
    }
}
