#pragma once

#include "GraphicsInterface.h"
#include "GraphicsPath.h"

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

        void setLineDash( const Vector<float> & _pattern, float _offset ) override;
        void clearLineDash() override;
        void setPathTrim( float _from, float _to ) override;
        void setPathMarkers( const GraphicsPathMarker & _start, const GraphicsPathMarker & _end ) override;
        void setStrokeAlignment( EGraphicsStrokeAlignment _alignment ) override;
        EGraphicsStrokeAlignment getStrokeAlignment() const override;
        void setUVMode( EGraphicsUVMode _mode ) override;
        EGraphicsUVMode getUVMode() const override;
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

    public:
        GraphicsPathPtr createPath() override;
        void drawPath( const GraphicsPathPtr & _path ) override;

    public:
        void drawRect( const mt::vec2f & _point, float _width, float _height ) override;
        void drawRoundedRect( const mt::vec2f & _point, float _width, float _height, float _radius ) override;
        void drawRoundedRectVarying( const mt::vec2f & _point, float _width, float _height, const mt::vec4f & _radii ) override;
        void drawRoundedPolygon( const Polygon & _polygon, float _radius ) override;
        void drawPolyline( const Polygon & _polygon, bool _closed ) override;
        void drawStyledPolyline( const VectorGraphicsPathPoints & _points, bool _closed ) override;
        void drawRoundedPolyline( const VectorGraphicsPathPoints & _points, float _radius, bool _closed ) override;
        void drawPolygonOffset( const Polygon & _polygon, float _offset, EGraphicsLineJoin _join ) override;
        void drawCircle( const mt::vec2f & _point, float _radius ) override;
        void drawEllipse( const mt::vec2f & _point, float _width, float _height ) override;
        void drawArc( const mt::vec2f & _point, float _radius, float _angleFrom, float _angleTo ) override;
        void drawEllipseArc( const mt::vec2f & _point, float _width, float _height, float _angleFrom, float _angleTo ) override;
        void drawRing( const mt::vec2f & _point, float _innerRadius, float _outerRadius, float _angleFrom, float _angleTo ) override;
        void drawRoundedRing( const mt::vec2f & _point, float _innerRadius, float _outerRadius, float _angleFrom, float _angleTo ) override;
        void drawCapsule( const mt::vec2f & _point, float _width, float _height ) override;
        void drawRegularPolygon( const mt::vec2f & _point, float _radius, uint32_t _vertexCount, float _rotation ) override;
        void drawStar( const mt::vec2f & _point, float _innerRadius, float _outerRadius, uint32_t _rayCount, float _rotation ) override;
        void drawSuperellipse( const mt::vec2f & _point, float _width, float _height, float _exponent, float _rotation ) override;

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
        mutable Vector<gp_render_batch_t> m_renderBatches;
        mutable Vector<mt::box2f> m_renderBatchBoundingBoxes;
        mutable mt::box2f m_renderBoundingBox;

        mutable bool m_invalidateLocalVertex2D;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Graphics> GraphicsPtr;
    //////////////////////////////////////////////////////////////////////////
}
