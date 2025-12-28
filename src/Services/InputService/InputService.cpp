#include "InputService.h"

#include "Interface/ApplicationInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionContainer.h"
#include "Kernel/Error.h"
#include "Kernel/ProfilerHelper.h"
#include "Kernel/EnumeratorHelper.h"
#include "Kernel/ThreadMutexHelper.h"

#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( InputService, Mengine::InputService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    InputService::InputService()
    {
        StdAlgorithm::fill_n( m_cursorPosition, MENGINE_INPUT_MAX_TOUCH, mt::vec2f( 0.f, 0.f ) );
        StdAlgorithm::fill_n( m_cursorPressure, MENGINE_INPUT_MAX_TOUCH, 0.f );
        StdAlgorithm::fill_n( m_keyBuffer, MENGINE_INPUT_MAX_KEY_CODE, false );
        StdAlgorithm::fill_n( m_mouseBuffer, MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE, false );
    }
    //////////////////////////////////////////////////////////////////////////
    InputService::~InputService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool InputService::_initializeService()
    {
        m_mutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        m_events.reserve( 16 );
        m_eventsAux.reserve( 16 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::_finalizeService()
    {
#if defined(MENGINE_DOCUMENT_ENABLE)
        for( const InputMousePositionProviderDesc & desc : m_mousePositionProviders )
        {
            LOGGER_ASSERTION( "forgot remove input mouse position provider (doc: %s)"
                , MENGINE_DOCUMENT_STR( desc.doc )
            );
        }
#endif

        MENGINE_ASSERTION_CONTAINER_EMPTY( m_mousePositionProviders );

        m_mousePositionProviders.clear();

        m_mutex = nullptr;

        m_eventsAux.clear();
        m_events.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::_update()
    {
        MENGINE_PROFILER_CATEGORY();

        m_mutex->lock();
        std::swap( m_eventsAux, m_events );
        m_events.clear();
        m_mutex->unlock();

        for( const InputVariantEvent & ev : m_eventsAux )
        {
            Helper::visit( ev, [this]( const auto & _event )
            {
                this->dispatchEvent_( _event );
            } );
        }

        m_eventsAux.clear();
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

        std::ptrdiff_t keyDownCount = StdAlgorithm::count( m_keyBuffer, m_keyBuffer + MENGINE_INPUT_MAX_KEY_CODE, true );

        if( keyDownCount != 1 )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool InputService::isAnyKeyDown() const
    {
        std::ptrdiff_t keyDownCount = StdAlgorithm::count( m_keyBuffer, m_keyBuffer + MENGINE_INPUT_MAX_KEY_CODE, true );

        return keyDownCount != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool InputService::isAnyMouseButtonDown() const
    {
        std::ptrdiff_t mouseDownCount = StdAlgorithm::count( m_mouseBuffer, m_mouseBuffer + MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE, true );

        return mouseDownCount != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool InputService::isMouseButtonDown( EMouseButtonCode _button ) const
    {
        MENGINE_ASSERTION_FATAL( _button < MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE, "invalid button code %u"
            , _button
        );

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
    void InputService::applyCursorPosition_( ETouchCode _touchId, const mt::vec2f & _screenPosition, float _pressure )
    {
        MENGINE_ASSERTION_FATAL( _touchId < MENGINE_INPUT_MAX_TOUCH, "invalid touch id %u"
            , _touchId
        );

        const mt::vec2f & currentPosition = m_cursorPosition[_touchId];

        bool change = false;

        if( mt::cmp_v2_v2( currentPosition, _screenPosition ) == false )
        {
            change = true;
        }

        float currentPressure = m_cursorPressure[_touchId];

        if( mt::equal_f_f( currentPressure, _pressure ) == false )
        {
            change = true;
        }

        m_cursorPosition[_touchId] = _screenPosition;
        m_cursorPressure[_touchId] = _pressure;

        if( change == true )
        {
            for( const InputMousePositionProviderDesc & desc : m_mousePositionProviders )
            {
                const InputMousePositionProviderInterfacePtr & provider = desc.provider;

                provider->onMousePositionChange( _touchId, _screenPosition, _pressure );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::setCursorPosition( ETouchCode _touchId, const mt::vec2f & _screenPosition, float _pressure )
    {
        this->applyCursorPosition_( _touchId, _screenPosition, _pressure );
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & InputService::getCursorPosition( ETouchCode _touchId ) const
    {
        MENGINE_ASSERTION_FATAL( _touchId < MENGINE_INPUT_MAX_TOUCH, "invalid touch id %u"
            , _touchId
        );

        const mt::vec2f & position = m_cursorPosition[_touchId];

        return position;
    }
    //////////////////////////////////////////////////////////////////////////
    float InputService::getCursorPressure( ETouchCode _touchId ) const
    {
        MENGINE_ASSERTION_FATAL( _touchId < MENGINE_INPUT_MAX_TOUCH, "invalid touch id %u"
            , _touchId
        );

        float pressure = m_cursorPressure[_touchId];

        return pressure;
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::addMousePositionProvider( const InputMousePositionProviderInterfacePtr & _provider, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        InputMousePositionProviderDesc desc;
        desc.provider = _provider;

#if defined(MENGINE_DOCUMENT_ENABLE)
        desc.doc = _doc;
#endif

        m_mousePositionProviders.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::removeMousePositionProvider( const InputMousePositionProviderInterfacePtr & _provider )
    {
        VectorMousePositionProviders::iterator it_found =
            StdAlgorithm::find_if( m_mousePositionProviders.begin(), m_mousePositionProviders.end(), [_provider]( const InputService::InputMousePositionProviderDesc & _desc )
        {
            return _desc.provider == _provider;
        } );

        if( it_found == m_mousePositionProviders.end() )
        {
            LOGGER_ERROR( "not found mouse provider" );

            return;
        }

        m_mousePositionProviders.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::onFocus( bool _focus )
    {
        MENGINE_UNUSED( _focus );

        StdAlgorithm::fill( m_keyBuffer, m_keyBuffer + MENGINE_INPUT_MAX_KEY_CODE, false );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::pushEvent( const InputVariantEvent & _event )
    {
        m_mutex->lock();
        m_events.emplace_back( _event );
        m_mutex->unlock();
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
    void InputService::dispatchEvent_( const InputKeyEvent & _event )
    {
        LOGGER_INFO( "input", "handle symbol code [%u] down [%u] repeat [%u]"
            , _event.code
            , _event.isDown
            , _event.isRepeat
        );

        m_keyBuffer[_event.code] = _event.isDown;

        APPLICATION_SERVICE()
            ->handleKeyEvent( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::dispatchEvent_( const InputTextEvent & _event )
    {
        LOGGER_INFO( "input", "handle text: %ls"
            , _event.text
        );

        APPLICATION_SERVICE()
            ->handleTextEvent( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::dispatchEvent_( const InputAccelerometerEvent & _event )
    {
        LOGGER_INFO( "input", "handle accelerometer: %.4f %.4f %.4f"
            , _event.dx
            , _event.dy
            , _event.dz
        );

        APPLICATION_SERVICE()
            ->handleAccelerometerEvent( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::dispatchEvent_( const InputMouseButtonEvent & _event )
    {
        if( _event.touchId == TC_TOUCH_INVALID )
        {
            return;
        }

        MENGINE_ASSERTION_FATAL( _event.touchId < MENGINE_INPUT_MAX_TOUCH, "invalid touch id %u"
            , _event.touchId
        );

        MENGINE_ASSERTION_FATAL( _event.button < MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE, "invalid button code %u"
            , _event.button
        );

        LOGGER_INFO( "input", "handle mouse button touch [%u] pos [%.4f:%.4f] button [%u] down [%u] pressed [%u] pressure [%f]"
            , _event.touchId
            , _event.position.screen.x
            , _event.position.screen.y
            , _event.button
            , _event.isDown
            , _event.isPressed
            , _event.pressure
        );

        m_mouseBuffer[_event.button] = _event.isDown;

        this->applyCursorPosition_( _event.touchId, _event.position.screen, _event.pressure );

        APPLICATION_SERVICE()
            ->handleMouseButtonEventBegin( _event );

        APPLICATION_SERVICE()
            ->handleMouseButtonEvent( _event );

        APPLICATION_SERVICE()
            ->handleMouseButtonEventEnd( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::dispatchEvent_( const InputMouseMoveEvent & _event )
    {
        if( _event.touchId == TC_TOUCH_INVALID )
        {
            return;
        }

        MENGINE_ASSERTION_FATAL( _event.touchId < MENGINE_INPUT_MAX_TOUCH, "invalid touch id %u"
            , _event.touchId
        );

        LOGGER_INFO( "input", "handle mouse move touch [%u] pos [%.4f:%.4f] delta [%.8f:%.8f] pressure [%f]"
            , _event.touchId
            , _event.position.screen.x
            , _event.position.screen.y
            , _event.screenDelta.x
            , _event.screenDelta.y
            , _event.pressure
        );

        this->applyCursorPosition_( _event.touchId, _event.position.screen, _event.pressure );

        APPLICATION_SERVICE()
            ->handleMouseMove( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::dispatchEvent_( const InputMouseWheelEvent & _event )
    {
        LOGGER_INFO( "input", "handle mouse wheel wheel [%u] scroll [%d] pos [%.4f:%.4f] "
            , _event.wheel
            , _event.scroll
            , _event.position.screen.x
            , _event.position.screen.y
        );

        APPLICATION_SERVICE()
            ->handleMouseWheel( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::dispatchEvent_( const InputMouseEnterEvent & _event )
    {
        MENGINE_ASSERTION_FATAL( _event.touchId < MENGINE_INPUT_MAX_TOUCH, "invalid touch id %u"
            , _event.touchId
        );

        LOGGER_INFO( "input", "handle mouse enter touch [%u] pos [%.4f:%.4f] pressure [%f]"
            , _event.touchId
            , _event.position.screen.x
            , _event.position.screen.y
            , _event.pressure
        );

        this->applyCursorPosition_( _event.touchId, _event.position.screen, _event.pressure );

        APPLICATION_SERVICE()
            ->handleMouseEnter( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    void InputService::dispatchEvent_( const InputMouseLeaveEvent & _event )
    {
        MENGINE_ASSERTION_FATAL( _event.touchId < MENGINE_INPUT_MAX_TOUCH, "invalid touch id %u"
            , _event.touchId
        );

        LOGGER_INFO( "input", "handle mouse leave touch [%u] pos [%.4f:%.4f] pressure [%f]"
            , _event.touchId
            , _event.position.screen.x
            , _event.position.screen.y
            , _event.pressure
        );

        APPLICATION_SERVICE()
            ->handleMouseLeave( _event );
    }
    //////////////////////////////////////////////////////////////////////////
}
