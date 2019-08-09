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
        void setLineWeight( float _weight );
        float getLineWeight() const;

        void setLineSoft( float _soft );
        float getLineSoft() const;

        void setLineColor( const Color & _color );
        const Color & getLineColor() const;

        void setCurveQuality( uint8_t _quality );
        uint8_t getCurveQuality() const;

        void setEllipseQuality( uint8_t _quality );
        uint8_t getEllipseQuality() const;

    public:
        void moveTo( const mt::vec2f & _point );
        void lineTo( const mt::vec2f & _point );
        void quadraticCurveTo( const mt::vec2f & _p0, const mt::vec2f & _point );
        void bezierCurveTo( const mt::vec2f & _p0, const mt::vec2f & _p1, const mt::vec2f & _point );

    public:
        void drawRect( const mt::vec2f & _point, float _width, float _height );
        void drawCircle( const mt::vec2f & _point, float _radius );
        void drawEllipse( const mt::vec2f & _point, float _width, float _height );

    public:
        void clear();

    protected:
        void render( const RenderContext * _context ) const override;

    protected:
        void _invalidateWorldMatrix() override;
        void _invalidateColor() override;

    protected:
        void updateLocalVertex2D_() const;

    protected:
        float m_lineWeight;
        float m_lineSoft;
        Color m_lineColor;

        uint8_t m_curveQuality;
        uint8_t m_ellipseQuality;

        typedef Vector<mt::vec2f> VectorPoints;

        struct LineEdge
        {
            uint32_t controls;
            mt::vec2f p[3];

            uint8_t quality;
            float dt;

            float weight;
            Color color;
        };

        typedef Vector<LineEdge> VectorLineEdge;

        struct LineDesc
        {
            VectorPoints points;
            VectorLineEdge edges;

            float soft;
        };

        typedef Vector<LineDesc> VectorLines;
        VectorLines m_lines;

        struct RectDesc
        {
            mt::vec2f point;
            float width;
            float height;

            float weight;
            float soft;
            Color color;
        };

        typedef Vector<RectDesc> VectorRects;
        VectorRects m_rects;

        struct EllipseDesc
        {
            mt::vec2f point;
            float width;
            float height;

            uint8_t quality;

            float weight;
            float soft;
            Color color;
        };

        typedef Vector<EllipseDesc> VectorEllipses;
        VectorEllipses m_ellipses;

        mutable VectorRenderVertex2D m_renderVertex2D;
        mutable VectorRenderIndex m_renderIndices;

        mutable bool m_invalidateLocalVertex2D;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Vectorizator> VectorizatorPtr;
    //////////////////////////////////////////////////////////////////////////
}
