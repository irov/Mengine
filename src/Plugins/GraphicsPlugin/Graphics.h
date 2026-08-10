#pragma once

#include "GraphicsInterface.h"

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseTransformation.h"

#include "Kernel/RenderVertex2D.h"
#include "Kernel/VectorRenderVertex2D.h"
#include "Kernel/VectorRenderIndex.h"

#include "graphics/graphics.hpp"
#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Graphics
        : public Node
        , public GraphicsInterface
        , protected BaseRender
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( Graphics );
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();
        DECLARE_UNKNOWABLE();
        DECLARE_NODE_UNKNOWABLE();

    public:
        Graphics();
        ~Graphics() override;

    public:
        bool _compile() override;
        void _release() override;

    public:
        void setLineWidth( float _width ) override;
        float getLineWidth() const override;

        void setOutlineWidth( float _outlineWidth ) override;
        float getOutlineWidth() const override;

        void setOutlineColor( const Color & _color ) override;
        Color getOutlineColor() const override;

        void setLineSoft( float _penumbra ) override;
        float getLineSoft() const override;

        void setLineColor( const Color & _color ) override;
        Color getLineColor() const override;

        void setLineCap( EGraphicsLineCap _cap ) override;
        EGraphicsLineCap getLineCap() const override;

        void setLineJoin( EGraphicsLineJoin _join ) override;
        EGraphicsLineJoin getLineJoin() const override;

        void setMiterLimit( float _limit ) override;
        float getMiterLimit() const override;

        void setFillRule( EGraphicsFillRule _rule ) override;
        EGraphicsFillRule getFillRule() const override;

        void setCurveQuality( uint8_t _quality ) override;
        uint8_t getCurveQuality() const override;

        void setEllipseQuality( uint8_t _quality ) override;
        uint8_t getEllipseQuality() const override;

        void setRectQuality( uint8_t _quality ) override;
        uint8_t getRectQuality() const override;

    public:
        void pushState() override;
        void popState() override;

    public:
        void beginFill() override;
        void endFill() override;
        void beginCompoundFill( EGraphicsFillRule _rule ) override;
        void endCompoundFill() override;

    public:
        void pointMoveTo( const mt::vec2f & _point ) override;
        void pointLineTo( const mt::vec2f & _point ) override;
        void pointQuadraticCurveTo( const mt::vec2f & _p0, const mt::vec2f & _point ) override;
        void pointBezierCurveTo( const mt::vec2f & _p0, const mt::vec2f & _p1, const mt::vec2f & _point ) override;
        void pointClose() override;
        void pointArcTo( const mt::vec2f & _p0, const mt::vec2f & _p1, float _radius ) override;

    public:
        void drawRect( const mt::vec2f & _point, float _width, float _height ) override;
        void drawRoundedRect( const mt::vec2f & _point, float _width, float _height, float _radius ) override;
        void drawRoundedPolygon( const Polygon & _polygon, float _radius ) override;
        void drawPolyline( const Polygon & _polygon, bool _closed ) override;
        void drawCircle( const mt::vec2f & _point, float _radius ) override;
        void drawEllipse( const mt::vec2f & _point, float _width, float _height ) override;
        void drawArc( const mt::vec2f & _point, float _radius, float _angleFrom, float _angleTo ) override;
        void drawEllipseArc( const mt::vec2f & _point, float _width, float _height, float _angleFrom, float _angleTo ) override;
        void drawSector( const mt::vec2f & _point, float _radius, float _angleFrom, float _angleTo ) override;
        void drawRing( const mt::vec2f & _point, float _innerRadius, float _outerRadius, float _angleFrom, float _angleTo ) override;

    public:
        mt::box2f getBounds() const override;

    public:
        void clear() override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        void _invalidateWorldMatrix() const override;
        void _invalidateColor() const override;

    protected:
        void updateLocalVertex2D_() const;

    protected:
        gp_canvas_t * m_canvas;

        RenderMaterialInterfacePtr m_material;

        mutable VectorRenderVertex2D m_renderVertex2D;
        mutable VectorRenderIndex m_renderIndices;
        mutable mt::box2f m_renderBoundingBox;

        mutable bool m_invalidateLocalVertex2D;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Graphics> GraphicsPtr;
    //////////////////////////////////////////////////////////////////////////
}
