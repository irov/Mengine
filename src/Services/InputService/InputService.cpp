#include "InputService.h"

#include "Interface/NotificationServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionContainer.h"

#include "Config/Algorithm.h"

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
        m_eventsAux.reserve( 16 );
        m_events.reserve( 16 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::_finalizeService()
    {
        m_events.clear();
        m_eventsAux.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::_stopService()
    {
#if MENGINE_DOCUMENT_ENABLE
        for( const InputMousePositionProviderDesc & desc : m_mousePositionProviders )
        {
            LOGGER_ERROR( "forgot remove input mouse position provider (doc: %s)"
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#endif

        MENGINE_ASSERTION_CONTAINER_EMPTY( m_mousePositionProviders );

        m_mousePositionProviders.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::update()
    {
        m_events = std::move( m_eventsAux );

        for( const InputUnionEvent & ev : m_events )
        {
            switch( ev.type )
            {
            case IET_KEY:
                {
                    this->keyEvent_( ev.data.key );
                }break;
            case IET_TEXT:
                {
                    this->textEvent_( ev.data.text );
                }break;
            case IET_MOUSE_BUTTON:
                {
                    this->mouseButtonEvent_( ev.data.button );
                }break;
            case IET_MOUSE_WHELL:
                {
                    this->mouseWheelEvent_( ev.data.wheel );
                }break;
            case IET_MOUSE_MOVE:
                {
                    this->mouseMoveEvent_( ev.data.move );
                }break;
            case IET_MOUSE_POSITION:
                {
                    this->mousePositionEvent_( ev.data.position );
                }break;
            case IET_MOUSE_ENTER:
                {
                    this->mouseEnterEvent_( ev.data.enter );
                }break;
            case IET_MOUSE_LEAVE:
                {
                    this->mouseLeaveEvent_( ev.data.leave );
                }break;
            default:
                {
                    LOGGER_ERROR( "invalid input event type %u"
                        , ev.type
                    );
                }break;
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
    bool InputService::isKeyDown( EKeyCode _keyCode ) const
    {
        bool isDown = m_keyBuffer[_keyCode];

        return isDown;
    }
    //////////////////////////////////////////////////////////////////////////
    bool InputService::isExclusiveKeyDown( EKeyCode _keyCode ) const
    {
        bool isDown = m_keyBuffer[_keyCode];

        if( isDown == false )
        {
            return false;
        }

        std::ptrdiff_t keyDownCount = std::count( m_keyBuffer, m_keyBuffer + MENGINE_INPUT_MAX_KEY_CODE, true );

        if( keyDownCount != 1 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool InputService::isAnyKeyDown() const
    {
        std::ptrdiff_t keyDownCount = std::count( m_keyBuffer, m_keyBuffer + MENGINE_INPUT_MAX_KEY_CODE, true );

        return keyDownCount != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool InputService::isAnyMouseButtonDown() const
    {
        bool isDown0 = m_mouseBuffer[0];
        bool isDown1 = m_mouseBuffer[1];
        bool isDown2 = m_mouseBuffer[2];

        return isDown0 == true || isDown1 == true || isDown2 == true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool InputService::isMouseButtonDown( EMouseCode _button ) const
    {
        MENGINE_ASSERTION_FATAL( _button < MENGINE_INPUT_MAX_MOUSE_CODE );

        bool isDown = m_mouseBuffer[_button];

        return isDown;
    }
    //////////////////////////////////////////////////////////////////////////
    bool InputService::validCursorPosition( float _x, float _y, float * const _vx, float * const _vy ) const
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
    void InputService::applyCursorPosition_( ETouchCode _touchId, float _x, float _y, float _pressure )
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
    void InputService::setCursorPosition( ETouchCode _touchId, const mt::vec2f & _point, float _pressure )
    {
        this->applyCursorPosition_( _touchId, _point.x, _point.y, _pressure );
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & InputService::getCursorPosition( ETouchCode _touchId ) const
    {
        const mt::vec2f & position = m_cursorPosition[_touchId];

        return position;
    }
    //////////////////////////////////////////////////////////////////////////
    float InputService::getCursorPressure( ETouchCode _touchId ) const
    {
        float pressure = m_cursorPressure[_touchId];

        return pressure;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId InputService::addMousePositionProvider( const InputMousePositionProviderInterfacePtr & _provider, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        UniqueId new_id = ENUMERATOR_SERVICE()
            ->generateUniqueIdentity();

        InputMousePositionProviderDesc desc;
        desc.id = new_id;
        desc.provider = _provider;

#if MENGINE_DOCUMENT_ENABLE
        desc.doc = _doc;
#endif

        m_mousePositionProviders.emplace_back( desc );

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::removeMousePositionProvider( UniqueId _id )
    {
        VectorMousePositionProviders::iterator it_found =
            std::find_if( m_mousePositionProviders.begin(), m_mousePositionProviders.end(), [_id]( const InputService::InputMousePositionProviderDesc & _desc )
        {
            return _desc.id == _id;
        } );

        if( it_found == m_mousePositionProviders.end() )
        {
            LOGGER_ERROR( "not found provider '%u'"
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
        m_eventsAux.emplace_back( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::getSpecial( InputSpecialData * const _special ) const
    {
        bool isAlt = this->isAltDown();
        bool isShift = this->isShiftDown();
        bool isControl = this->isControlDown();
        bool isSpecial = this->isSpecialDown();

        _special->isAlt = isAlt;
        _special->isShift = isShift;
        _special->isControl = isControl;
        _special->isSpecial = isSpecial;
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::keyEvent_( const InputKeyEvent & _event )
    {
        bool isRepeat = (m_keyBuffer[_event.code] == true && _event.isDown == true);

        m_keyBuffer[_event.code] = _event.isDown;

        InputKeyEvent event = _event;
        
        //ToDo
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
        MENGINE_ASSERTION_FATAL( _event.code < MENGINE_INPUT_MAX_MOUSE_CODE );

        m_mouseBuffer[_event.code] = _event.isDown;

        this->applyCursorPosition_( _event.touchId, _event.x, _event.y, _event.pressure );

        APPLICATION_SERVICE()
            ->mouseButtonEvent( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::mouseMoveEvent_( const InputMouseMoveEvent & _event )
    {
        this->applyCursorPosition_( _event.touchId, _event.x, _event.y, _event.pressure );

        APPLICATION_SERVICE()
            ->mouseMove( _event );
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
    //////////////////////////////////////////////////////////////////////////
}
