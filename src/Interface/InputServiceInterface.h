#pragma once

#include "Interface/Interface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/InputHandlerInterface.h"
#include "Interface/InputMousePositionProviderInterface.h"

#include "Kernel/KeyCode.h"
#include "Kernel/MouseButtonCode.h"
#include "Kernel/TouchCode.h"

#include "Config/Typedef.h"
#include "Config/UniqueId.h"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct InputUnionEvent
    {
        EInputEventType type;

        union
        {
            InputSpecialData special;

            InputKeyEvent key;
            InputTextEvent text;
            InputMouseButtonEvent button;
            InputMouseWheelEvent wheel;
            InputMouseMoveEvent move;
            InputMouseEnterEvent enter;
            InputMouseLeaveEvent leave;
        } data;
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
        virtual bool isMouseButtonDown( EMouseButtonCode _button ) const = 0;

    public:
        virtual void setCursorPosition( ETouchCode _touchId, const mt::vec2f & _point, float _pressure ) = 0;
        virtual const mt::vec2f & getCursorPosition( ETouchCode _touchId ) const = 0;
        virtual float getCursorPressure( ETouchCode _touchId ) const = 0;
        virtual bool validCursorPosition( float _x, float _y, float * const _vx, float * const _vy ) const = 0;

    public:
        virtual UniqueId addMousePositionProvider( const InputMousePositionProviderInterfacePtr & _provider, const DocumentInterfacePtr & _doc ) = 0;
        virtual void removeMousePositionProvider( UniqueId _id ) = 0;

    public:
        virtual void onFocus( bool _focus ) = 0;

    public:
        virtual void getSpecial( InputSpecialData * const _special ) const = 0;

    public:
        virtual void pushEvent( const InputUnionEvent & _event ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define INPUT_SERVICE()\
    ((Mengine::InputServiceInterface*)SERVICE_GET(Mengine::InputServiceInterface))
//////////////////////////////////////////////////////////////////////////
