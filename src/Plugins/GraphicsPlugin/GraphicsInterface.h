#pragma once

#include "Interface/UnknownNodeInterface.h"

#include "Kernel/Color.h"
#include "Kernel/Node.h"
#include "Kernel/Polygon.h"
#include "Kernel/Vector.h"

#include "math/box2.h"
#include "math/vec2.h"
#include "math/vec4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum class EGraphicsLineCap : uint8_t
    {
        Butt,
        Square,
        Round
    };
    //////////////////////////////////////////////////////////////////////////
    enum class EGraphicsLineJoin : uint8_t
    {
        Miter,
        Bevel,
        Round
    };
    //////////////////////////////////////////////////////////////////////////
    enum class EGraphicsFillRule : uint8_t
    {
        NonZero,
        EvenOdd
    };
    //////////////////////////////////////////////////////////////////////////
    enum class EGraphicsStrokeAlignment : uint8_t
    {
        Center,
        Inner,
        Outer
    };
    //////////////////////////////////////////////////////////////////////////
    enum class EGraphicsUVMode : uint8_t
    {
        Bounds,
        Path
    };
    //////////////////////////////////////////////////////////////////////////
    enum class EGraphicsPathMarkerType : uint8_t
    {
        None,
        FilledArrow,
        OpenArrow,
        Triangle,
        Circle,
        Diamond
    };
    //////////////////////////////////////////////////////////////////////////
    enum EGraphicsPathStyleAttribute : uint32_t
    {
        GraphicsPathStyleAttributeWidth = 1U << 0U,
        GraphicsPathStyleAttributeColor = 1U << 1U,
        GraphicsPathStyleAttributeOutlineColor = 1U << 2U
    };
    //////////////////////////////////////////////////////////////////////////
    struct GraphicsPathPoint
    {
        mt::vec2f position;
        float width = 1.f;
        mt::vec4f color;
        mt::vec4f outlineColor;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<GraphicsPathPoint> VectorGraphicsPathPoints;
    //////////////////////////////////////////////////////////////////////////
    struct GraphicsPathMarker
    {
        EGraphicsPathMarkerType type = EGraphicsPathMarkerType::None;
        float length = 0.f;
        float width = 0.f;
        float inset = 0.f;
    };
    //////////////////////////////////////////////////////////////////////////
    class GraphicsPath;
    typedef IntrusivePtr<GraphicsPath> GraphicsPathPtr;
    //////////////////////////////////////////////////////////////////////////
    class GraphicsInterface
        : public UnknownNodeInterface
    {
    public:
        virtual void setLineWidth( float _width ) = 0;
        virtual float getLineWidth() const = 0;
        virtual void setOutlineWidth( float _outlineWidth ) = 0;
        virtual float getOutlineWidth() const = 0;
        virtual void setOutlineColor( const Color & _color ) = 0;
        virtual Color getOutlineColor() const = 0;
        virtual void setLineSoft( float _penumbra ) = 0;
        virtual float getLineSoft() const = 0;
        virtual void setLineColor( const Color & _color ) = 0;
        virtual Color getLineColor() const = 0;
        virtual void setLineCap( EGraphicsLineCap _cap ) = 0;
        virtual EGraphicsLineCap getLineCap() const = 0;
        virtual void setLineJoin( EGraphicsLineJoin _join ) = 0;
        virtual EGraphicsLineJoin getLineJoin() const = 0;
        virtual void setMiterLimit( float _limit ) = 0;
        virtual float getMiterLimit() const = 0;
        virtual void setFillRule( EGraphicsFillRule _rule ) = 0;
        virtual EGraphicsFillRule getFillRule() const = 0;
        virtual void setLineDash( const Vector<float> & _pattern, float _offset ) = 0;
        virtual void clearLineDash() = 0;
        virtual void setPathTrim( float _from, float _to ) = 0;
        virtual void setPathMarkers( const GraphicsPathMarker & _start, const GraphicsPathMarker & _end ) = 0;
        virtual void setStrokeAlignment( EGraphicsStrokeAlignment _alignment ) = 0;
        virtual EGraphicsStrokeAlignment getStrokeAlignment() const = 0;
        virtual void setUVMode( EGraphicsUVMode _mode ) = 0;
        virtual EGraphicsUVMode getUVMode() const = 0;
        virtual void setCurveQuality( uint8_t _quality ) = 0;
        virtual uint8_t getCurveQuality() const = 0;
        virtual void setEllipseQuality( uint8_t _quality ) = 0;
        virtual uint8_t getEllipseQuality() const = 0;
        virtual void setRectQuality( uint8_t _quality ) = 0;
        virtual uint8_t getRectQuality() const = 0;

    public:
        virtual void pushState() = 0;
        virtual void popState() = 0;
        virtual void beginFill() = 0;
        virtual void endFill() = 0;

    public:
        virtual GraphicsPathPtr createPath() = 0;
        virtual void drawPath( const GraphicsPathPtr & _path ) = 0;

    public:
        virtual void drawRect( const mt::vec2f & _point, float _width, float _height ) = 0;
        virtual void drawRoundedRect( const mt::vec2f & _point, float _width, float _height, float _radius ) = 0;
        virtual void drawRoundedRectVarying( const mt::vec2f & _point, float _width, float _height, const mt::vec4f & _radii ) = 0;
        virtual void drawRoundedPolygon( const Polygon & _polygon, float _radius ) = 0;
        virtual void drawPolyline( const Polygon & _polygon, bool _closed ) = 0;
        virtual void drawStyledPolyline( const VectorGraphicsPathPoints & _points, bool _closed ) = 0;
        virtual void drawRoundedPolyline( const VectorGraphicsPathPoints & _points, float _radius, bool _closed ) = 0;
        virtual void drawPolygonOffset( const Polygon & _polygon, float _offset, EGraphicsLineJoin _join ) = 0;
        virtual void drawCircle( const mt::vec2f & _point, float _radius ) = 0;
        virtual void drawEllipse( const mt::vec2f & _point, float _width, float _height ) = 0;
        virtual void drawArc( const mt::vec2f & _point, float _radius, float _angleFrom, float _angleTo ) = 0;
        virtual void drawEllipseArc( const mt::vec2f & _point, float _width, float _height, float _angleFrom, float _angleTo ) = 0;
        virtual void drawRing( const mt::vec2f & _point, float _innerRadius, float _outerRadius, float _angleFrom, float _angleTo ) = 0;
        virtual void drawRoundedRing( const mt::vec2f & _point, float _innerRadius, float _outerRadius, float _angleFrom, float _angleTo ) = 0;
        virtual void drawCapsule( const mt::vec2f & _point, float _width, float _height ) = 0;
        virtual void drawRegularPolygon( const mt::vec2f & _point, float _radius, uint32_t _vertexCount, float _rotation ) = 0;
        virtual void drawStar( const mt::vec2f & _point, float _innerRadius, float _outerRadius, uint32_t _rayCount, float _rotation ) = 0;
        virtual void drawSuperellipse( const mt::vec2f & _point, float _width, float _height, float _exponent, float _rotation ) = 0;

    public:
        virtual mt::box2f getBounds() const = 0;
        virtual void clear() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<GraphicsInterface> GraphicsInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
