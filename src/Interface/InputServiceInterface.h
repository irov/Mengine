#pragma once

#include "Interface/Interface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/InputHandlerInterface.h"

#include "Config/Typedef.h"

#include "math/vec2.h"

#ifndef MENGINE_INPUT_MAX_TOUCH
#define MENGINE_INPUT_MAX_TOUCH 16
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    union InputUnionEvent
    {
        InputEventType type;

        InputKeyEvent key;
        InputTextEvent text;
        InputMouseButtonEvent button;
        InputMouseWheelEvent wheel;
        InputMouseMoveEvent move;
        InputMousePositionEvent position;
    };
    //////////////////////////////////////////////////////////////////////////
    class InputMousePositionProviderInterface
        : public Interface
    {
    public:
        virtual void onMousePositionChange( uint32_t _touchId, const mt::vec2f & _position ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<InputMousePositionProviderInterface> InputMousePositionProviderInterfacePtr;
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
        virtual bool isKeyDown( uint32_t _keyCode ) const = 0;
        virtual bool isExclusiveKeyDown( uint32_t _keyCode ) const = 0;
        virtual bool isAnyKeyDown() const = 0;

    public:
        virtual bool isAnyMouseButtonDown() const = 0;
        virtual bool isMouseButtonDown( uint32_t _buttonId ) const = 0;

        virtual void setCursorPosition( uint32_t _touchId, const mt::vec2f & _point ) = 0;
        virtual const mt::vec2f & getCursorPosition( uint32_t _touchId ) const = 0;
        virtual bool validCursorPosition( float _x, float _y, float * _vx, float * _vy ) const = 0;

        virtual uint32_t addMousePositionProvider( const InputMousePositionProviderInterfacePtr & _provider ) = 0;
        virtual void removeMousePositionProvider( uint32_t _id ) = 0;

    public:
        virtual void onFocus( bool _focus ) = 0;

    public:
        virtual void pushEvent( const InputUnionEvent & _event ) = 0;

    public:
        MENGINE_INLINE void pushMouseMoveEvent( uint32_t _touchId, float _x, float _y, float _dx, float _dy, float _pressure )
        {
            float vx;
            float vy;
            if( this->validCursorPosition( _x, _y, &vx, &vy ) == false )
            {
                return;
            }

            InputUnionEvent ev;
            ev.move.type = IET_MOUSE_MOVE;

            ev.move.touchId = _touchId;
            ev.move.x = vx;
            ev.move.y = vy;
            ev.move.dx = _dx;
            ev.move.dy = _dy;
            ev.move.pressure = _pressure;

            this->pushEvent( ev );
        }

        MENGINE_INLINE void pushMouseButtonEvent( uint32_t _touchId, float _x, float _y, uint32_t _button, float _pressure, bool _isDown )
        {
            float vx;
            float vy;
            if( this->validCursorPosition( _x, _y, &vx, &vy ) == false )
            {
                return;
            }

            InputUnionEvent ev;
            ev.button.type = IET_MOUSE_BUTTON;

            ev.button.touchId = _touchId;
            ev.button.x = vx;
            ev.button.y = vy;
            ev.button.button = _button;
            ev.button.pressure = _pressure;
            ev.button.isDown = _isDown;
            ev.button.isPressed = true;

            this->pushEvent( ev );
        }

        MENGINE_INLINE void pushKeyEvent( float _x, float _y, KeyCode _code, bool _isDown, bool _repeating )
        {
            float vx;
            float vy;
            this->validCursorPosition( _x, _y, &vx, &vy );

            InputUnionEvent ev;
            ev.key.type = IET_KEY;

            ev.key.x = vx;
            ev.key.y = vy;
            ev.key.code = _code;
            ev.key.isDown = _isDown;
            ev.key.isRepeat = _repeating;

            this->pushEvent( ev );
        }

        MENGINE_INLINE void pushTextEvent( float _x, float _y, WChar _key )
        {
            float vx;
            float vy;
            this->validCursorPosition( _x, _y, &vx, &vy );

            InputUnionEvent ev;
            ev.text.type = IET_TEXT;

            ev.text.x = vx;
            ev.text.y = vy;
            ev.text.key = _key;

            this->pushEvent( ev );
        }

        MENGINE_INLINE void pushMouseLeaveEvent( uint32_t _touchId, float _x, float _y, float _pressure )
        {
            float vx;
            float vy;
            this->validCursorPosition( _x, _y, &vx, &vy );

            InputUnionEvent ev;
            ev.position.type = IET_MOUSE_LEAVE;

            ev.position.touchId = _touchId;
            ev.position.x = vx;
            ev.position.y = vy;
            ev.position.pressure = _pressure;

            this->pushEvent( ev );
        }

        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void pushMouseEnterEvent( uint32_t _touchId, float _x, float _y, float _pressure )
        {
            float vx;
            float vy;
            this->validCursorPosition( _x, _y, &vx, &vy );

            InputUnionEvent ev;
            ev.position.type = IET_MOUSE_ENTER;

            ev.position.touchId = _touchId;
            ev.position.x = vx;
            ev.position.y = vy;
            ev.position.pressure = _pressure;

            this->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void pushMouseWheelEvent( float _x, float _y, uint32_t _button, int32_t _whell )
        {
            float vx;
            float vy;
            this->validCursorPosition( _x, _y, &vx, &vy );

            InputUnionEvent ev;
            ev.wheel.type = IET_MOUSE_WHELL;

            ev.wheel.x = vx;
            ev.wheel.y = vy;
            ev.wheel.button = _button;
            ev.wheel.wheel = _whell;

            this->pushEvent( ev );
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE void pushMousePositionEvent( uint32_t _touchId, float _x, float _y, float _pressure )
        {
            float vx;
            float vy;
            if( this->validCursorPosition( _x, _y, &vx, &vy ) == false )
            {
                return;
            }

            InputUnionEvent ev;
            ev.position.type = IET_MOUSE_POSITION;

            ev.position.touchId = _touchId;
            ev.position.x = vx;
            ev.position.y = vy;
            ev.position.pressure = _pressure;

            this->pushEvent( ev );
        }
    };
}
//////////////////////////////////////////////////////////////////////////
#define INPUT_SERVICE()\
    ((Mengine::InputServiceInterface*)SERVICE_GET(Mengine::InputServiceInterface))
//////////////////////////////////////////////////////////////////////////
