#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseTransformation.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MovieSceneEffect
        : public Node
        , protected BaseRender
        , protected BaseTransformation
    {
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();

    public:
        MovieSceneEffect();
        ~MovieSceneEffect() override;

    public:
        bool setPropagateNode( Node * _propagate );

    public:
        void enablePropagate( bool _value );

    protected:
        void invalidateColor() const override;

    protected:
        void _invalidateWorldMatrix() const override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        mutable Node * m_propagate;

        bool m_propagateEnable;

        mt::vec3f m_position;
        mt::vec3f m_origin;

        mt::vec3f m_scale;
        mt::vec2f m_skew;
        mt::vec3f m_rotation;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<MovieSceneEffect> MovieSceneEffectPtr;
    //////////////////////////////////////////////////////////////////////////
}