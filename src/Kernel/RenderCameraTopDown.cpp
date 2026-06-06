#include "RenderCameraTopDown.h"

#include "Interface/UpdationInterface.h"

#include "Config/StdMath.h"

#include "math/vec3.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static mt::vec3f orbitDirectionFromAngles( float _pitch, float _yaw )
        {
            float cp = StdMath::cosf( _pitch );
            float sp = StdMath::sinf( _pitch );
            float cy = StdMath::cosf( _yaw );
            float sy = StdMath::sinf( _yaw );

            mt::vec3f dir;
            dir.x = cp * sy;
            dir.y = sp;
            dir.z = cp * cy;

            return dir;
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec3f computeUpForLook( const mt::vec3f & _look )
        {
            mt::vec3f worldUp( 0.f, 1.f, 0.f );

            mt::vec3f right;
            mt::cross_v3_v3( &right, _look, worldUp );

            float r2 = mt::sqrlength_v3( right );

            if( r2 < 1e-6f )
            {
                worldUp = mt::vec3f( 0.f, 0.f, 1.f );
                mt::cross_v3_v3( &right, _look, worldUp );
            }

            mt::vec3f up;
            mt::cross_v3_v3( &up, right, _look );

            mt::vec3f upN;
            mt::norm_safe_v3( &upN, up );

            return upN;
        }
        //////////////////////////////////////////////////////////////////////////
        static float lcgNext( uint32_t * const _state )
        {
            uint32_t s = *_state * 1664525u + 1013904223u;
            *_state = s;

            uint32_t bits = (s >> 8) & 0x00ffffffu;
            float f = (float)bits / (float)0x00ffffffu;
            return f * 2.f - 1.f;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    RenderCameraTopDown::RenderCameraTopDown()
        : m_camera( nullptr )
        , m_pitch( 1.0471975512f )
        , m_yaw( 0.f )
        , m_distance( 1500.f )
        , m_offset( 0.f, 0.f, 0.f )
        , m_followSpeed( 0.f )
        , m_currentEye( 0.f, 0.f, 0.f )
        , m_currentEyeValid( false )
        , m_shakeAmplitude( 0.f )
        , m_shakeDuration( 0.f )
        , m_shakeTimeLeft( 0.f )
        , m_shakeSeed( 0x9E3779B9u )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderCameraTopDown::~RenderCameraTopDown()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraTopDown::setRenderCameraProjection( RenderCameraProjection * const _camera )
    {
        m_camera = _camera;

        m_currentEyeValid = false;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderCameraProjection * RenderCameraTopDown::getRenderCameraProjection() const
    {
        return m_camera;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraTopDown::setPitch( float _pitch )
    {
        m_pitch = _pitch;
    }
    //////////////////////////////////////////////////////////////////////////
    float RenderCameraTopDown::getPitch() const
    {
        return m_pitch;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraTopDown::setYaw( float _yaw )
    {
        m_yaw = _yaw;
    }
    //////////////////////////////////////////////////////////////////////////
    float RenderCameraTopDown::getYaw() const
    {
        return m_yaw;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraTopDown::setDistance( float _distance )
    {
        m_distance = _distance;
    }
    //////////////////////////////////////////////////////////////////////////
    float RenderCameraTopDown::getDistance() const
    {
        return m_distance;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraTopDown::setOffset( const mt::vec3f & _offset )
    {
        m_offset = _offset;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec3f & RenderCameraTopDown::getOffset() const
    {
        return m_offset;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraTopDown::setFollowSpeed( float _followSpeed )
    {
        m_followSpeed = _followSpeed;
    }
    //////////////////////////////////////////////////////////////////////////
    float RenderCameraTopDown::getFollowSpeed() const
    {
        return m_followSpeed;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraTopDown::shake( float _amplitude, float _duration )
    {
        m_shakeAmplitude = _amplitude;
        m_shakeDuration = (_duration > 0.f) ? _duration : 0.f;
        m_shakeTimeLeft = m_shakeDuration;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraTopDown::snapToTarget()
    {
        m_currentEyeValid = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraTopDown::update( const UpdateContext * _context )
    {
        if( m_camera == nullptr )
        {
            return;
        }

        float dt = _context->time * 0.001f;

        mt::vec3f selfWP = this->getWorldPosition();

        mt::vec3f target;
        target.x = selfWP.x + m_offset.x;
        target.y = selfWP.y + m_offset.y;
        target.z = selfWP.z + m_offset.z;

        mt::vec3f orbitDir = Detail::orbitDirectionFromAngles( m_pitch, m_yaw );

        mt::vec3f desiredEye;
        desiredEye.x = target.x + orbitDir.x * m_distance;
        desiredEye.y = target.y + orbitDir.y * m_distance;
        desiredEye.z = target.z + orbitDir.z * m_distance;

        if( m_currentEyeValid == false || m_followSpeed <= 0.f || dt <= 0.f )
        {
            m_currentEye = desiredEye;
            m_currentEyeValid = true;
        }
        else
        {
            float k = 1.f - StdMath::expf( -m_followSpeed * dt );

            m_currentEye.x += (desiredEye.x - m_currentEye.x) * k;
            m_currentEye.y += (desiredEye.y - m_currentEye.y) * k;
            m_currentEye.z += (desiredEye.z - m_currentEye.z) * k;
        }

        mt::vec3f finalEye = m_currentEye;

        if( m_shakeTimeLeft > 0.f && m_shakeDuration > 0.f && m_shakeAmplitude > 0.f )
        {
            float k = m_shakeTimeLeft / m_shakeDuration;
            float amp = m_shakeAmplitude * k;

            float rx = Detail::lcgNext( &m_shakeSeed );
            float ry = Detail::lcgNext( &m_shakeSeed );
            float rz = Detail::lcgNext( &m_shakeSeed );

            finalEye.x += rx * amp;
            finalEye.y += ry * amp;
            finalEye.z += rz * amp;

            m_shakeTimeLeft -= dt;

            if( m_shakeTimeLeft < 0.f )
            {
                m_shakeTimeLeft = 0.f;
            }
        }

        this->applyToCamera_( finalEye, target );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderCameraTopDown::applyToCamera_( const mt::vec3f & _eyePos, const mt::vec3f & _target )
    {
        mt::vec3f look;
        look.x = _target.x - _eyePos.x;
        look.y = _target.y - _eyePos.y;
        look.z = _target.z - _eyePos.z;

        mt::vec3f lookN;
        mt::norm_safe_v3( &lookN, look );

        mt::vec3f up = Detail::computeUpForLook( lookN );

        m_camera->setCameraPosition( _eyePos );
        m_camera->setCameraDirection( lookN );
        m_camera->setCameraUp( up );
    }
    //////////////////////////////////////////////////////////////////////////
}
