#pragma once

#include "Kernel/InputEvent.h"
#include "Kernel/Resolution.h"

namespace Mengine
{
    class TouchGestureRecognizer
    {
    public:
        TouchGestureRecognizer();
        ~TouchGestureRecognizer();

    public:
        bool handleButton( const InputMouseButtonEvent & _event, const Resolution & _resolution, InputGestureEvent * const _gesture );
        void handleMove( const InputMouseMoveEvent & _event, const Resolution & _resolution );
        bool finishMove( const InputTouchMoveEndEvent & _event, const Resolution & _resolution, InputGestureEvent * const _gesture );
        void resetTap();
        const mt::vec2f & getTouchPosition( ETouchCode _touchId ) const;

    protected:
        bool finishTap_( const InputMouseButtonEvent & _event, const Resolution & _resolution, InputGestureEvent * const _gesture );
        bool beginPinch_( const InputBaseData & _base, const InputSpecialData & _special, const Resolution & _resolution, InputGestureEvent * const _gesture );
        bool endPinch_( const InputBaseData & _base, const InputSpecialData & _special, EInputGesturePhase _phase, InputGestureEvent * const _gesture );
        void makePinchEvent_( const InputBaseData & _base, const InputSpecialData & _special, float _scale, EInputGesturePhase _phase, InputGestureEvent * const _gesture ) const;

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
        bool m_tapCandidate;

        mt::vec2f m_lastTapScreen;
        Timestamp m_lastTapTimestamp;
        bool m_hasLastTap;

        ETouchCode m_pinchTouch0;
        ETouchCode m_pinchTouch1;
        float m_pinchDistance;
    };
}
