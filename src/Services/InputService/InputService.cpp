#include "InputService.h"

#include "Interface/NotificationServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/EnumeratorServiceInterface.h"

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
            m_cursorPressure[i] = 0.f;
        }

        std::fill( m_keyBuffer, m_keyBuffer + sizeof( m_keyBuffer ), false );
        std::fill( m_mouseBuffer, m_mouseBuffer + sizeof( m_mouseBuffer ), false );
        std::fill( m_mouseBufferSpecial, m_mouseBufferSpecial + sizeof( m_mouseBufferSpecial ), false );

        m_eventsAdd.reserve( 16 );
        m_events.reserve( 16 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::_finalizeService()
    {
        m_events.clear();
        m_eventsAdd.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::_stopService()
    {
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
                    this->mouseEnterEvent_( ev.enter );
                }break;
            case IET_MOUSE_LEAVE:
                {
                    this->mouseLeaveEvent_( ev.leave );
                }
            }
        }

        m_events.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool InputService::isSpecialDown() const
    {
        if( this->isAltDown() == true )
        {
            return true;
        }

        if( this->isShiftDown() == true )
        {
            return true;
        }

        if( this->isControlDown() == true )
        {
            return true;
        }

        return false;
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
    bool InputService::isControlDown() const
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
    void InputService::applyCursorPosition_( uint32_t _touchId, float _x, float _y, float _pressure )
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

        if( mt::equal_f_f( m_cursorPressure[_touchId], _pressure ) == false )
        {
            change = true;
        }

        m_cursorPosition[_touchId] = point;
        m_cursorPressure[_touchId] = _pressure;

        if( change == true )
        {
            for( const InputMousePositionProviderDesc & desc : m_mousePositionProviders )
            {
                const InputMousePositionProviderInterfacePtr & provider = desc.provider;

                provider->onMousePositionChange( _touchId, point, _pressure );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::setCursorPosition( uint32_t _touchId, const mt::vec2f & _point, float _pressure )
    {
        this->applyCursorPosition_( _touchId, _point.x, _point.y, _pressure );
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & InputService::getCursorPosition( uint32_t _touchId ) const
    {
        return m_cursorPosition[_touchId];
    }
    //////////////////////////////////////////////////////////////////////////
    float InputService::getCursorPressure( uint32_t _touchId ) const
    {
        return m_cursorPressure[_touchId];
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t InputService::addMousePositionProvider( const InputMousePositionProviderInterfacePtr & _provider )
    {
        uint32_t new_id = GENERATE_UNIQUE_IDENTITY();

        InputMousePositionProviderDesc desc;
        desc.id = new_id;
        desc.provider = _provider;

        m_mousePositionProviders.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::removeMousePositionProvider( uint32_t _id )
    {
        VectorMousePositionProviders::iterator it_found =
            std::find_if( m_mousePositionProviders.begin(), m_mousePositionProviders.end(), [_id]( const InputService::InputMousePositionProviderDesc & _desc )
        {
            return _desc.id == _id;
        } );

        if( it_found == m_mousePositionProviders.end() )
        {
            LOGGER_ERROR( "not found provider '%d'"
                , _id
            );

            return;
        }

        m_mousePositionProviders.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::onFocus( bool _focus )
    {
        MENGINE_UNUSED( _focus );

        std::fill( m_keyBuffer, m_keyBuffer + sizeof( m_keyBuffer ), false );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::pushEvent( const InputUnionEvent & _event )
    {
        m_eventsAdd.emplace_back( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::keyEvent_( const InputKeyEvent & _event )
    {
        bool isRepeat = (m_keyBuffer[_event.code] == true && _event.isDown == true);

        m_keyBuffer[_event.code] = _event.isDown;

        InputKeyEvent event;
        event.type = _event.type;
        event.x = _event.x;
        event.y = _event.y;
        event.code = _event.code;
        event.isDown = _event.isDown;
        event.isRepeat = isRepeat;

        APPLICATION_SERVICE()
            ->keyEvent( event );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::textEvent_( const InputTextEvent & _event )
    {
        APPLICATION_SERVICE()
            ->textEvent( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::mouseButtonEvent_( const InputMouseButtonEvent & _event )
    {
        m_mouseBuffer[_event.code] = _event.isDown;
        m_mouseBufferSpecial[_event.code] = this->isSpecialDown();

        this->applyCursorPosition_( _event.touchId, _event.x, _event.y, _event.pressure );

        APPLICATION_SERVICE()
            ->mouseButtonEvent( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::mouseMoveEvent_( const InputMouseMoveEvent & _event )
    {
        if( PLATFORM_SERVICE()
            ->hasTouchpad() == true )
        {
            if( this->isAnyMouseButtonDown() == true )
            {
                this->applyCursorPosition_( _event.touchId, _event.x, _event.y, _event.pressure );

                APPLICATION_SERVICE()
                    ->mouseMove( _event );
            }
        }
        else
        {
            this->applyCursorPosition_( _event.touchId, _event.x, _event.y, _event.pressure );

            APPLICATION_SERVICE()
                ->mouseMove( _event );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::mouseWheelEvent_( const InputMouseWheelEvent & _event )
    {
        APPLICATION_SERVICE()
            ->mouseWheel( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::mousePositionEvent_( const InputMousePositionEvent & _event )
    {
        this->applyCursorPosition_( _event.touchId, _event.x, _event.y, _event.pressure );

        APPLICATION_SERVICE()
            ->mousePosition( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::mouseEnterEvent_( const InputMouseEnterEvent & _event )
    {
        this->applyCursorPosition_( _event.touchId, _event.x, _event.y, _event.pressure );

        APPLICATION_SERVICE()
            ->mouseEnter( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::mouseLeaveEvent_( const InputMouseLeaveEvent & _event )
    {
        this->applyCursorPosition_( _event.touchId, _event.x, _event.y, _event.pressure );

        APPLICATION_SERVICE()
            ->mouseLeave( _event );
    }
}
