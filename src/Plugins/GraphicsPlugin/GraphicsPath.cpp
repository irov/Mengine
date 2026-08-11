#include "GraphicsPath.h"

#include <cstddef>

#include "Kernel/Assertion.h"
#include "Kernel/MemoryAllocator.h"

//////////////////////////////////////////////////////////////////////////
static void * gp_path_malloc( gp_size_t _size, void * _ud )
{
    MENGINE_UNUSED( _ud );
    return Mengine::Helper::allocateMemory( _size, "gp" );
}
//////////////////////////////////////////////////////////////////////////
static void * gp_path_realloc( void * _ptr, gp_size_t _size, void * _ud )
{
    MENGINE_UNUSED( _ud );
    return Mengine::Helper::reallocateMemory( _ptr, _size, "gp" );
}
//////////////////////////////////////////////////////////////////////////
static void gp_path_free( void * _ptr, void * _ud )
{
    MENGINE_UNUSED( _ud );
    Mengine::Helper::deallocateMemory( _ptr, "gp" );
}
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DEBUG)
#   define GP_PATH_CALL(m, args) if( m args == GP_FAILURE ) MENGINE_ASSERTION_FATAL(false, #m #args)
#else
#   define GP_PATH_CALL(m, args) m args
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GraphicsPath::GraphicsPath( uint8_t _curveQuality, uint8_t _ellipseQuality )
        : m_path( nullptr )
    {
        GP_PATH_CALL( gp_path_create, (&m_path, &gp_path_malloc, &gp_path_realloc, &gp_path_free, nullptr,
            _curveQuality, _ellipseQuality) );
    }
    //////////////////////////////////////////////////////////////////////////
    GraphicsPath::~GraphicsPath()
    {
        GP_PATH_CALL( gp_path_destroy, (m_path) );
        m_path = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPath::setStyle( const GraphicsPathStyle & _style )
    {
        gp_path_style_t style;
        style.attributes = _style.attributes;
        style.width = _style.width;
        style.color = {_style.color.getR(), _style.color.getG(), _style.color.getB(), _style.color.getA()};
        style.outline_color = {_style.outlineColor.getR(), _style.outlineColor.getG(), _style.outlineColor.getB(), _style.outlineColor.getA()};
        GP_PATH_CALL( gp_path_set_style, (m_path, &style) );
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPath::clearStyle()
    {
        GP_PATH_CALL( gp_path_set_style, (m_path, GP_NULLPTR) );
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPath::clear()
    {
        GP_PATH_CALL( gp_path_clear, (m_path) );
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPath::moveTo( const mt::vec2f & _point )
    {
        GP_PATH_CALL( gp_path_move_to, (m_path, _point.x, _point.y) );
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPath::lineTo( const mt::vec2f & _point )
    {
        GP_PATH_CALL( gp_path_line_to, (m_path, _point.x, _point.y) );
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPath::quadraticCurveTo( const mt::vec2f & _control, const mt::vec2f & _point )
    {
        GP_PATH_CALL( gp_path_quadratic_curve_to, (m_path, _control.x, _control.y, _point.x, _point.y) );
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPath::bezierCurveTo( const mt::vec2f & _control0, const mt::vec2f & _control1, const mt::vec2f & _point )
    {
        GP_PATH_CALL( gp_path_bezier_curve_to, (m_path, _control0.x, _control0.y, _control1.x, _control1.y, _point.x, _point.y) );
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPath::tangentArcTo( const mt::vec2f & _point0, const mt::vec2f & _point1, float _radius )
    {
        GP_PATH_CALL( gp_path_tangent_arc_to, (m_path, _point0.x, _point0.y, _point1.x, _point1.y, _radius) );
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPath::ellipseArc( const mt::vec2f & _center, float _radiusX, float _radiusY, float _angleFrom, float _angleTo )
    {
        GP_PATH_CALL( gp_path_ellipse_arc, (m_path, _center.x, _center.y, _radiusX, _radiusY, _angleFrom, _angleTo) );
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPath::close()
    {
        GP_PATH_CALL( gp_path_close, (m_path) );
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPath::polyline( const Polygon & _polygon, bool _closed )
    {
        const VectorPoints & points = _polygon.getPoints();
        GP_PATH_CALL( gp_path_polyline, (m_path, points.data(), points.size(), sizeof( VectorPoints::value_type ), GP_NULLPTR, _closed == true ? GP_TRUE : GP_FALSE) );
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPath::styledPolyline( const VectorGraphicsPathPoints & _points, bool _closed )
    {
        gp_path_layout_t layout = {offsetof( GraphicsPathPoint, width ), offsetof( GraphicsPathPoint, color ), offsetof( GraphicsPathPoint, outlineColor )};
        GP_PATH_CALL( gp_path_polyline, (m_path, _points.data(), _points.size(), sizeof( VectorGraphicsPathPoints::value_type ), &layout, _closed == true ? GP_TRUE : GP_FALSE) );
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPath::catmullRom( const Polygon & _polygon, bool _closed )
    {
        const VectorPoints & points = _polygon.getPoints();
        GP_PATH_CALL( gp_path_catmull_rom, (m_path, points.data(), points.size(), sizeof( VectorPoints::value_type ), GP_NULLPTR, _closed == true ? GP_TRUE : GP_FALSE) );
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsPath::styledCatmullRom( const VectorGraphicsPathPoints & _points, bool _closed )
    {
        gp_path_layout_t layout = {offsetof( GraphicsPathPoint, width ), offsetof( GraphicsPathPoint, color ), offsetof( GraphicsPathPoint, outlineColor )};
        GP_PATH_CALL( gp_path_catmull_rom, (m_path, _points.data(), _points.size(), sizeof( VectorGraphicsPathPoints::value_type ), &layout, _closed == true ? GP_TRUE : GP_FALSE) );
    }
    //////////////////////////////////////////////////////////////////////////
    float GraphicsPath::getLength() const
    {
        float length;
        GP_PATH_CALL( gp_path_get_length, (m_path, &length) );
        return length;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::box2f GraphicsPath::getBounds() const
    {
        gp_bounds_t bounds;
        GP_PATH_CALL( gp_path_get_bounds, (m_path, &bounds) );
        return mt::box2f( bounds.minimum_x, bounds.minimum_y, bounds.maximum_x, bounds.maximum_y );
    }
    //////////////////////////////////////////////////////////////////////////
    GraphicsPathSample GraphicsPath::sampleNormalized( float _ratio ) const
    {
        return this->sample_( _ratio, GP_PATH_SAMPLE_NORMALIZED );
    }
    //////////////////////////////////////////////////////////////////////////
    GraphicsPathSample GraphicsPath::sampleDistance( float _distance ) const
    {
        return this->sample_( _distance, GP_PATH_SAMPLE_DISTANCE );
    }
    //////////////////////////////////////////////////////////////////////////
    GraphicsPathSample GraphicsPath::sample_( float _value, gp_path_sample_unit_t _unit ) const
    {
        gp_path_sample_t sample;
        GP_PATH_CALL( gp_path_sample, (m_path, _value, _unit, &sample) );
        GraphicsPathSample result;
        result.position = mt::vec2f( sample.x, sample.y );
        result.tangent = mt::vec2f( sample.tangent_x, sample.tangent_y );
        result.distance = sample.distance;
        result.ratio = sample.ratio;
        result.contourIndex = sample.contour_index;
        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    gp_path_t * GraphicsPath::getPath() const
    {
        return m_path;
    }
    //////////////////////////////////////////////////////////////////////////
}
