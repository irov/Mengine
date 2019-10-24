#pragma once

#include "Kernel/RenderCamera.h"

namespace Mengine
{
    class RenderCameraOrthogonal
        : public RenderCamera
    {
    public:
        RenderCameraOrthogonal();
        ~RenderCameraOrthogonal() override;

    public:
        void fromWorldToScreenPosition( const mt::mat4f & _worldMatrix, mt::vec2f & _screenPosition ) const override;

    public:
        void setCameraPosition( const mt::vec3f & _pos );
        void setCameraDirection( const mt::vec3f & _dir );
        void setCameraUp( const mt::vec3f & _up );
        void setCameraRightSign( float _rightSign );

    public:
        void setCameraNear( float _near );
        void setCameraFar( float _far );

    public:
        void setOrthogonalViewport( const Viewport & _viewport );
        const Viewport & getOrthogonalViewport() const;

    public:
        void setFixedOrthogonalViewport( bool _value );
        bool getFixedOrthogonalViewport() const;

    public:
        void setProxyViewMatrix( bool _value );
        bool getProxyViewMatrix() const;

    protected:
        void _updateViewMatrix() const override;
        void _updateProjectionMatrix() const override;

    protected:
        void makeViewport_( Viewport & _viewport ) const;

    protected:
        mt::vec3f m_cameraPosition;
        mt::vec3f m_cameraDirection;
        mt::vec3f m_cameraUp;
        float m_cameraRightSign;

        float m_cameraNear;
        float m_cameraFar;

        Viewport m_orthogonalViewport;

        mutable Viewport m_renderViewport;

        bool m_proxyViewMatrix;
        bool m_fixedOrthogonalViewport;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<RenderCameraOrthogonal> RenderCameraOrthogonalPtr;
    //////////////////////////////////////////////////////////////////////////
}
