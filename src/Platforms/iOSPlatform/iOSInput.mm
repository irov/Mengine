#include "iOSInput.h"

#include "Kernel/Logger.h"
#include "Kernel/InputServiceHelper.h"

#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    iOSInput::iOSInput()
    {
        StdAlgorithm::fill_n( m_keyDown, MENGINE_INPUT_MAX_KEY_CODE, false );
        StdAlgorithm::fill_n( m_fingers, MENGINE_INPUT_MAX_TOUCH, (UITouch *)nil );

        m_lastCursorPosition = mt::vec2f( 0.f, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    iOSInput::~iOSInput()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSInput::initialize()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSInput::finalize()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSInput::calcCursorPosition_( UIView * _view, CGPoint _location, mt::vec2f * const _point ) const
    {
        CGSize viewSize = _view.bounds.size;

        if( viewSize.width <= 0.f || viewSize.height <= 0.f )
        {
            _point->x = 0.f;
            _point->y = 0.f;

            return;
        }

        _point->x = static_cast<float>(_location.x / viewSize.width);
        _point->y = static_cast<float>(_location.y / viewSize.height);
    }
    //////////////////////////////////////////////////////////////////////////
    ETouchCode iOSInput::acquireFingerIndex_( UITouch * _touch )
    {
        for( uint32_t index = 0; index != MENGINE_INPUT_MAX_TOUCH; ++index )
        {
            if( m_fingers[index] == nil )
            {
                m_fingers[index] = _touch;

                return static_cast<ETouchCode>(index);
            }
        }

        return TC_TOUCH_INVALID;
    }
    //////////////////////////////////////////////////////////////////////////
    ETouchCode iOSInput::releaseFingerIndex_( UITouch * _touch )
    {
        for( uint32_t index = 0; index != MENGINE_INPUT_MAX_TOUCH; ++index )
        {
            if( m_fingers[index] == _touch )
            {
                m_fingers[index] = nil;

                return static_cast<ETouchCode>(index);
            }
        }

        return TC_TOUCH_INVALID;
    }
    //////////////////////////////////////////////////////////////////////////
    ETouchCode iOSInput::getFingerIndex_( UITouch * _touch ) const
    {
        for( uint32_t index = 0; index != MENGINE_INPUT_MAX_TOUCH; ++index )
        {
            if( m_fingers[index] == _touch )
            {
                return static_cast<ETouchCode>(index);
            }
        }

        return TC_TOUCH_INVALID;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSInput::handleTouchBegan( NSSet<UITouch *> * _touches, UIView * _view )
    {
        for( UITouch * touch in _touches )
        {
            ETouchCode fingerIndex = this->acquireFingerIndex_( touch );

            if( fingerIndex == TC_TOUCH_INVALID )
            {
                LOGGER_INFO( "platform", "touch began: no free touch" );

                continue;
            }

            CGPoint location = [touch locationInView:_view];

            mt::vec2f point;
            this->calcCursorPosition_( _view, location, &point );

            float pressure = static_cast<float>(touch.force / touch.maximumPossibleForce);

            if( touch.maximumPossibleForce <= 0.f )
            {
                pressure = 1.f;
            }

            m_lastCursorPosition = point;

            Helper::pushMouseButtonEvent( fingerIndex, point.x, point.y, MC_LBUTTON, pressure, true );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSInput::handleTouchMoved( NSSet<UITouch *> * _touches, UIView * _view )
    {
        for( UITouch * touch in _touches )
        {
            ETouchCode fingerIndex = this->getFingerIndex_( touch );

            if( fingerIndex == TC_TOUCH_INVALID )
            {
                LOGGER_INFO( "platform", "touch moved: unknown touch" );

                continue;
            }

            CGPoint location = [touch locationInView:_view];
            CGPoint prevLocation = [touch previousLocationInView:_view];

            mt::vec2f point;
            this->calcCursorPosition_( _view, location, &point );

            mt::vec2f prevPoint;
            this->calcCursorPosition_( _view, prevLocation, &prevPoint );

            float dx = point.x - prevPoint.x;
            float dy = point.y - prevPoint.y;

            float pressure = static_cast<float>(touch.force / touch.maximumPossibleForce);

            if( touch.maximumPossibleForce <= 0.f )
            {
                pressure = 1.f;
            }

            m_lastCursorPosition = point;

            Helper::pushMouseMoveEvent( fingerIndex, point.x, point.y, dx, dy, pressure, 0.f );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSInput::handleTouchEnded( NSSet<UITouch *> * _touches, UIView * _view )
    {
        for( UITouch * touch in _touches )
        {
            ETouchCode fingerIndex = this->releaseFingerIndex_( touch );

            if( fingerIndex == TC_TOUCH_INVALID )
            {
                LOGGER_INFO( "platform", "touch ended: unknown touch" );

                continue;
            }

            CGPoint location = [touch locationInView:_view];

            mt::vec2f point;
            this->calcCursorPosition_( _view, location, &point );

            float pressure = static_cast<float>(touch.force / touch.maximumPossibleForce);

            if( touch.maximumPossibleForce <= 0.f )
            {
                pressure = 1.f;
            }

            m_lastCursorPosition = point;

            Helper::pushMouseButtonEvent( fingerIndex, point.x, point.y, MC_LBUTTON, pressure, false );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSInput::handleTouchCancelled( NSSet<UITouch *> * _touches, UIView * _view )
    {
        this->handleTouchEnded( _touches, _view );
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSInput::isKeyDown( EKeyCode _code ) const
    {
        bool isDown = m_keyDown[_code];

        return isDown;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSInput::isAnyKeyDown() const
    {
        for( uint32_t i = 0; i != MENGINE_INPUT_MAX_KEY_CODE; ++i )
        {
            bool isDown = m_keyDown[i];

            if( isDown == false )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSInput::getCursorPosition( mt::vec2f * const _point ) const
    {
        *_point = m_lastCursorPosition;
    }
    //////////////////////////////////////////////////////////////////////////
}