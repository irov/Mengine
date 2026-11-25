#include "TouchGestureRecognizer.h"

#include "Kernel/Assertion.h"

#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static const Timestamp TOUCH_TAP_MAX_DURATION = MENGINE_UINT64_C( 250 );
    static const Timestamp TOUCH_DOUBLE_TAP_MAX_INTERVAL = MENGINE_UINT64_C( 350 );
    static const float TOUCH_TAP_MAX_DISTANCE = 0.015f;
    static const float TOUCH_DOUBLE_TAP_MAX_DISTANCE = 0.025f;
    static const float TOUCH_PINCH_MIN_DISTANCE = 0.001f;
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static mt::vec2f calcTouchDelta( const mt::vec2f & _from, const mt::vec2f & _to, const Resolution & _resolution )
        {
            mt::vec2f delta = _to - _from;

            float width = _resolution.getWidthF();
            float height = _resolution.getHeightF();
            float shortSide = StdAlgorithm::min( width, height );

            delta.x *= width / shortSide;
            delta.y *= height / shortSide;

            return delta;
        }
        //////////////////////////////////////////////////////////////////////////
        static float calcTouchDistanceSquared( const mt::vec2f & _from, const mt::vec2f & _to, const Resolution & _resolution )
        {
            mt::vec2f delta = Detail::calcTouchDelta( _from, _to, _resolution );
            float distanceSquared = mt::dot_v2_v2( delta, delta );

            return distanceSquared;
        }
        //////////////////////////////////////////////////////////////////////////
        static float calcTouchDistance( const mt::vec2f & _from, const mt::vec2f & _to, const Resolution & _resolution )
        {
            mt::vec2f delta = Detail::calcTouchDelta( _from, _to, _resolution );
            float distance = mt::length_v2( delta );

            return distance;
        }
        //////////////////////////////////////////////////////////////////////////
        static void fillTouchGestureEvent( const InputBaseData & _base, const InputSpecialData & _special, EInputGestureType _type, const mt::vec2f & _screen, EInputGesturePhase _phase, ETouchCode _touchId0, ETouchCode _touchId1, InputGestureEvent * const _event )
        {
            _event->base = _base;
            _event->special = _special;
            _event->position.screen = _screen;
            _event->position.world = mt::vec2f( 0.f, 0.f );
            _event->type = _type;
            _event->phase = _phase;
            _event->isTouch = true;
            _event->touchId0 = _touchId0;
            _event->touchId1 = _touchId1;
            _event->scale = 1.f;
            _event->degrees = 0.f;
            _event->direction = mt::vec2f( 0.f, 0.f );
            _event->pressure = 0.f;
            _event->stage = 0;
            _event->stageTransition = 0.f;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    TouchGestureRecognizer::TouchGestureRecognizer()
        : m_touchCount( 0 )
        , m_startScreen( 0.f, 0.f )
        , m_startTimestamp( 0 )
        , m_tapCandidate( false )
        , m_lastTapScreen( 0.f, 0.f )
        , m_lastTapTimestamp( 0 )
        , m_hasLastTap( false )
        , m_pinchTouch0( TC_TOUCH_INVALID )
        , m_pinchTouch1( TC_TOUCH_INVALID )
        , m_pinchDistance( 0.f )
    {
        for( TouchDesc & touch : m_touches )
        {
            touch.screen = mt::vec2f( 0.f, 0.f );
            touch.active = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    TouchGestureRecognizer::~TouchGestureRecognizer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TouchGestureRecognizer::handleButton( const InputMouseButtonEvent & _event, const Resolution & _resolution, InputGestureEvent * const _gesture )
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
                m_tapCandidate = true;

                return false;
            }

            this->resetTap();

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

        if( m_tapCandidate == false )
        {
            return false;
        }

        m_tapCandidate = false;

        if( this->finishTap_( _event, _resolution, _gesture ) == false )
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

        if( m_tapCandidate == false )
        {
            return;
        }

        float distanceSquared = Detail::calcTouchDistanceSquared( m_startScreen, touch.screen, _resolution );

        if( distanceSquared > TOUCH_TAP_MAX_DISTANCE * TOUCH_TAP_MAX_DISTANCE )
        {
            this->resetTap();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool TouchGestureRecognizer::finishMove( const InputTouchMoveEndEvent & _event, const Resolution & _resolution, InputGestureEvent * const _gesture )
    {
        if( m_touchCount != 2 )
        {
            return false;
        }

        if( m_pinchDistance == 0.f )
        {
            if( this->beginPinch_( _event.base, _event.special, _resolution, _gesture ) == false )
            {
                return false;
            }

            return true;
        }

        const TouchDesc & touch0 = m_touches[m_pinchTouch0];
        const TouchDesc & touch1 = m_touches[m_pinchTouch1];

        float distance = Detail::calcTouchDistance( touch0.screen, touch1.screen, _resolution );

        if( distance == m_pinchDistance )
        {
            return false;
        }

        if( distance < TOUCH_PINCH_MIN_DISTANCE )
        {
            this->endPinch_( _event.base, _event.special, IGP_CANCEL, _gesture );

            return true;
        }

        float scale = distance / m_pinchDistance;
        m_pinchDistance = distance;

        this->makePinchEvent_( _event.base, _event.special, scale, IGP_CHANGE, _gesture );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TouchGestureRecognizer::resetTap()
    {
        m_tapCandidate = false;
        m_hasLastTap = false;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & TouchGestureRecognizer::getTouchPosition( ETouchCode _touchId ) const
    {
        const TouchDesc & touch = m_touches[_touchId];

        MENGINE_ASSERTION_FATAL( touch.active == true, "touch [%u] is not active"
            , _touchId
        );

        return touch.screen;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TouchGestureRecognizer::finishTap_( const InputMouseButtonEvent & _event, const Resolution & _resolution, InputGestureEvent * const _gesture )
    {
        bool hasLastTap = m_hasLastTap;
        m_hasLastTap = false;

        Timestamp timestamp = _event.base.timestamp;

        if( timestamp < m_startTimestamp )
        {
            return false;
        }

        Timestamp duration = timestamp - m_startTimestamp;

        if( duration > TOUCH_TAP_MAX_DURATION )
        {
            return false;
        }

        const mt::vec2f & screen = _event.position.screen;
        float distanceSquared = Detail::calcTouchDistanceSquared( m_startScreen, screen, _resolution );

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

        float tapDistanceSquared = Detail::calcTouchDistanceSquared( lastTapScreen, screen, _resolution );

        if( tapDistanceSquared > TOUCH_DOUBLE_TAP_MAX_DISTANCE * TOUCH_DOUBLE_TAP_MAX_DISTANCE )
        {
            return false;
        }

        m_hasLastTap = false;

        Detail::fillTouchGestureEvent( _event.base, _event.special, IGT_DOUBLE_TAP, screen, IGP_NONE, _event.touchId, TC_TOUCH_INVALID, _gesture );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TouchGestureRecognizer::beginPinch_( const InputBaseData & _base, const InputSpecialData & _special, const Resolution & _resolution, InputGestureEvent * const _gesture )
    {
        MENGINE_ASSERTION_FATAL( m_pinchDistance == 0.f, "touch pinch is already active" );

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

            float distance = Detail::calcTouchDistance( firstTouch.screen, touch.screen, _resolution );

            if( distance < TOUCH_PINCH_MIN_DISTANCE )
            {
                return false;
            }

            m_pinchTouch0 = first;
            m_pinchTouch1 = static_cast<ETouchCode>(index);
            m_pinchDistance = distance;

            this->makePinchEvent_( _base, _special, 1.f, IGP_BEGIN, _gesture );

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TouchGestureRecognizer::endPinch_( const InputBaseData & _base, const InputSpecialData & _special, EInputGesturePhase _phase, InputGestureEvent * const _gesture )
    {
        if( m_pinchDistance == 0.f )
        {
            return false;
        }

        this->makePinchEvent_( _base, _special, 1.f, _phase, _gesture );

        m_pinchDistance = 0.f;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TouchGestureRecognizer::makePinchEvent_( const InputBaseData & _base, const InputSpecialData & _special, float _scale, EInputGesturePhase _phase, InputGestureEvent * const _gesture ) const
    {
        const TouchDesc & touch0 = m_touches[m_pinchTouch0];
        const TouchDesc & touch1 = m_touches[m_pinchTouch1];
        mt::vec2f screen = (touch0.screen + touch1.screen) * 0.5f;

        Detail::fillTouchGestureEvent( _base, _special, IGT_MAGNIFICATION, screen, _phase, m_pinchTouch0, m_pinchTouch1, _gesture );

        _gesture->scale = _scale;
    }
    //////////////////////////////////////////////////////////////////////////
}
