#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"

#include "Kernel/RenderVertex2D.h"
#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/VectorRenderIndex.h"

#include "math/vec2.h"

namespace Mengine
{
    class Vectorizator
        : public Node
        , public BaseRender
    {
        DECLARE_VISITABLE( Vectorizator );
        DECLARE_RENDERABLE();

    public:
        Vectorizator();
        ~Vectorizator() override;

    public:
        uint32_t addLine( const mt::vec2f & _from, const mt::vec2f & _to, float _weight, const Color & _color );
        void removeLine( uint32_t _id );

    public:
        uint32_t addQuadraticBezier( const mt::vec2f & _from, const mt::vec2f & _to, const mt::vec2f & _p0, uint32_t _quality, float _weight, const Color & _color );
        void removeQuadraticBezier( uint32_t _id );

    protected:
        void render( const RenderContext * _context ) const override;

    protected:
        void updateLocalVertex2D_() const;

    protected:
        uint32_t m_enumerator;

        struct LineDesc
        {
            uint32_t id;
            mt::vec2f from;
            mt::vec2f to;
            float weight;

            Color color;
        };

        typedef Vector<LineDesc> VectorLines;
        VectorLines m_lines;

        struct QuadraticBezierDesc
        {
            uint32_t id;
            mt::vec2f from;
            mt::vec2f to;
            mt::vec2f p[1];
            uint32_t quality;
            float weight;

            Color color;
        };

        typedef Vector<QuadraticBezierDesc> VectorQuadraticBeziers;
        VectorQuadraticBeziers m_quadraticBeziers;

        mutable VectorRenderVertex2D m_renderVertex2D;
        mutable VectorRenderIndex m_renderIndices;

        mutable bool m_invalidateLocalVertex2D;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Vectorizator> VectorizatorPtr;
    //////////////////////////////////////////////////////////////////////////
}
