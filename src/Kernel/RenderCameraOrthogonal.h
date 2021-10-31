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
        void setCameraPosition( const mt::vec3f & _pos );
        void setCameraDirection( const mt::vec3f & _dir );
        void setCameraUp( const mt::vec3f & _up );
        void setCameraRightSign( float _rightSign );

    public:
        void setCameraNear( float _near );
        void setCameraFar( float _far );

    public:
        void setCameraScale( const mt::vec2f & _scale );
        const mt::vec2f & getCameraScale() const;

        void setCameraOffset( const mt::vec2f & _offset );
        const mt::vec2f & getCameraOffset() const;

    public:
        void setOrthogonalViewport( const Viewport & _viewport );
        const Viewport & getOrthogonalViewport() const;

    public:
        void setFixedOrthogonalViewport( bool _value );
        bool getFixedOrthogonalViewport() const;

    public:
        void setProxyViewMatrix( bool _value );
        bool getProxyViewMatrix() const;

    public:
        void setClampViewport( bool _value );
        bool getClampViewport() const;

    protected:
        void fromWorldToScreenPosition( const mt::mat4f & _worldMatrix, mt::vec2f * const _screenPosition ) const override;

    protected:
        void fromScreenToWorldPosition( const mt::vec2f & _screenPoint, float _deep, mt::vec3f * const _worldPosition ) const override;
        void fromScreenToWorldDelta( const mt::vec2f & _screenDeltha, float _deep, mt::vec3f * const _worldDelta ) const override;

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
    typedef IntrusiveNodePtr<RenderCameraOrthogonal> RenderCameraOrthogonalPtr;
    //////////////////////////////////////////////////////////////////////////
}
