#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/RenderCameraProjection.h"

#include "math/vec3.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    class RenderCameraTopDown
        : public Node
        , protected BaseUpdation
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( RenderCameraTopDown );
        DECLARE_VISITABLE( Node );
        DECLARE_UPDATABLE();
        DECLARE_TRANSFORMABLE();

    public:
        RenderCameraTopDown();
        ~RenderCameraTopDown() override;

    public:
        void setRenderCameraProjection( RenderCameraProjection * const _camera );
        RenderCameraProjection * getRenderCameraProjection() const;

    public:
        void setPitch( float _pitch );
        float getPitch() const;

        void setYaw( float _yaw );
        float getYaw() const;

        void setDistance( float _distance );
        float getDistance() const;

        void setOffset( const mt::vec3f & _offset );
        const mt::vec3f & getOffset() const;

        void setFollowSpeed( float _followSpeed );
        float getFollowSpeed() const;

    public:
        void shake( float _amplitude, float _duration );

    public:
        void snapToTarget();

    public:
        void update( const UpdateContext * _context ) override;

    protected:
        void applyToCamera_( const mt::vec3f & _eyePos, const mt::vec3f & _target );

    protected:
        RenderCameraProjection * m_camera;

        float m_pitch;
        float m_yaw;
        float m_distance;
        mt::vec3f m_offset;

        float m_followSpeed;

        mt::vec3f m_currentEye;
        bool m_currentEyeValid;

        float m_shakeAmplitude;
        float m_shakeDuration;
        float m_shakeTimeLeft;
        uint32_t m_shakeSeed;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderCameraTopDown, Node> RenderCameraTopDownPtr;
    //////////////////////////////////////////////////////////////////////////
}
