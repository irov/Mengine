#include "InputService.h"

#include "Interface/NotificationServiceInterface.h"
#include "Interface/ApplicationInterface.h"

#include "Kernel/Logger.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( InputService, Mengine::InputService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    InputService::InputService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    InputService::~InputService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool InputService::_initializeService()
    {
        for( uint32_t i = 0; i != MENGINE_INPUT_MAX_TOUCH; ++i )
        {
            m_cursorPosition[i] = mt::vec2f( 0.f, 0.f );
        }

        std::fill( m_keyBuffer, m_keyBuffer + sizeof( m_keyBuffer ), false );
        std::fill( m_mouseBuffer, m_mouseBuffer + sizeof( m_mouseBuffer ), false );

        m_eventsAdd.reserve( 16 );
        m_events.reserve( 16 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::_finalizeService()
    {
        m_events.clear();
        m_eventsAdd.clear();

        m_mousePositionProviders.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::update()
    {
        std::swap( m_events, m_eventsAdd );
        m_eventsAdd.clear();

        for( const InputUnionEvent & ev : m_events )
        {
            switch( ev.type )
            {
            case IET_KEY:
                {
                    this->keyEvent_( ev.key );
                }break;
            case IET_TEXT:
                {
                    this->textEvent_( ev.text );
                }break;
            case IET_MOUSE_BUTTON:
                {
                    this->mouseButtonEvent_( ev.button );
                }break;
            case IET_MOUSE_WHELL:
                {
                    this->mouseWheelEvent_( ev.wheel );
                }break;
            case IET_MOUSE_MOVE:
                {
                    this->mouseMoveEvent_( ev.move );
                }break;
            case IET_MOUSE_POSITION:
                {
                    this->mousePositionEvent_( ev.position );
                }break;
            case IET_MOUSE_ENTER:
                {
                    this->mouseEnterEvent_( ev.position );
                }break;
            case IET_MOUSE_LEAVE:
                {
                    this->mouseLeaveEvent_( ev.position );
                }
            }
        }

        m_events.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool InputService::isAltDown() const
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
    bool InputService::isShiftDown() const
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
    bool InputService::isCtrlDown() const
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
    bool InputService::isKeyDown( uint32_t _keyCode ) const
    {
        bool isDown = m_keyBuffer[_keyCode];

        return isDown;
    }
    //////////////////////////////////////////////////////////////////////////
    bool InputService::isExclusiveKeyDown( uint32_t _keyCode ) const
    {
        bool isDown = m_keyBuffer[_keyCode];

        if( isDown == false )
        {
            return false;
        }

        std::ptrdiff_t keyDownCount = std::count( m_keyBuffer, m_keyBuffer + 256, true );

        if( keyDownCount != 1 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool InputService::isAnyKeyDown() const
    {
        std::ptrdiff_t keyDownCount = std::count( m_keyBuffer, m_keyBuffer + 256, true );

        return keyDownCount != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool InputService::isAnyMouseButtonDown() const
    {
        return m_mouseBuffer[0] || m_mouseBuffer[1] || m_mouseBuffer[2];
    }
    //////////////////////////////////////////////////////////////////////////
    bool InputService::isMouseButtonDown( uint32_t _button ) const
    {
        return m_mouseBuffer[_button];
    }
    //////////////////////////////////////////////////////////////////////////
    bool InputService::validCursorPosition( float _x, float _y, float * _vx, float * _vy ) const
    {
        bool inside = true;

        if( _x < 0.f )
        {
            *_vx = 0.f;

            inside = false;
        }
        else if( _x > 1.f )
        {
            *_vx = 1.f;

            inside = false;
        }
        else
        {
            *_vx = _x;
        }

        if( _y < 0.f )
        {
            *_vy = 0.f;

            inside = false;
        }
        else if( _y > 1.f )
        {
            *_vy = 1.f;

            inside = false;
        }
        else
        {
            *_vy = _y;
        }

        return inside;
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::applyCursorPosition_( uint32_t _touchId, float _x, float _y )
    {
        if( _touchId >= MENGINE_INPUT_MAX_TOUCH )
        {
            return;
        }

        mt::vec2f point( _x, _y );

        bool change = false;
        if( mt::cmp_v2_v2( m_cursorPosition[_touchId], point ) == false )
        {
            change = true;
        }

        m_cursorPosition[_touchId] = point;

        if( change == true )
        {
            for( const InputMousePositionProviderDesc & desc : m_mousePositionProviders )
            {
                const InputMousePositionProviderInterfacePtr & provider = desc.provider;

                provider->onMousePositionChange( _touchId, point );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::setCursorPosition( uint32_t _touchId, const mt::vec2f & _point )
    {
        this->applyCursorPosition_( _touchId, _point.x, _point.y );
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & InputService::getCursorPosition( uint32_t _touchId ) const
    {
        return m_cursorPosition[_touchId];
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t InputService::addMousePositionProvider( const InputMousePositionProviderInterfacePtr & _provider )
    {
        uint32_t new_id = ++m_enumerator;

        InputMousePositionProviderDesc desc;
        desc.id = new_id;
        desc.provider = _provider;

        m_mousePositionProviders.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    class InputService::FMousePositionProviderFind
    {
    public:
        FMousePositionProviderFind( uint32_t _id )
            : m_id( _id )
        {
        }

    public:
        bool operator()( const InputService::InputMousePositionProviderDesc & _desc ) const
        {
            return _desc.id == m_id;
        }

    protected:
        uint32_t m_id;
    };
    //////////////////////////////////////////////////////////////////////////
    void InputService::removeMousePositionProvider( uint32_t _id )
    {
        VectorMousePositionProviders::iterator it_found =
            std::find_if( m_mousePositionProviders.begin(), m_mousePositionProviders.end(), FMousePositionProviderFind( _id ) );

        if( it_found == m_mousePositionProviders.end() )
        {
            LOGGER_ERROR( "InputService::removeMousePositionProvider not found provider"
            );

            return;
        }

        m_mousePositionProviders.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::onFocus( bool _focus )
    {
        (void)_focus;

        std::fill( m_keyBuffer, m_keyBuffer + sizeof( m_keyBuffer ), false );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::pushEvent( const InputUnionEvent & _event )
    {
        m_eventsAdd.emplace_back( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::keyEvent_( const InputKeyEvent & _params )
    {
        bool isRepeat = (m_keyBuffer[_params.code] == true && _params.isDown == true);

        m_keyBuffer[_params.code] = _params.isDown;

        InputKeyEvent event;
        event.type = _params.type;
        event.x = _params.x;
        event.y = _params.y;
        event.code = _params.code;
        event.isDown = _params.isDown;
        event.isRepeat = isRepeat;

        APPLICATION_SERVICE()
            ->keyEvent( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::textEvent_( const InputTextEvent & _params )
    {
        APPLICATION_SERVICE()
            ->textEvent( _params );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::mouseButtonEvent_( const InputMouseButtonEvent & _params )
    {
        m_mouseBuffer[_params.button] = _params.isDown;

        this->applyCursorPosition_( _params.touchId, _params.x, _params.y );

        APPLICATION_SERVICE()
            ->mouseButtonEvent( _params );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::mouseMoveEvent_( const InputMouseMoveEvent & _params )
    {
        if( PLATFORM_SERVICE()
            ->hasTouchpad() == true )
        {
            if( this->isAnyMouseButtonDown() == true )
            {
                this->applyCursorPosition_( _params.touchId, _params.x, _params.y );

                APPLICATION_SERVICE()
                    ->mouseMove( _params );
            }
        }
        else
        {
            this->applyCursorPosition_( _params.touchId, _params.x, _params.y );

            APPLICATION_SERVICE()
                ->mouseMove( _params );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::mouseWheelEvent_( const InputMouseWheelEvent& _params )
    {
        APPLICATION_SERVICE()
            ->mouseWheel( _params );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::mousePositionEvent_( const InputMousePositionEvent& _params )
    {
        this->applyCursorPosition_( _params.touchId, _params.x, _params.y );

        APPLICATION_SERVICE()
            ->mousePosition( _params );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::mouseEnterEvent_( const InputMousePositionEvent& _params )
    {
        this->applyCursorPosition_( _params.touchId, _params.x, _params.y );

        APPLICATION_SERVICE()
            ->mouseEnter( _params );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::mouseLeaveEvent_( const InputMousePositionEvent& _params )
    {
        this->applyCursorPosition_( _params.touchId, _params.x, _params.y );

        APPLICATION_SERVICE()
            ->mouseLeave( _params );
    }
}
