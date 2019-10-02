#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/Materialable.h"

#include "Kernel/RenderVertex2D.h"

namespace Mengine
{
    class Line
        : public Node
        , public BaseRender
        , public Materialable
    {
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();

    public:
        Line();
        ~Line() override;

    public:
        void setFrom( const mt::vec3f & _value );
        const mt::vec3f & getFrom() const;

        void setTo( const mt::vec3f & _value );
        const mt::vec3f & getTo() const;

        void setWidth( float _width );
        float getWidth() const;

    protected:
        RenderMaterialInterfacePtr _updateMaterial() const override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        mt::vec3f m_from;
        mt::vec3f m_to;

        float m_width;

        mutable RenderVertex2D m_vertices[4];
    };
}
