#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/BaseTransformation.h"

#include "Kernel/RenderCameraOrthogonal.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderCameraOrthogonalTarget
        : public Node
        , protected BaseUpdation
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( RenderCameraOrthogonal );
        DECLARE_VISITABLE( Node );
        DECLARE_UPDATABLE();
        DECLARE_TRANSFORMABLE();

    public:
        RenderCameraOrthogonalTarget();
        ~RenderCameraOrthogonalTarget() override;

    public:
        void setRenderCameraOrthogonal( RenderCameraOrthogonal * const _camera );
        RenderCameraOrthogonal * getRenderCameraOrthogonal() const;

    public:
        void setSpeed( float _speed );
        float getSpeed() const;

    public:
        void setFixedHorizont( float _horizont );
        float getFixedHorizont() const;
        bool isFixedHorizont() const;

    public:
        void update( const UpdateContext * _context ) override;

    protected:
        RenderCameraOrthogonal * m_camera;
        float m_speed;

        bool m_fixedHorizont;
        float m_horizont;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<RenderCameraOrthogonalTarget> RenderCameraOrthogonalTargetPtr;
    //////////////////////////////////////////////////////////////////////////
}
