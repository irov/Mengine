#pragma once

#include "Kernel/RenderCamera.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderCameraOrthogonal
        : public RenderCamera
    {
        DECLARE_FACTORABLE( RenderCameraOrthogonal );

    public:
        RenderCameraOrthogonal();
        ~RenderCameraOrthogonal() override;

    public:
        void setCameraPosition( const mt::vec3f & _cameraPosition );
        void setCameraDirection( const mt::vec3f & _cameraDirection );
        void setCameraUp( const mt::vec3f & _cameraUp );
        void setCameraRightSign( float _cameraRightSign );

    public:
        void setCameraNear( float _cameraNear );
        void setCameraFar( float _cameraFar );

    public:
        void setCameraScale( const mt::vec2f & _cameraScale );
        const mt::vec2f & getCameraScale() const;

        void setCameraOffset( const mt::vec2f & _cameraOffset );
        const mt::vec2f & getCameraOffset() const;

    public:
        void setOrthogonalViewport( const Viewport & _orthogonalViewport );
        const Viewport & getOrthogonalViewport() const;

    public:
        void setFixedOrthogonalViewport( bool _fixedOrthogonalViewport );
        bool getFixedOrthogonalViewport() const;

    public:
        void setProxyViewMatrix( bool _proxyViewMatrix );
        bool getProxyViewMatrix() const;

    public:
        void setClampViewport( bool _clampViewport );
        bool getClampViewport() const;

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

        float m_cameraNear;
        float m_cameraFar;

        mt::vec2f m_cameraScale;
        mt::vec2f m_cameraOffset;

        Viewport m_orthogonalViewport;

        mutable Viewport m_renderViewport;

        bool m_proxyViewMatrix;
        bool m_fixedOrthogonalViewport;

        bool m_clampViewport;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderCameraOrthogonal, RenderCameraInterface> RenderCameraOrthogonalPtr;
    //////////////////////////////////////////////////////////////////////////
}
