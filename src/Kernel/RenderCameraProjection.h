#pragma once

#include "Kernel/RenderCamera.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderCameraProjection
        : public RenderCamera
    {
        DECLARE_FACTORABLE( RenderCameraProjection );

    public:
        RenderCameraProjection();
        ~RenderCameraProjection() override;

    public:
        void setCameraPosition( const mt::vec3f & _cameraPosition );
        void setCameraDirection( const mt::vec3f & _cameraDirection );
        void setCameraUp( const mt::vec3f & _cameraUp );
        void setCameraRightSign( float _cameraRightSign );

    public:
        void setCameraFOV( float _cameraFOV );
        void setCameraAspect( float _cameraAspect );
        void setCameraNear( float _cameraNear );
        void setCameraFar( float _cameraFar );

    protected:
        void fromWorldToScreenPosition( const mt::mat4f & _worldMatrix, mt::vec2f * const _screenPosition ) const override;

    protected:
        void fromScreenToWorldPosition( const mt::vec2f & _screenPoint, float _deep, mt::vec3f * const _worldPosition ) const override;
        void fromScreenToWorldDelta( const mt::vec2f & _screenDelta, float _deep, mt::vec3f * const _worldDelta ) const override;

    protected:
        void _updateViewMatrix() const override;
        void _updateProjectionMatrix() const override;

    protected:
        void makeViewport_( Viewport * const _viewport ) const;

    protected:
        mt::vec3f m_cameraPosition;
        mt::vec3f m_cameraDirection;
        mt::vec3f m_cameraUp;
        float m_cameraRightSign;

        float m_cameraFOV;
        float m_cameraAspect;
        float m_cameraNear;
        float m_cameraFar;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderCameraProjection, RenderCameraInterface> RenderCameraProjectionPtr;
    //////////////////////////////////////////////////////////////////////////
}
