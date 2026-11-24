#pragma once

#include "Kernel/InputEvent.h"
#include "Kernel/Resolution.h"
#include "Kernel/TouchPinchRecognizer.h"

#include "Config/Variant.h"

namespace Mengine
{
    class TouchGestureRecognizer
    {
    public:
        typedef Variant<InputGestureMagnificationEvent, InputGestureDoubleTapEvent> Event;

    public:
        TouchGestureRecognizer();

    public:
        bool handleButton( const InputMouseButtonEvent & _event, const Resolution & _resolution, Event * const _gesture );
        void handleMove( const InputMouseMoveEvent & _event, const Resolution & _resolution );
        bool finishMove( const InputMouseMoveEndEvent & _event, const Resolution & _resolution, Event * const _gesture );
        bool cancel( const InputPointerCancelEvent & _event, Event * const _gesture );
        void resetTap();
        const mt::vec2f * getTouchPosition( ETouchCode _touchId ) const;

    protected:
        bool finishSingleTouch_( const InputMouseButtonEvent & _event, const Resolution & _resolution, Event * const _gesture );
        bool beginPinch_( const InputBaseData & _base, const InputSpecialData & _special, const Resolution & _resolution, Event * const _gesture );
        bool endPinch_( const InputBaseData & _base, const InputSpecialData & _special, EInputGesturePhase _phase, Event * const _gesture );
        void makePinchEvent_( const InputBaseData & _base, const InputSpecialData & _special, float _scale, EInputGesturePhase _phase, Event * const _gesture ) const;

    protected:
        struct TouchDesc
        {
            mt::vec2f screen;
            bool active;
        };

        TouchDesc m_touches[MENGINE_INPUT_MAX_TOUCH];
        uint32_t m_touchCount;

        mt::vec2f m_startScreen;
        Timestamp m_startTimestamp;
        bool m_singleTouch;
        bool m_tapCandidate;

        mt::vec2f m_lastTapScreen;
        Timestamp m_lastTapTimestamp;
        bool m_hasLastTap;

        ETouchCode m_pinchTouch0;
        ETouchCode m_pinchTouch1;
        TouchPinchRecognizer m_pinch;
    };
}
