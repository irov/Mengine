#include "Graphics.h"

#include "Interface/RenderMaterialServiceInterface.h"

#include "Kernel/BezierHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/MemoryAllocator.h"
#include "Kernel/RenderHelper.h"

#include "Config/StdAlgorithm.h"

#include "math/line2.h"

//////////////////////////////////////////////////////////////////////////
static void * gp_malloc( gp_size_t _size, void * _ud )
{
    MENGINE_UNUSED( _ud );

    void * p = Mengine::Helper::allocateMemory( _size, "gp" );

    return p;
}
//////////////////////////////////////////////////////////////////////////
static void * gp_realloc( void * _ptr, gp_size_t _size, void * _ud )
{
    MENGINE_UNUSED( _ud );

    void * p = Mengine::Helper::reallocateMemory( _ptr, _size, "gp" );

    return p;
}
//////////////////////////////////////////////////////////////////////////
static void gp_free( void * _ptr, void * _ud )
{
    MENGINE_UNUSED( _ud );

    Mengine::Helper::deallocateMemory( _ptr, "gp" );
}
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
        , m_invalidateLocalVertex2D( false )
    {
        GP_CALL( gp_canvas_create, (&m_canvas, &gp_malloc, &gp_realloc, &gp_free, nullptr) );

        mt::box2_reset( &m_renderBoundingBox, 0.f, 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
    Graphics::~Graphics()
    {
        GP_CALL( gp_canvas_destroy, (m_canvas) );
        m_canvas = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Graphics::_compile()
    {
        RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
            ->getMaterial3( EM_COLOR_BLEND, PT_TRIANGLELIST, nullptr, 0, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( material, "invalid get material" );

        m_material = material;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::_release()
    {
        m_material = nullptr;
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
    void Graphics::beginCompoundFill( EGraphicsFillRule _rule )
    {
        GP_CALL( gp_begin_compound_fill, (m_canvas, static_cast<gp_fill_rule_t>(_rule)) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::endCompoundFill()
    {
        GP_CALL( gp_end_compound_fill, (m_canvas) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::pointMoveTo( const mt::vec2f & _point )
    {
        GP_CALL( gp_point_move_to, (m_canvas, _point.x, _point.y) );
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::pointLineTo( const mt::vec2f & _point )
    {
        GP_CALL( gp_point_line_to, (m_canvas, _point.x, _point.y) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::pointQuadraticCurveTo( const mt::vec2f & _p0, const mt::vec2f & _point )
    {
        GP_CALL( gp_point_quadratic_curve_to, (m_canvas, _p0.x, _p0.y, _point.x, _point.y) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::pointBezierCurveTo( const mt::vec2f & _p0, const mt::vec2f & _p1, const mt::vec2f & _point )
    {
        GP_CALL( gp_point_bezier_curve_to, (m_canvas, _p0.x, _p0.y, _p1.x, _p1.y, _point.x, _point.y) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::pointClose()
    {
        GP_CALL( gp_point_close, (m_canvas) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::pointArcTo( const mt::vec2f & _p0, const mt::vec2f & _p1, float _radius )
    {
        GP_CALL( gp_point_tangent_arc_to, (m_canvas, _p0.x, _p0.y, _p1.x, _p1.y, _radius) );

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

        GP_CALL( gp_polyline, (m_canvas, points.data(), points.size(), sizeof( VectorPoints::value_type ), _closed == true ? GP_TRUE : GP_FALSE) );

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
    void Graphics::drawSector( const mt::vec2f & _point, float _radius, float _angleFrom, float _angleTo )
    {
        GP_CALL( gp_sector, (m_canvas, _point.x, _point.y, _radius, _angleFrom, _angleTo) );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Graphics::drawRing( const mt::vec2f & _point, float _innerRadius, float _outerRadius, float _angleFrom, float _angleTo )
    {
        GP_CALL( gp_ring, (m_canvas, _point.x, _point.y, _innerRadius, _outerRadius, _angleFrom, _angleTo) );

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

        VectorRenderVertex2D::value_type * vertexData = m_renderVertex2D.data();
        VectorRenderVertex2D::size_type vertexSize = m_renderVertex2D.size();

        VectorRenderIndex::value_type * indexData = m_renderIndices.data();
        VectorRenderIndex::size_type indexSize = m_renderIndices.size();

        _renderPipeline->addRenderObject( _context, m_material, nullptr, vertexData, (uint32_t)vertexSize, indexData, (uint32_t)indexSize, &m_renderBoundingBox, false, MENGINE_DOCUMENT_FORWARD );
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
    void Graphics::updateLocalVertex2D_() const
    {
        m_invalidateLocalVertex2D = false;

        Color color;
        this->calcTotalColor( &color );

        gp_mesh_t mesh;
        GP_CALL( gp_calculate_mesh_size, (m_canvas, &mesh) );

        m_renderVertex2D.resize( mesh.vertex_count );
        m_renderIndices.resize( mesh.index_count );

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

        mesh.uv_buffer = GP_NULLPTR;
        mesh.uv_offset = 0;
        mesh.uv_stride = 0;

        VectorRenderIndex::value_type * indices = m_renderIndices.data();

        mesh.indices_buffer = indices;
        mesh.indices_offset = 0;
        mesh.indices_stride = sizeof( VectorRenderIndex::value_type );

        GP_CALL( gp_render, (m_canvas, &mesh) );

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
    }
    //////////////////////////////////////////////////////////////////////////
}
