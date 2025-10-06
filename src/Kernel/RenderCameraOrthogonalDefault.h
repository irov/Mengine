#pragma once

#include "Kernel/RenderCamera.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderCameraOrthogonalDefault
        : public RenderCamera
    {
        DECLARE_FACTORABLE( RenderCameraOrthogonalDefault );

    public:
        RenderCameraOrthogonalDefault();
        ~RenderCameraOrthogonalDefault() override;

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
        mutable Viewport m_renderViewport;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderCameraOrthogonalDefault, RenderCameraInterface> RenderCameraOrthogonalDefaultPtr;
    //////////////////////////////////////////////////////////////////////////
}
