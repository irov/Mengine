#pragma once

#include "Interface/Interface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/InputHandlerInterface.h"
#include "Interface/InputMousePositionProviderInterface.h"

#include "Kernel/KeyCode.h"
#include "Kernel/MouseCode.h"

#include "Config/Typedef.h"
#include "Config/UniqueId.h"

#include "math/vec2.h"

#ifndef MENGINE_INPUT_MAX_TOUCH
#define MENGINE_INPUT_MAX_TOUCH 16
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    union InputUnionEvent
    {
        EInputEventType type;

        InputKeyEvent key;
        InputTextEvent text;
        InputMouseButtonEvent button;
        InputMouseWheelEvent wheel;
        InputMouseMoveEvent move;
        InputMousePositionEvent position;
        InputMouseEnterEvent enter;
        InputMouseLeaveEvent leave;
    };
    //////////////////////////////////////////////////////////////////////////
    class InputServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "InputService" )

    public:
        virtual void update() = 0;

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
        virtual bool isMouseButtonDown( EMouseCode _buttonId ) const = 0;

    public:
        virtual void setCursorPosition( uint32_t _touchId, const mt::vec2f & _point, float _pressure ) = 0;
        virtual const mt::vec2f & getCursorPosition( uint32_t _touchId ) const = 0;
        virtual float getCursorPressure( uint32_t _touchId ) const = 0;
        virtual bool validCursorPosition( float _x, float _y, float * const _vx, float * const _vy ) const = 0;

    public:
        virtual UniqueId addMousePositionProvider( const InputMousePositionProviderInterfacePtr & _provider, const DocumentPtr & _doc ) = 0;
        virtual void removeMousePositionProvider( UniqueId _id ) = 0;

    public:
        virtual void onFocus( bool _focus ) = 0;

    public:
        virtual void pushEvent( const InputUnionEvent & _event ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define INPUT_SERVICE()\
    ((Mengine::InputServiceInterface*)SERVICE_GET(Mengine::InputServiceInterface))
//////////////////////////////////////////////////////////////////////////
