#pragma once

#include "Interface/InputServiceInterface.h"

#include "Kernel/ServiceBase.h"

#include "Kernel/Resolution.h"
#include "Kernel/Viewport.h"
#include "Kernel/Vector.h"

#include "math/vec2.h"

namespace Mengine
{
    class InputService
        : public ServiceBase<InputServiceInterface>
    {
    public:
        InputService();
        ~InputService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

    public:
        void update() override;

    public:
        bool isSpecialDown() const override;
        bool isAltDown() const override;
        bool isShiftDown() const override;
        bool isControlDown() const override;

    public:
        bool isKeyDown( EKeyCode _keyCode ) const override;
        bool isExclusiveKeyDown( EKeyCode _keyCode ) const override;
        bool isAnyKeyDown() const override;

        bool isAnyMouseButtonDown() const override;
        bool isMouseButtonDown( EMouseCode _button ) const override;

    public:
        void setCursorPosition( uint32_t _touchId, const mt::vec2f & _point, float _pressure ) override;
        const mt::vec2f & getCursorPosition( uint32_t _touchId ) const override;
        float getCursorPressure( uint32_t _touchId ) const override;
        bool validCursorPosition( float _x, float _y, float * const _vx, float * const _vy ) const override;

    public:
        UniqueId addMousePositionProvider( const InputMousePositionProviderInterfacePtr & _provider, const DocumentPtr & _doc ) override;
        void removeMousePositionProvider( UniqueId _id ) override;

    public:
        void onFocus( bool _focus ) override;

    public:
        void pushEvent( const InputUnionEvent & _event ) override;

    protected:
        void keyEvent_( const InputKeyEvent & _params );
        void textEvent_( const InputTextEvent & _params );
        void mouseButtonEvent_( const InputMouseButtonEvent & _params );
        void mouseMoveEvent_( const InputMouseMoveEvent & _params );
        void mouseWheelEvent_( const InputMouseWheelEvent & _params );
        void mousePositionEvent_( const InputMousePositionEvent & _params );
        void mouseEnterEvent_( const InputMouseEnterEvent & _params );
        void mouseLeaveEvent_( const InputMouseLeaveEvent & _params );

    protected:
        void applyCursorPosition_( uint32_t _touchId, float _x, float _y, float _pressure );

    private:
        mt::vec2f m_cursorPosition[MENGINE_INPUT_MAX_TOUCH] = {{0.f, 0.f}};
        float m_cursorPressure[MENGINE_INPUT_MAX_TOUCH] = {0.f};

        struct InputMousePositionProviderDesc
        {
            UniqueId id;
            InputMousePositionProviderInterfacePtr provider;

#if MENGINE_DOCUMENT_ENABLE
            DocumentPtr doc;
#endif
        };

        typedef Vector<InputMousePositionProviderDesc> VectorMousePositionProviders;
        VectorMousePositionProviders m_mousePositionProviders;

        typedef Vector<InputUnionEvent> VectorInputEvents;
        VectorInputEvents m_eventsAux;
        VectorInputEvents m_events;

        bool m_keyBuffer[256] = {false};
        bool m_mouseBuffer[3] = {false};
        bool m_mouseBufferSpecial[3] = {false};
    };
}
