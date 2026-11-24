#include "Graphics.h"

#include <cstddef>

#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/RenderTextureServiceInterface.h"

#include "GraphicsGradient.h"

#include "Kernel/BezierHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMath.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Materialable.h"
#include "Kernel/Logger.h"
#include "Kernel/RenderHelper.h"
#include "Kernel/FactorableUnique.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdUtility.h"

#include "math/line2.h"

//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DEBUG)
#   define GP_CALL(m, args) if( m args == GP_FAILURE ) MENGINE_ASSERTION_FATAL(false, #m #args)
#else
#   define GP_CALL(m, args) m args
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Graphics::Graphics()
        : m_canvas( nullptr )
        , m_uvRect( 0.f, 0.f, 1.f, 1.f )
        , m_invalidateLocalVertex2D( false )
    {
        mt::box2_reset( &m_renderBoundingBox, 0.f, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    Graphics::~Graphics()
    {
        GP_CALL( gp_canvas_destroy, (m_canvas) );
        m_canvas = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::createCanvas( gp_graphics_t * _graphics )
    {
        GP_CALL( gp_canvas_create, (_graphics, &m_canvas) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool Graphics::_compile()
    {
        RenderMaterialInterfacePtr material;

        if( m_resourceImage != nullptr )
        {
            if( m_resourceImage->compile() == false )
            {
                const ConstString & name = this->getName();
                const ConstString & resourceName = m_resourceImage->getName();
                const Char * nameStr = name.c_str();
                const Char * resourceNameStr = resourceName.c_str();

                LOGGER_ERROR( "graphics '%s' image resource '%s' not compile"
                    , nameStr
                    , resourceNameStr
                );

                return false;
            }

            material = Helper::makeImageMaterial( m_resourceImage, ConstString::none(), EMB_NORMAL, false, false, MENGINE_DOCUMENT_FACTORABLE );
        }
        else
        {
            material = RENDERMATERIAL_SERVICE()
                ->getMaterial3( EM_COLOR_BLEND, PT_TRIANGLELIST, nullptr, 0, MENGINE_DOCUMENT_FACTORABLE );
        }

        MENGINE_ASSERTION_MEMORY_PANIC( material, "invalid get material" );

        m_material = material;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::_release()
    {
        if( m_resourceImage != nullptr )
        {
            m_resourceImage->release();
        }

        m_material = nullptr;
        m_gradientMaterials.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::_dispose()
    {
        m_resourceImage = nullptr;

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setResourceImage( const ResourceImagePtr & _resourceImage )
    {
        if( m_resourceImage == _resourceImage )
        {
            return;
        }

        this->recompile( [this, &_resourceImage]()
        {
            m_resourceImage = _resourceImage;

            return true;
        } );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & Graphics::getResourceImage() const
    {
        return m_resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setUVRect( const mt::vec4f & _uvRect )
    {
        m_uvRect = _uvRect;
        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec4f & Graphics::getUVRect() const
    {
        return m_uvRect;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setLineWidth( float _width )
    {
        GP_CALL( gp_set_thickness, (m_canvas, _width) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    float Graphics::getLineWidth() const
    {
        float lineWidth;
        GP_CALL( gp_get_thickness, (m_canvas, &lineWidth) );

        return lineWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setOutlineWidth( float _outlineWidth )
    {
        GP_CALL( gp_set_outline_width, (m_canvas, _outlineWidth) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    float Graphics::getOutlineWidth() const
    {
        float outlineWidth;
        GP_CALL( gp_get_outline_width, (m_canvas, &outlineWidth) );

        return outlineWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setOutlineColor( const Color & _color )
    {
        float r = _color.getR();
        float g = _color.getG();
        float b = _color.getB();
        float a = _color.getA();

        GP_CALL( gp_set_outline_color, (m_canvas, r, g, b, a) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    Color Graphics::getOutlineColor() const
    {
        gp_color_t c;
        GP_CALL( gp_get_outline_color, (m_canvas, &c) );

        return Color( c.r, c.g, c.b, c.a );
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setLineSoft( float _penumbra )
    {
        GP_CALL( gp_set_penumbra, (m_canvas, _penumbra) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    float Graphics::getLineSoft() const
    {
        float linePenumbra;
        GP_CALL( gp_get_penumbra, (m_canvas, &linePenumbra) );

        return linePenumbra;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setLineColor( const Color & _color )
    {
        float r = _color.getR();
        float g = _color.getG();
        float b = _color.getB();
        float a = _color.getA();

        GP_CALL( gp_set_color, (m_canvas, r, g, b, a) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    Color Graphics::getLineColor() const
    {
        gp_color_t c;
        GP_CALL( gp_get_color, (m_canvas, &c) );
        
        return Color( c.r, c.g, c.b, c.a );
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setLineCap( EGraphicsLineCap _cap )
    {
        GP_CALL( gp_set_line_cap, (m_canvas, static_cast<gp_line_cap_t>(_cap)) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    EGraphicsLineCap Graphics::getLineCap() const
    {
        gp_line_cap_t cap;
        GP_CALL( gp_get_line_cap, (m_canvas, &cap) );

        return static_cast<EGraphicsLineCap>(cap);
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setLineJoin( EGraphicsLineJoin _join )
    {
        GP_CALL( gp_set_line_join, (m_canvas, static_cast<gp_line_join_t>(_join)) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    EGraphicsLineJoin Graphics::getLineJoin() const
    {
        gp_line_join_t join;
        GP_CALL( gp_get_line_join, (m_canvas, &join) );

        return static_cast<EGraphicsLineJoin>(join);
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setMiterLimit( float _limit )
    {
        GP_CALL( gp_set_miter_limit, (m_canvas, _limit) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    float Graphics::getMiterLimit() const
    {
        float limit;
        GP_CALL( gp_get_miter_limit, (m_canvas, &limit) );

        return limit;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setFillRule( EGraphicsFillRule _rule )
    {
        GP_CALL( gp_set_fill_rule, (m_canvas, static_cast<gp_fill_rule_t>(_rule)) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    EGraphicsFillRule Graphics::getFillRule() const
    {
        gp_fill_rule_t rule;
        GP_CALL( gp_get_fill_rule, (m_canvas, &rule) );

        return static_cast<EGraphicsFillRule>(rule);
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setLineDash( const Vector<float> & _pattern, float _offset )
    {
        GP_CALL( gp_set_line_dash, (m_canvas, _pattern.data(), (gp_uint8_t)_pattern.size(), _offset) );
        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::clearLineDash()
    {
        GP_CALL( gp_clear_line_dash, (m_canvas) );
        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setPathTrim( float _from, float _to )
    {
        GP_CALL( gp_set_path_trim, (m_canvas, _from, _to) );
        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setPathMarkers( const GraphicsPathMarker & _start, const GraphicsPathMarker & _end )
    {
        gp_path_marker_t start = {static_cast<gp_path_marker_type_t>(_start.type), _start.length, _start.width, _start.inset};
        gp_path_marker_t end = {static_cast<gp_path_marker_type_t>(_end.type), _end.length, _end.width, _end.inset};
        GP_CALL( gp_set_path_markers, (m_canvas, &start, &end) );
        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setStrokeAlignment( EGraphicsStrokeAlignment _alignment )
    {
        GP_CALL( gp_set_stroke_alignment, (m_canvas, static_cast<gp_stroke_alignment_t>(_alignment)) );
        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    EGraphicsStrokeAlignment Graphics::getStrokeAlignment() const
    {
        gp_stroke_alignment_t alignment;
        GP_CALL( gp_get_stroke_alignment, (m_canvas, &alignment) );
        return static_cast<EGraphicsStrokeAlignment>(alignment);
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setUVMode( EGraphicsUVMode _mode )
    {
        GP_CALL( gp_set_uv_mode, (m_canvas, static_cast<gp_uv_mode_t>(_mode)) );
        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    EGraphicsUVMode Graphics::getUVMode() const
    {
        gp_uv_mode_t mode;
        GP_CALL( gp_get_uv_mode, (m_canvas, &mode) );
        return static_cast<EGraphicsUVMode>(mode);
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setCurveQuality( uint8_t _quality )
    {
        GP_CALL( gp_set_curve_quality, (m_canvas, _quality) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint8_t Graphics::getCurveQuality() const
    {
        gp_uint8_t quality;
        GP_CALL( gp_get_curve_quality, (m_canvas, &quality) );

        return quality;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setEllipseQuality( uint8_t _quality )
    {
        GP_CALL( gp_set_ellipse_quality, (m_canvas, _quality) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint8_t Graphics::getEllipseQuality() const
    {
        gp_uint8_t quality;
        GP_CALL( gp_get_ellipse_quality, (m_canvas, &quality) );
        
        return quality;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::setRectQuality( uint8_t _quality )
    {
        GP_CALL( gp_set_rect_quality, (m_canvas, _quality) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint8_t Graphics::getRectQuality() const
    {
        gp_uint8_t quality;
        GP_CALL( gp_get_rect_quality, (m_canvas, &quality) );

        return quality;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::pushState()
    {
        GP_CALL( gp_push_state, (m_canvas) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::popState()
    {
        GP_CALL( gp_pop_state, (m_canvas) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::beginFill()
    {
        GP_CALL( gp_begin_fill, (m_canvas) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::endFill()
    {
        GP_CALL( gp_end_fill, (m_canvas) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Graphics::setLinearGradient( const mt::vec2f & _from, const mt::vec2f & _to, const VectorGraphicsGradientStops & _stops )
    {
        mt::vec2f axis = _to - _from;
        float lengthSquared = mt::dot_v2_v2( axis, axis );

        MENGINE_ASSERTION_ISINFINITE( lengthSquared, "invalid linear gradient length squared" );

        if( lengthSquared <= 0.f )
        {
            return false;
        }

        mt::vec2f scale = axis / lengthSquared;

        MENGINE_ASSERTION_ISINFINITE( scale.x, "invalid linear gradient horizontal scale" );
        MENGINE_ASSERTION_ISINFINITE( scale.y, "invalid linear gradient vertical scale" );

        GradientDesc gradient;
        gradient.radial = false;
        gradient.origin = _from;
        gradient.coordinateScale = scale;

        if( this->setFillGradient_( StdUtility::move( gradient ), _stops ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Graphics::setRadialGradient( const mt::vec2f & _center, float _radius, const VectorGraphicsGradientStops & _stops )
    {
        MENGINE_ASSERTION_ISINFINITE( _center.x, "invalid radial gradient center x" );
        MENGINE_ASSERTION_ISINFINITE( _center.y, "invalid radial gradient center y" );
        MENGINE_ASSERTION_ISINFINITE( _radius, "invalid radial gradient radius" );

        if( _radius <= 0.f )
        {
            return false;
        }

        float inverseRadius = 1.f / _radius;

        MENGINE_ASSERTION_ISINFINITE( inverseRadius, "invalid radial gradient inverse radius" );

        GradientDesc gradient;
        gradient.radial = true;
        gradient.origin = _center;
        gradient.coordinateScale.set( inverseRadius, inverseRadius );

        if( this->setFillGradient_( StdUtility::move( gradient ), _stops ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Graphics::setFillGradient_( GradientDesc _gradient, const VectorGraphicsGradientStops & _stops )
    {
        if( _stops.empty() == true )
        {
            return false;
        }

        _gradient.stops.reserve( _stops.size() );
        float previousOffset = 0.f;

        for( const GraphicsGradientStopDesc & stop : _stops )
        {
            MENGINE_ASSERTION_ISINFINITE( stop.offset, "invalid gradient stop offset" );

            if( stop.offset < previousOffset )
            {
                return false;
            }

            if( stop.offset > 1.f )
            {
                return false;
            }

            float r = stop.color.getR();
            float g = stop.color.getG();
            float b = stop.color.getB();
            float a = stop.color.getA();

            MENGINE_ASSERTION_ISINFINITE( r, "invalid gradient red component" );
            MENGINE_ASSERTION_ISINFINITE( g, "invalid gradient green component" );
            MENGINE_ASSERTION_ISINFINITE( b, "invalid gradient blue component" );
            MENGINE_ASSERTION_ISINFINITE( a, "invalid gradient alpha component" );

            float clampedR = StdAlgorithm::clamp( r, 0.f, 1.f );
            float clampedG = StdAlgorithm::clamp( g, 0.f, 1.f );
            float clampedB = StdAlgorithm::clamp( b, 0.f, 1.f );
            float clampedA = StdAlgorithm::clamp( a, 0.f, 1.f );

            Color color( clampedR, clampedG, clampedB, clampedA );
            _gradient.stops.emplace_back( GraphicsGradientStopDesc{stop.offset, color} );
            previousOffset = stop.offset;
        }

        for( size_t index = 0; index != m_gradients.size(); ++index )
        {
            const GradientDesc & paint = m_gradients[index];

            if( paint.radial != _gradient.radial )
            {
                continue;
            }

            if( paint.origin != _gradient.origin )
            {
                continue;
            }

            if( paint.coordinateScale != _gradient.coordinateScale )
            {
                continue;
            }

            if( paint.stops.size() != _gradient.stops.size() )
            {
                continue;
            }

            bool equal = StdAlgorithm::equal( paint.stops.begin(), paint.stops.end(), _gradient.stops.begin(), []( const GraphicsGradientStopDesc & _left, const GraphicsGradientStopDesc & _right )
            {
                if( _left.offset != _right.offset )
                {
                    return false;
                }

                if( _left.color.isEqualF( _right.color ) == false )
                {
                    return false;
                }

                return true;
            } );

            if( equal == true )
            {
                uint32_t key = (uint32_t)index + 1;
                GP_CALL( gp_set_fill_key, (m_canvas, key) );

                return true;
            }
        }

        m_gradients.emplace_back( StdUtility::move( _gradient ) );
        uint32_t key = (uint32_t)m_gradients.size();
        GP_CALL( gp_set_fill_key, (m_canvas, key) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::clearFillGradient()
    {
        GP_CALL( gp_set_fill_key, (m_canvas, 0) );
    }
    //////////////////////////////////////////////////////////////////////////
    GraphicsPathPtr Graphics::createPath()
    {
        GraphicsPathPtr path = Helper::makeFactorableUnique<GraphicsPath>( MENGINE_DOCUMENT_FACTORABLE, m_canvas );

        return path;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawPath( const GraphicsPathPtr & _path )
    {
        GP_CALL( gp_draw_path, (m_canvas, _path->getPath()) );
        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawRect( const mt::vec2f & _point, float _width, float _height )
    {
        GP_CALL( gp_rect, (m_canvas, _point.x, _point.y, _width, _height) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawRoundedRect( const mt::vec2f & _point, float _width, float _height, float _radius )
    {
        GP_CALL( gp_rounded_rect, (m_canvas, _point.x, _point.y, _width, _height, _radius) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawRoundedRectVarying( const mt::vec2f & _point, float _width, float _height, const mt::vec4f & _radii )
    {
        GP_CALL( gp_rounded_rect_varying, (m_canvas, _point.x, _point.y, _width, _height, _radii.x, _radii.y, _radii.z, _radii.w) );
        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawNineSlice( const mt::vec2f & _point, float _width, float _height, const mt::vec2f & _sourceSize, const mt::vec4f & _uv, const mt::vec4f & _insets, EGraphicsNineSliceMode _edgeMode, EGraphicsNineSliceMode _centerMode )
    {
        GP_CALL( gp_nine_slice, (m_canvas, _point.x, _point.y, _width, _height, _sourceSize.x, _sourceSize.y, _uv.x, _uv.y, _uv.z, _uv.w, _insets.x, _insets.y, _insets.z, _insets.w, static_cast<gp_nine_slice_mode_t>(_edgeMode), static_cast<gp_nine_slice_mode_t>(_centerMode)) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawRoundedPolygon( const Polygon & _polygon, float _radius )
    {
        const VectorPoints & points = _polygon.getPoints();

        GP_CALL( gp_rounded_polygon, (m_canvas, points.data(), points.size(), sizeof( VectorPoints::value_type ), _radius) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawPolyline( const Polygon & _polygon, bool _closed )
    {
        const VectorPoints & points = _polygon.getPoints();

        GP_CALL( gp_polyline, (m_canvas, points.data(), points.size(), sizeof( VectorPoints::value_type ), GP_NULLPTR, _closed == true ? GP_TRUE : GP_FALSE) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawStyledPolyline( const VectorGraphicsPathPoints & _points, bool _closed )
    {
        static_assert( offsetof( GraphicsPathPoint, position ) == 0, "GraphicsPathPoint position must be first" );
        static_assert( sizeof( mt::vec2f ) == sizeof( float ) * 2, "GraphicsPathPoint position must contain two floats" );

        gp_path_layout_t layout;
        layout.width_offset = offsetof( GraphicsPathPoint, width );
        layout.color_offset = offsetof( GraphicsPathPoint, color );
        layout.outline_color_offset = offsetof( GraphicsPathPoint, outlineColor );

        GP_CALL( gp_polyline, (m_canvas, _points.data(), _points.size(), sizeof( VectorGraphicsPathPoints::value_type ), &layout, _closed == true ? GP_TRUE : GP_FALSE) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawRoundedPolyline( const VectorGraphicsPathPoints & _points, float _radius, bool _closed )
    {
        gp_path_layout_t layout;
        layout.width_offset = offsetof( GraphicsPathPoint, width );
        layout.color_offset = offsetof( GraphicsPathPoint, color );
        layout.outline_color_offset = offsetof( GraphicsPathPoint, outlineColor );

        GP_CALL( gp_rounded_polyline, (m_canvas, _points.data(), _points.size(), sizeof( VectorGraphicsPathPoints::value_type ), &layout, _radius, _closed == true ? GP_TRUE : GP_FALSE) );
        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawPolygonOffset( const Polygon & _polygon, float _offset, EGraphicsLineJoin _join )
    {
        const VectorPoints & points = _polygon.getPoints();
        GP_CALL( gp_polygon_offset, (m_canvas, points.data(), points.size(), sizeof( VectorPoints::value_type ), _offset, static_cast<gp_line_join_t>(_join)) );
        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawCircle( const mt::vec2f & _point, float _radius )
    {
        GP_CALL( gp_circle, (m_canvas, _point.x, _point.y, _radius) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawEllipse( const mt::vec2f & _point, float _width, float _height )
    {
        GP_CALL( gp_ellipse, (m_canvas, _point.x, _point.y, _width, _height) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawArc( const mt::vec2f & _point, float _radius, float _angleFrom, float _angleTo )
    {
        GP_CALL( gp_arc, (m_canvas, _point.x, _point.y, _radius, _angleFrom, _angleTo) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawEllipseArc( const mt::vec2f & _point, float _width, float _height, float _angleFrom, float _angleTo )
    {
        GP_CALL( gp_ellipse_arc, (m_canvas, _point.x, _point.y, _width, _height, _angleFrom, _angleTo) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawRing( const mt::vec2f & _point, float _innerRadius, float _outerRadius, float _angleFrom, float _angleTo )
    {
        GP_CALL( gp_ring, (m_canvas, _point.x, _point.y, _innerRadius, _outerRadius, _angleFrom, _angleTo) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawRoundedRing( const mt::vec2f & _point, float _innerRadius, float _outerRadius, float _angleFrom, float _angleTo )
    {
        GP_CALL( gp_rounded_ring, (m_canvas, _point.x, _point.y, _innerRadius, _outerRadius, _angleFrom, _angleTo) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawCapsule( const mt::vec2f & _point, float _width, float _height )
    {
        GP_CALL( gp_capsule, (m_canvas, _point.x, _point.y, _width, _height) );
        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawRegularPolygon( const mt::vec2f & _point, float _radius, uint32_t _vertexCount, float _rotation )
    {
        GP_CALL( gp_regular_polygon, (m_canvas, _point.x, _point.y, _radius, _vertexCount, _rotation) );
        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawStar( const mt::vec2f & _point, float _innerRadius, float _outerRadius, uint32_t _rayCount, float _rotation )
    {
        GP_CALL( gp_star, (m_canvas, _point.x, _point.y, _innerRadius, _outerRadius, _rayCount, _rotation) );
        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawSuperellipse( const mt::vec2f & _point, float _width, float _height, float _exponent, float _rotation )
    {
        GP_CALL( gp_superellipse, (m_canvas, _point.x, _point.y, _width, _height, _exponent, _rotation) );
        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::box2f Graphics::getBounds() const
    {
        if( m_invalidateLocalVertex2D == true )
        {
            this->updateLocalVertex2D_();
        }

        return m_renderBoundingBox;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::clear()
    {
        GP_CALL( gp_canvas_clear, (m_canvas) );

        m_renderVertex2D.clear();
        m_renderIndices.clear();
        m_renderBatches.clear();
        m_renderBatchBoundingBoxes.clear();
        m_gradients.clear();

        for( GradientMaterialDesc & gradient : m_gradientMaterials )
        {
            gradient.generation = 0;
        }

        mt::box2_reset( &m_renderBoundingBox, 0.f, 0.f );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        if( m_invalidateLocalVertex2D == true )
        {
            this->updateLocalVertex2D_();
        }

        if( m_renderIndices.empty() == true )
        {
            return;
        }

        for( size_t index = 0; index != m_renderBatches.size(); ++index )
        {
            const gp_render_batch_t & batch = m_renderBatches[index];
            const RenderMaterialInterfacePtr & material = batch.fill_key == 0 ? m_material : this->getGradientMaterial_( batch.fill_key );

            if( material == nullptr )
            {
                continue;
            }

            const RenderVertex2D * vertexData = m_renderVertex2D.data() + batch.vertex_offset;
            const RenderIndex * indexData = m_renderIndices.data() + batch.index_offset;

            _renderPipeline->addRenderObject( _context, material, nullptr, vertexData, batch.vertex_count, indexData, batch.index_count, &m_renderBatchBoundingBoxes[index], EROF_NONE, MENGINE_DOCUMENT_FORWARD );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderMaterialInterfacePtr & Graphics::getGradientMaterial_( uint32_t _key ) const
    {
        if( m_gradientMaterials.size() < _key )
        {
            m_gradientMaterials.resize( _key );
        }

        size_t index = _key - 1;
        const GradientDesc & gradient = m_gradients[index];
        GradientMaterialDesc & cache = m_gradientMaterials[index];

        if( cache.texture == nullptr )
        {
            cache.texture = RENDERTEXTURE_SERVICE()
                ->createTexture( 1, Helper::GRAPHICS_GRADIENT_RAMP_WIDTH, 1, PF_A8R8G8B8, DF_IMAGE_NONE, MENGINE_DOCUMENT_FACTORABLE );

            if( cache.texture == nullptr )
            {
                LOGGER_ERROR( "invalid create Graphics gradient texture" );

                return RenderMaterialInterfacePtr::none();
            }
        }

        uint64_t generation = GRAPHICS_SERVICE()
            ->getGradientTextureGeneration();

        if( cache.generation != generation )
        {
            if( Helper::uploadGraphicsGradientRamp( cache.texture, gradient.stops ) == false )
            {
                LOGGER_ERROR( "invalid upload Graphics gradient texture" );

                return RenderMaterialInterfacePtr::none();
            }

            cache.generation = generation;
        }

        if( cache.material == nullptr || cache.radial != gradient.radial )
        {
            const RenderMaterialStage * stage = GRAPHICS_SERVICE()
                ->getGradientMaterialStage( gradient.radial );

            if( stage == nullptr )
            {
                return RenderMaterialInterfacePtr::none();
            }

            ConstString name = STRINGIZE_STRING_LOCAL( "GraphicsGradient" );
            cache.material = RENDERMATERIAL_SERVICE()
                ->getMaterial2( name, stage, PT_TRIANGLELIST, &cache.texture, 1, MENGINE_DOCUMENT_FACTORABLE );
            cache.radial = gradient.radial;
        }

        return cache.material;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::_invalidateWorldMatrix() const
    {
        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::_invalidateColor() const
    {
        BaseRender::_invalidateColor();

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::updateVertexUVs_() const
    {
        for( const gp_render_batch_t & batch : m_renderBatches )
        {
            RenderVertex2D * vertices = m_renderVertex2D.data() + batch.vertex_offset;

            if( batch.fill_key != 0 )
            {
                const GradientDesc & gradient = m_gradients[batch.fill_key - 1];
                for( uint32_t index = 0; index != batch.vertex_count; ++index )
                {
                    RenderVertex2D & vertex = vertices[index];
                    mt::vec2f point( vertex.position.x - gradient.origin.x, vertex.position.y - gradient.origin.y );

                    if( gradient.radial == true )
                    {
                        vertex.uv[0] = point * gradient.coordinateScale;
                    }
                    else
                    {
                        float offset = mt::dot_v2_v2( point, gradient.coordinateScale );
                        vertex.uv[0].set( offset, 0.5f );
                    }
                }
            }
            else if( m_resourceImage != nullptr )
            {
                for( uint32_t index = 0; index != batch.vertex_count; ++index )
                {
                    RenderVertex2D & vertex = vertices[index];
                    mt::vec2f uv(
                        m_uvRect.x + vertex.uv[0].x * (m_uvRect.z - m_uvRect.x),
                        m_uvRect.y + vertex.uv[0].y * (m_uvRect.w - m_uvRect.y)
                    );

                    m_resourceImage->correctUV( 0, uv, &vertex.uv[0] );
                    m_resourceImage->correctUV( 1, uv, &vertex.uv[1] );
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::updateLocalVertex2D_() const
    {
        m_invalidateLocalVertex2D = false;

        Color color;
        this->calcTotalColor( &color );

        if( m_resourceImage != nullptr )
        {
            color *= m_resourceImage->getColor();
        }

        gp_mesh_t mesh;
        GP_CALL( gp_calculate_mesh_size, (m_canvas, &mesh) );

        m_renderVertex2D.resize( mesh.vertex_count );
        m_renderIndices.resize( mesh.index_count );
        m_renderBatches.resize( mesh.batch_count );
        m_renderBatchBoundingBoxes.resize( mesh.batch_count );

        mesh.color.r = color.getR();
        mesh.color.g = color.getG();
        mesh.color.b = color.getB();
        mesh.color.a = color.getA();

        VectorRenderVertex2D::value_type * vertices = m_renderVertex2D.data();

        mesh.positions_buffer = vertices;
        mesh.positions_offset = MENGINE_OFFSETOF( VectorRenderVertex2D::value_type, position );
        mesh.positions_stride = sizeof( VectorRenderVertex2D::value_type );

        mesh.colors_buffer = vertices;
        mesh.colors_offset = MENGINE_OFFSETOF( VectorRenderVertex2D::value_type, color );
        mesh.colors_stride = sizeof( VectorRenderVertex2D::value_type );

        mesh.uv_buffer = vertices;
        mesh.uv_offset = MENGINE_OFFSETOF( VectorRenderVertex2D::value_type, uv[0] );
        mesh.uv_stride = sizeof( VectorRenderVertex2D::value_type );

        VectorRenderIndex::value_type * indices = m_renderIndices.data();

        mesh.indices_buffer = indices;
        mesh.indices_offset = 0;
        mesh.indices_stride = sizeof( VectorRenderIndex::value_type );

        mesh.batches_buffer = m_renderBatches.data();
        mesh.batches_offset = 0;
        mesh.batches_stride = sizeof( gp_render_batch_t );

        GP_CALL( gp_render, (m_canvas, &mesh) );

        if( m_resourceImage != nullptr || m_gradients.empty() == false )
        {
            this->updateVertexUVs_();
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        for( RenderVertex2D & vertex : m_renderVertex2D )
        {
#if defined(MENGINE_RENDER_COLOR_RGBA)
            // graphics packs colors as 0xAARRGGBB. RGBA render backends read
            // the uint32 bytes as R, G, B, A, so swap red and blue before the
            // vertex buffer is submitted.
            uint32_t argb = vertex.color;
            vertex.color = (argb & 0xFF00FF00U)
                | ((argb & 0x00FF0000U) >> 16U)
                | ((argb & 0x000000FFU) << 16U);
#endif

            mt::vec3f pwm;
            mt::mul_v3_v3_m4( &pwm, vertex.position, wm );

            vertex.position = pwm;
        }

        Helper::makeRenderBoundingBox( &m_renderBoundingBox, m_renderVertex2D.data(), (uint32_t)m_renderVertex2D.size() );

        for( size_t batchIndex = 0; batchIndex != m_renderBatches.size(); ++batchIndex )
        {
            const gp_render_batch_t & batch = m_renderBatches[batchIndex];
            Helper::makeRenderBoundingBox( &m_renderBatchBoundingBoxes[batchIndex]
                , m_renderVertex2D.data() + batch.vertex_offset, batch.vertex_count );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
