#include "TouchPinchRecognizer.h"

#include "Assertion.h"
#include "InputServiceHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static const float TOUCH_PINCH_MIN_DISTANCE = 0.001f;
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static float calcTouchDistance( const mt::vec2f & _first, const mt::vec2f & _second, const Resolution & _resolution )
        {
            mt::vec2f delta = Helper::calcTouchDelta( _first, _second, _resolution );
            float distance = mt::length_v2( delta );

            return distance;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    TouchPinchRecognizer::TouchPinchRecognizer()
        : m_distance( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TouchPinchRecognizer::begin( const mt::vec2f & _first, const mt::vec2f & _second, const Resolution & _resolution )
    {
        MENGINE_ASSERTION_FATAL( this->isActive() == false, "touch pinch is already active" );

        float distance = Detail::calcTouchDistance( _first, _second, _resolution );

        if( distance < TOUCH_PINCH_MIN_DISTANCE )
        {
            return false;
        }

        m_distance = distance;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TouchPinchRecognizer::isActive() const
    {
        return m_distance != 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    EInputGesturePhase TouchPinchRecognizer::update( const mt::vec2f & _first, const mt::vec2f & _second, const Resolution & _resolution, float * const _scale )
    {
        MENGINE_ASSERTION_FATAL( this->isActive() == true, "touch pinch is inactive" );

        float distance = Detail::calcTouchDistance( _first, _second, _resolution );

        if( distance == m_distance )
        {
            return IGP_NONE;
        }

        if( distance < TOUCH_PINCH_MIN_DISTANCE )
        {
            this->reset();

            return IGP_CANCEL;
        }

        *_scale = distance / m_distance;
        m_distance = distance;

        return IGP_CHANGE;
    }
    //////////////////////////////////////////////////////////////////////////
    void TouchPinchRecognizer::reset()
    {
        m_distance = 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
}
