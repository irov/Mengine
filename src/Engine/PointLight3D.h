#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseTransformation.h"

#include "math/vec3.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    class PointLight3D
        : public Node
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( PointLight3D );
        DECLARE_VISITABLE( Node );
        DECLARE_TRANSFORMABLE();

    public:
        PointLight3D();
        ~PointLight3D() override;

    public:
        void setLightRadius( float _radius );
        float getLightRadius() const;

        void setLightColor( const mt::vec3f & _color );
        const mt::vec3f & getLightColor() const;

        void setLightIntensity( float _intensity );
        float getLightIntensity() const;

    public:
        void updateLightFromTransform();

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        void apply_();

    protected:
        float m_radius;
        mt::vec3f m_color;
        float m_intensity;

        uint32_t m_slot;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<PointLight3D> PointLight3DPtr;
    //////////////////////////////////////////////////////////////////////////
}
