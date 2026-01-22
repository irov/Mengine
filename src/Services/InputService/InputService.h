#pragma once

#include "Interface/InputServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Resolution.h"
#include "Kernel/Viewport.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class InputService
        : public ServiceBase<InputServiceInterface>
    {
        DECLARE_FACTORABLE( InputService );
        DECLARE_LIFECYCLEABLE();

    public:
        InputService();
        ~InputService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void _update() override;

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
        bool isMouseButtonDown( EMouseButtonCode _button ) const override;

    public:
        void setCursorPosition( ETouchCode _touchId, const mt::vec2f & _screenPosition, float _pressure ) override;
        const mt::vec2f & getCursorPosition( ETouchCode _touchId ) const override;
        float getCursorPressure( ETouchCode _touchId ) const override;
        bool validCursorPosition( float _x, float _y, float * const _vx, float * const _vy ) const override;

    public:
        void addMousePositionProvider( const InputMousePositionProviderInterfacePtr & _provider, const DocumentInterfacePtr & _doc ) override;
        void removeMousePositionProvider( const InputMousePositionProviderInterfacePtr & _provider ) override;

    public:
        void onFocus( bool _focus ) override;

    public:
        void pushEvent( const InputVariantEvent & _event ) override;

    protected:
        void getSpecial( InputSpecialData * const _special ) const override;

    protected:
        void dispatchEvent_( const InputKeyEvent & _params );
        void dispatchEvent_( const InputTextEvent & _params );
        void dispatchEvent_( const InputAccelerometerEvent & _params );
        void dispatchEvent_( const InputMouseButtonEvent & _params );
        void dispatchEvent_( const InputMouseMoveEvent & _params );
        void dispatchEvent_( const InputMouseWheelEvent & _params );
        void dispatchEvent_( const InputMouseEnterEvent & _params );
        void dispatchEvent_( const InputMouseLeaveEvent & _params );

    protected:
        void applyCursorPosition_( ETouchCode _touchId, const mt::vec2f & _screenPosition, float _pressure );

    protected:
        mt::vec2f m_cursorPosition[MENGINE_INPUT_MAX_TOUCH];
        float m_cursorPressure[MENGINE_INPUT_MAX_TOUCH];

        ThreadMutexInterfacePtr m_mutex;

        struct InputMousePositionProviderDesc
        {
            InputMousePositionProviderInterfacePtr provider;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
#endif
        };

        typedef Vector<InputMousePositionProviderDesc> VectorMousePositionProviders;
        VectorMousePositionProviders m_mousePositionProviders;

        typedef Vector<InputVariantEvent> VectorInputEvents;
        VectorInputEvents m_events;
        VectorInputEvents m_eventsAux;

        bool m_keyBuffer[MENGINE_INPUT_MAX_KEY_CODE];
        bool m_mouseBuffer[MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE];
    };
}
