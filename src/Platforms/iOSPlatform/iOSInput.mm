#include "iOSInput.h"

#include "Kernel/Logger.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/TimestampHelper.h"

#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static constexpr NSTimeInterval IOS_ACCELEROMETER_UPDATE_INTERVAL = 0.02;
        //////////////////////////////////////////////////////////////////////////
        static Timestamp getTouchTimestamp( const UITouch * _touch )
        {
            NSTimeInterval touchTimestamp = _touch.timestamp;

            Timestamp timestamp = static_cast<Timestamp>(touchTimestamp * 1000.0);

            return timestamp;
        }
        //////////////////////////////////////////////////////////////////////////
        static Timestamp getAccelerometerTimestamp( const CMAccelerometerData * _data )
        {
            NSTimeInterval accelerometerTimestamp = _data.timestamp;

            Timestamp timestamp = static_cast<Timestamp>(accelerometerTimestamp * 1000000000.0);

            return timestamp;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    iOSInput::iOSInput()
        : m_motionManager( nil )
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
        m_motionManager = [[CMMotionManager alloc] init];

        m_motionManager.accelerometerUpdateInterval = Detail::IOS_ACCELEROMETER_UPDATE_INTERVAL;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSInput::finalize()
    {
        this->stopAccelerometer();

        m_motionManager = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSInput::startAccelerometer()
    {
        if( m_motionManager.accelerometerAvailable == NO )
        {
            return;
        }

        if( m_motionManager.accelerometerActive == YES )
        {
            return;
        }

        [m_motionManager startAccelerometerUpdatesToQueue:[NSOperationQueue mainQueue]
                                             withHandler:^(CMAccelerometerData * data, NSError * error) {
            MENGINE_UNUSED( error );

            if( data == nil )
            {
                return;
            }

            Timestamp timestampSensor = Detail::getAccelerometerTimestamp( data );
            Timestamp timestampSensorMs = Helper::convertTimestampNanosecondsToMilliseconds( timestampSensor );
            Timestamp bootTimestamp = Helper::getBootTimestamp();
            Timestamp platformTimestamp = Helper::getPlatformTimestamp();
            Timestamp timestamp = Helper::convertTimestampTimebase( timestampSensorMs, bootTimestamp, platformTimestamp );

            CMAcceleration acceleration = data.acceleration;

            Helper::pushAccelerometerEvent( timestamp, timestampSensor
                , static_cast<float>(acceleration.x)
                , static_cast<float>(acceleration.y)
                , static_cast<float>(acceleration.z)
            );
        }];
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSInput::stopAccelerometer()
    {
        if( m_motionManager.accelerometerActive == NO )
        {
            return;
        }

        [m_motionManager stopAccelerometerUpdates];
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

            Timestamp timestamp = Detail::getTouchTimestamp( touch );

            Helper::pushMouseButtonEvent( timestamp, fingerIndex, point.x, point.y, MC_LBUTTON, pressure, true );
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

            Timestamp timestamp = Detail::getTouchTimestamp( touch );

            Helper::pushMouseMoveEvent( timestamp, fingerIndex, point.x, point.y, dx, dy, pressure, 0.f );
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

            Timestamp timestamp = Detail::getTouchTimestamp( touch );

            Helper::pushMouseButtonEvent( timestamp, fingerIndex, point.x, point.y, MC_LBUTTON, pressure, false );
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
