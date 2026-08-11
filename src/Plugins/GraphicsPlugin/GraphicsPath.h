#pragma once

#include "GraphicsInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Color.h"
#include "Kernel/Polygon.h"
#include "Kernel/Vector.h"

#include "graphics/graphics.hpp"

#include "math/box2.h"
#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct GraphicsPathStyle
    {
        uint32_t attributes = 0;
        float width = 1.f;
        Color color;
        Color outlineColor;
    };
    //////////////////////////////////////////////////////////////////////////
    struct GraphicsPathSample
    {
        mt::vec2f position;
        mt::vec2f tangent;
        float distance = 0.f;
        float ratio = 0.f;
        uint32_t contourIndex = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class GraphicsPath
        : public Factorable
    {
        DECLARE_FACTORABLE( GraphicsPath );

    public:
        GraphicsPath( uint8_t _curveQuality, uint8_t _ellipseQuality );
        ~GraphicsPath() override;

    public:
        void setStyle( const GraphicsPathStyle & _style );
        void clearStyle();
        void clear();

    public:
        void moveTo( const mt::vec2f & _point );
        void lineTo( const mt::vec2f & _point );
        void quadraticCurveTo( const mt::vec2f & _control, const mt::vec2f & _point );
        void bezierCurveTo( const mt::vec2f & _control0, const mt::vec2f & _control1, const mt::vec2f & _point );
        void tangentArcTo( const mt::vec2f & _point0, const mt::vec2f & _point1, float _radius );
        void ellipseArc( const mt::vec2f & _center, float _radiusX, float _radiusY, float _angleFrom, float _angleTo );
        void close();
        void polyline( const Polygon & _polygon, bool _closed );
        void styledPolyline( const VectorGraphicsPathPoints & _points, bool _closed );
        void catmullRom( const Polygon & _polygon, bool _closed );
        void styledCatmullRom( const VectorGraphicsPathPoints & _points, bool _closed );

    public:
        float getLength() const;
        mt::box2f getBounds() const;
        GraphicsPathSample sampleNormalized( float _ratio ) const;
        GraphicsPathSample sampleDistance( float _distance ) const;

    public:
        gp_path_t * getPath() const;

    protected:
        GraphicsPathSample sample_( float _value, gp_path_sample_unit_t _unit ) const;

    protected:
        gp_path_t * m_path;
    };
    //////////////////////////////////////////////////////////////////////////
}
