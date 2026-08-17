#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/InputMousePositionProviderInterface.h"
#include "Interface/InputControllerHandlerInterface.h"

#include "Kernel/InputEvent.h"
#include "Kernel/KeyCode.h"
#include "Kernel/MouseButtonCode.h"
#include "Kernel/TouchCode.h"

#include "Config/Typedef.h"
#include "Config/UniqueId.h"
#include "Config/Variant.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class InputServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "InputService" )

    public:
        virtual bool isSpecialDown() const = 0;
        virtual bool isAltDown() const = 0;
        virtual bool isShiftDown() const = 0;
        virtual bool isControlDown() const = 0;

    public:
        virtual bool isKeyDown( EKeyCode _keyCode ) const = 0;
        virtual bool isExclusiveKeyDown( EKeyCode _keyCode ) const = 0;
        virtual bool isAnyKeyDown() const = 0;

    public:
        virtual bool isAnyMouseButtonDown() const = 0;
        virtual bool isMouseButtonDown( EMouseButtonCode _button ) const = 0;

    public:
        virtual uint32_t getControllerCount() const = 0;
        virtual ControllerId getControllerId( uint32_t _index ) const = 0;
        virtual bool isControllerConnected( ControllerId _controllerId ) const = 0;
        virtual bool isControllerButtonDown( ControllerId _controllerId, EControllerButton _button ) const = 0;
        virtual float getControllerAxisValue( ControllerId _controllerId, EControllerAxis _axis ) const = 0;
        virtual bool getControllerState( ControllerId _controllerId, InputControllerState * const _state ) const = 0;

        virtual void setControllerDeadZone( EControllerAxis _axis, float _deadZone ) = 0;
        virtual float getControllerDeadZone( EControllerAxis _axis ) const = 0;

        virtual void addControllerHandler( const InputControllerHandlerInterfacePtr & _handler, const DocumentInterfacePtr & _doc ) = 0;
        virtual void removeControllerHandler( const InputControllerHandlerInterfacePtr & _handler ) = 0;

    public:
        virtual void setCursorPosition( ETouchCode _touchId, const mt::vec2f & _point, float _pressure ) = 0;
        virtual const mt::vec2f & getCursorPosition( ETouchCode _touchId ) const = 0;
        virtual float getCursorPressure( ETouchCode _touchId ) const = 0;
        virtual bool validCursorPosition( float _x, float _y, float * const _vx, float * const _vy ) const = 0;

    public:
        virtual void addMousePositionProvider( const InputMousePositionProviderInterfacePtr & _provider, const DocumentInterfacePtr & _doc ) = 0;
        virtual void removeMousePositionProvider( const InputMousePositionProviderInterfacePtr & _provider ) = 0;

    public:
        virtual void onFocus( bool _focus ) = 0;

    public:
        virtual void getSpecial( InputSpecialData * const _special ) const = 0;

    public:
        typedef Variant<
            InputKeyEvent,
            InputTextEvent,
            InputAccelerometerEvent,
            InputMouseButtonEvent,
            InputMouseWheelEvent,
            InputMouseMoveEvent,
            InputMouseEnterEvent,
            InputMouseLeaveEvent,
            InputControllerConnectEvent,
            InputControllerButtonEvent,
            InputControllerAxisEvent> InputVariantEvent;

        virtual void pushEvent( const InputVariantEvent & _event ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define INPUT_SERVICE()\
    ((Mengine::InputServiceInterface *)SERVICE_GET(Mengine::InputServiceInterface))
//////////////////////////////////////////////////////////////////////////
