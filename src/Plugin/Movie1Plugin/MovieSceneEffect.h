#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"

namespace Mengine
{
    class MovieSceneEffect
        : public Node
        , public BaseRender
    {
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();

    public:
        MovieSceneEffect();
        ~MovieSceneEffect() override;

    public:
        void setPropagateNode( Node * _propagate );

    public:
        void enablePropagate( bool _value );

    protected:
        void _invalidateColor() override;

    protected:
        void _invalidateWorldMatrix() override;

    protected:
        void _render( const RenderContext * _context ) override;

    protected:
        Node * m_propagate;

        bool m_propagateEnable;

        mt::vec3f m_position;
        mt::vec3f m_origin;

        mt::vec3f m_scale;
        mt::vec2f m_skew;
        mt::vec3f m_rotation;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MovieSceneEffect> MovieSceneEffectPtr;
    //////////////////////////////////////////////////////////////////////////
}