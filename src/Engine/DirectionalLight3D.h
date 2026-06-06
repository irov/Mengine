#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseTransformation.h"

#include "math/vec3.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    class DirectionalLight3D
        : public Node
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( DirectionalLight3D );
        DECLARE_VISITABLE( Node );
        DECLARE_TRANSFORMABLE();

    public:
        DirectionalLight3D();
        ~DirectionalLight3D() override;

    public:
        void setLightDirection( const mt::vec3f & _dir );
        const mt::vec3f & getLightDirection() const;

        void setLightColor( const mt::vec3f & _color );
        const mt::vec3f & getLightColor() const;

        void setLightIntensity( float _intensity );
        float getLightIntensity() const;

        void setLightAmbient( const mt::vec3f & _ambient );
        const mt::vec3f & getLightAmbient() const;

    protected:
        bool _activate() override;
        void _deactivate() override;

    protected:
        void apply_();

    protected:
        mt::vec3f m_direction;
        mt::vec3f m_color;
        mt::vec3f m_ambient;
        float m_intensity;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<DirectionalLight3D> DirectionalLight3DPtr;
    //////////////////////////////////////////////////////////////////////////
}
