#include "TouchGestureRecognizer.h"

#include "Kernel/Assertion.h"
#include "Kernel/InputServiceHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static const Timestamp TOUCH_TAP_MAX_DURATION = MENGINE_UINT64_C( 250 );
    static const Timestamp TOUCH_DOUBLE_TAP_MAX_INTERVAL = MENGINE_UINT64_C( 350 );
    static const float TOUCH_TAP_MAX_DISTANCE = 0.015f;
    static const float TOUCH_DOUBLE_TAP_MAX_DISTANCE = 0.025f;
    //////////////////////////////////////////////////////////////////////////
    TouchGestureRecognizer::TouchGestureRecognizer()
        : m_touches{}
        , m_touchCount( 0 )
        , m_startScreen( 0.f, 0.f )
        , m_startTimestamp( 0 )
        , m_singleTouch( false )
        , m_tapCandidate( false )
        , m_lastTapScreen( 0.f, 0.f )
        , m_lastTapTimestamp( 0 )
        , m_hasLastTap( false )
        , m_pinchTouch0( TC_TOUCH_INVALID )
        , m_pinchTouch1( TC_TOUCH_INVALID )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TouchGestureRecognizer::handleButton( const InputMouseButtonEvent & _event, const Resolution & _resolution, Event * const _gesture )
    {
        TouchDesc & touch = m_touches[_event.touchId];

        MENGINE_ASSERTION_FATAL( touch.active != _event.isDown, "repeated touch button event [%u]"
            , _event.touchId
        );

        touch.active = _event.isDown;
        touch.screen = _event.position.screen;

        if( _event.isDown == true )
        {
            ++m_touchCount;

            if( m_touchCount == 1 )
            {
                m_startScreen = touch.screen;
                m_startTimestamp = _event.base.timestamp;
                m_singleTouch = true;
                m_tapCandidate = true;

                return false;
            }

            m_singleTouch = false;
            m_hasLastTap = false;

            if( m_touchCount == 2 )
            {
                if( this->beginPinch_( _event.base, _event.special, _resolution, _gesture ) == false )
                {
                    return false;
                }

                return true;
            }

            if( this->endPinch_( _event.base, _event.special, IGP_CANCEL, _gesture ) == false )
            {
                return false;
            }

            return true;
        }

        --m_touchCount;

        if( _event.isCancelled == true )
        {
            m_singleTouch = false;
            this->resetTap();

            if( this->endPinch_( _event.base, _event.special, IGP_CANCEL, _gesture ) == false )
            {
                return false;
            }

            return true;
        }

        if( this->endPinch_( _event.base, _event.special, IGP_END, _gesture ) == true )
        {
            return true;
        }

        if( m_touchCount == 2 )
        {
            if( this->beginPinch_( _event.base, _event.special, _resolution, _gesture ) == false )
            {
                return false;
            }

            return true;
        }

        if( m_singleTouch == false )
        {
            return false;
        }

        m_singleTouch = false;

        if( this->finishSingleTouch_( _event, _resolution, _gesture ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TouchGestureRecognizer::handleMove( const InputMouseMoveEvent & _event, const Resolution & _resolution )
    {
        TouchDesc & touch = m_touches[_event.touchId];

        if( touch.active == false )
        {
            return;
        }

        touch.screen = _event.position.screen;

        if( m_singleTouch == false )
        {
            return;
        }

        mt::vec2f delta = Helper::calcTouchDelta( m_startScreen, touch.screen, _resolution );
        float distanceSquared = mt::dot_v2_v2( delta, delta );

        if( distanceSquared > TOUCH_TAP_MAX_DISTANCE * TOUCH_TAP_MAX_DISTANCE )
        {
            m_tapCandidate = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool TouchGestureRecognizer::finishMove( const InputMouseMoveEndEvent & _event, const Resolution & _resolution, Event * const _gesture )
    {
        if( m_touchCount != 2 )
        {
            return false;
        }

        if( m_pinch.isActive() == false )
        {
            if( this->beginPinch_( _event.base, _event.special, _resolution, _gesture ) == false )
            {
                return false;
            }

            return true;
        }

        const TouchDesc & touch0 = m_touches[m_pinchTouch0];
        const TouchDesc & touch1 = m_touches[m_pinchTouch1];

        float scale;
        EInputGesturePhase phase = m_pinch.update( touch0.screen, touch1.screen, _resolution, &scale );

        if( phase == IGP_CANCEL )
        {
            this->makePinchEvent_( _event.base, _event.special, 1.f, IGP_CANCEL, _gesture );

            return true;
        }

        if( phase != IGP_CHANGE )
        {
            return false;
        }

        this->makePinchEvent_( _event.base, _event.special, scale, IGP_CHANGE, _gesture );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TouchGestureRecognizer::cancel( const InputPointerCancelEvent & _event, Event * const _gesture )
    {
        for( TouchDesc & touch : m_touches )
        {
            touch.active = false;
        }

        m_touchCount = 0;
        m_singleTouch = false;
        this->resetTap();

        if( this->endPinch_( _event.base, _event.special, IGP_CANCEL, _gesture ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TouchGestureRecognizer::resetTap()
    {
        m_tapCandidate = false;
        m_hasLastTap = false;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f * TouchGestureRecognizer::getTouchPosition( ETouchCode _touchId ) const
    {
        const TouchDesc & touch = m_touches[_touchId];

        if( touch.active == false )
        {
            return nullptr;
        }

        return &touch.screen;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TouchGestureRecognizer::finishSingleTouch_( const InputMouseButtonEvent & _event, const Resolution & _resolution, Event * const _gesture )
    {
        bool hasLastTap = m_hasLastTap;
        m_hasLastTap = false;

        Timestamp timestamp = _event.base.timestamp;

        if( timestamp < m_startTimestamp )
        {
            return false;
        }

        if( m_tapCandidate == false )
        {
            return false;
        }

        Timestamp duration = timestamp - m_startTimestamp;

        if( duration > TOUCH_TAP_MAX_DURATION )
        {
            return false;
        }

        const mt::vec2f & screen = _event.position.screen;
        mt::vec2f delta = Helper::calcTouchDelta( m_startScreen, screen, _resolution );
        float distanceSquared = mt::dot_v2_v2( delta, delta );

        if( distanceSquared > TOUCH_TAP_MAX_DISTANCE * TOUCH_TAP_MAX_DISTANCE )
        {
            return false;
        }

        Timestamp lastTapTimestamp = m_lastTapTimestamp;
        mt::vec2f lastTapScreen = m_lastTapScreen;

        m_hasLastTap = true;
        m_lastTapScreen = screen;
        m_lastTapTimestamp = timestamp;

        if( hasLastTap == false )
        {
            return false;
        }

        if( timestamp < lastTapTimestamp )
        {
            return false;
        }

        Timestamp interval = timestamp - lastTapTimestamp;

        if( interval > TOUCH_DOUBLE_TAP_MAX_INTERVAL )
        {
            return false;
        }

        mt::vec2f tapDelta = Helper::calcTouchDelta( lastTapScreen, screen, _resolution );
        float tapDistanceSquared = mt::dot_v2_v2( tapDelta, tapDelta );
        bool withinTapDistance = tapDistanceSquared <= TOUCH_DOUBLE_TAP_MAX_DISTANCE * TOUCH_DOUBLE_TAP_MAX_DISTANCE;

        if( withinTapDistance == false )
        {
            return false;
        }

        m_hasLastTap = false;

        InputGestureDoubleTapEvent tap;
        tap.base = _event.base;
        tap.special = _event.special;
        tap.position = _event.position;
        tap.touchId = _event.touchId;
        tap.isTouch = true;

        *_gesture = tap;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TouchGestureRecognizer::beginPinch_( const InputBaseData & _base, const InputSpecialData & _special, const Resolution & _resolution, Event * const _gesture )
    {
        ETouchCode first = TC_TOUCH_INVALID;

        for( uint32_t index = 0; index != MENGINE_INPUT_MAX_TOUCH; ++index )
        {
            const TouchDesc & touch = m_touches[index];

            if( touch.active == false )
            {
                continue;
            }

            if( first == TC_TOUCH_INVALID )
            {
                first = static_cast<ETouchCode>(index);

                continue;
            }

            const TouchDesc & firstTouch = m_touches[first];

            if( m_pinch.begin( firstTouch.screen, touch.screen, _resolution ) == false )
            {
                return false;
            }

            m_pinchTouch0 = first;
            m_pinchTouch1 = static_cast<ETouchCode>(index);

            this->makePinchEvent_( _base, _special, 1.f, IGP_BEGIN, _gesture );

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TouchGestureRecognizer::endPinch_( const InputBaseData & _base, const InputSpecialData & _special, EInputGesturePhase _phase, Event * const _gesture )
    {
        if( m_pinch.isActive() == false )
        {
            return false;
        }

        this->makePinchEvent_( _base, _special, 1.f, _phase, _gesture );

        m_pinch.reset();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TouchGestureRecognizer::makePinchEvent_( const InputBaseData & _base, const InputSpecialData & _special, float _scale, EInputGesturePhase _phase, Event * const _gesture ) const
    {
        const TouchDesc & touch0 = m_touches[m_pinchTouch0];
        const TouchDesc & touch1 = m_touches[m_pinchTouch1];
        mt::vec2f screen = (touch0.screen + touch1.screen) * 0.5f;

        InputGestureMagnificationEvent pinch;
        pinch.base = _base;
        pinch.special = _special;
        pinch.position.screen = screen;
        pinch.position.world = mt::vec2f( 0.f, 0.f );
        pinch.scale = _scale;
        pinch.phase = _phase;
        pinch.isTouch = true;
        pinch.touchId0 = m_pinchTouch0;
        pinch.touchId1 = m_pinchTouch1;

        *_gesture = pinch;
    }
    //////////////////////////////////////////////////////////////////////////
}
