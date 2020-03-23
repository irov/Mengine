#include "Vectorizator.h"

#include "Interface/RenderMaterialServiceInterface.h"

#include "Kernel/BezierHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/MemoryAllocator.h"

#include "math/line2.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
static void * gp_malloc( gp_size_t _size, void * _ud )
{
    MENGINE_UNUSED( _ud );

    return Mengine::Helper::allocateMemory( _size, "gp" );
}
//////////////////////////////////////////////////////////////////////////
static void gp_free( void * _ptr, void * _ud )
{
    MENGINE_UNUSED( _ud );

    return Mengine::Helper::deallocateMemory( _ptr, "gp" );
}
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Vectorizator::Vectorizator()
        : m_canvas( nullptr )
        , m_invalidateLocalVertex2D( false )
    {
        gp_canvas_create( &m_canvas, &gp_malloc, &gp_free, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    Vectorizator::~Vectorizator()
    {
        gp_canvas_destroy( m_canvas );
        m_canvas = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Vectorizator::_compile()
    {
        RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
            ->getMaterial3( EM_COLOR_BLEND, PT_TRIANGLELIST, 0, nullptr, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( material, false );

        m_material = material;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::_release()
    {
        m_material = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::setLineWidth( float _width )
    {
        gp_set_line_width( m_canvas, _width );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    float Vectorizator::getLineWidth() const
    {
        float lineWidth;
        gp_get_line_width( m_canvas, &lineWidth );

        return lineWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::setLineSoft( float _penumbra )
    {
        gp_set_penumbra( m_canvas, _penumbra );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    float Vectorizator::getLineSoft() const
    {
        float linePenumbra;
        gp_get_penumbra( m_canvas, &linePenumbra );

        return linePenumbra;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::setLineColor( const Color & _color )
    {
        gp_color_t c;
        c.r = _color.getR();
        c.g = _color.getG();
        c.b = _color.getB();
        c.a = _color.getA();

        gp_set_color( m_canvas, &c );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    Color Vectorizator::getLineColor() const
    {
        gp_color_t c;
        gp_get_color( m_canvas, &c );
        
        return Color( c.r, c.g, c.b, c.a );
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::setCurveQuality( uint8_t _quality )
    {
        gp_set_curve_quality( m_canvas, _quality );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint8_t Vectorizator::getCurveQuality() const
    {
        gp_uint8_t quality;
        gp_get_curve_quality( m_canvas, &quality );

        return quality;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::setEllipseQuality( uint8_t _quality )
    {
        gp_set_ellipse_quality( m_canvas, _quality );

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint8_t Vectorizator::getEllipseQuality() const
    {
        gp_uint8_t quality;
        gp_get_ellipse_quality( m_canvas, &quality );
        
        return quality;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::beginFill()
    {
        if( gp_begin_fill( m_canvas ) == GP_FAILURE )
        {
            MENGINE_ASSERTION_FATAL( false, "gp" );

            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::endFill()
    {
        if( gp_end_fill( m_canvas ) == GP_FAILURE )
        {
            MENGINE_ASSERTION_FATAL( false, "gp" );

            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::moveTo( const mt::vec2f & _point )
    {
        if( gp_move_to( m_canvas, _point.x, _point.y ) == GP_FAILURE )
        {
            MENGINE_ASSERTION_FATAL( false, "gp" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::lineTo( const mt::vec2f & _point )
    {
        if( gp_line_to( m_canvas, _point.x, _point.y ) == GP_FAILURE )
        {
            MENGINE_ASSERTION_FATAL( false, "gp" );
        }

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::quadraticCurveTo( const mt::vec2f & _p0, const mt::vec2f & _point )
    {
        if( gp_quadratic_curve_to( m_canvas, _p0.x, _p0.y, _point.x, _point.y ) == GP_FAILURE )
        {
            MENGINE_ASSERTION_FATAL( false, "gp" );
        }

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::bezierCurveTo( const mt::vec2f & _p0, const mt::vec2f & _p1, const mt::vec2f & _point )
    {
        if( gp_bezier_curve_to( m_canvas, _p0.x, _p0.y, _p1.x, _p1.y, _point.x, _point.y ) == GP_FAILURE )
        {
            MENGINE_ASSERTION_FATAL( false, "gp" );
        }

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::drawRect( const mt::vec2f & _point, float _width, float _height )
    {
        if( gp_draw_rect( m_canvas, _point.x, _point.y, _width, _height ) == GP_FAILURE )
        {
            MENGINE_ASSERTION_FATAL( false, "gp" );
        }

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::drawRoundedRect( const mt::vec2f & _point, float _width, float _height, float _radius )
    {
        if( gp_draw_rounded_rect( m_canvas, _point.x, _point.y, _width, _height, _radius ) == GP_FAILURE )
        {
            MENGINE_ASSERTION_FATAL( false, "gp" );

            return;
        }

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::drawCircle( const mt::vec2f & _point, float _radius )
    {
        if( gp_draw_circle( m_canvas, _point.x, _point.y, _radius ) == GP_FAILURE )
        {
            MENGINE_ASSERTION_FATAL( false, "gp" );
        }

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::drawEllipse( const mt::vec2f & _point, float _width, float _height )
    {
        if( gp_draw_ellipse( m_canvas, _point.x, _point.y, _width, _height ) == GP_FAILURE )
        {
            MENGINE_ASSERTION_FATAL( false, "gp" );
        }

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::clear()
    {
        gp_canvas_clear( m_canvas );

        m_renderVertex2D.clear();
        m_renderIndices.clear();

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
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

        _renderPipeline->addRenderObject( _context, m_material, nullptr, vertexData, vertexSize, indexData, indexSize, nullptr, false, MENGINE_DOCUMENT_FORWARD );
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::_invalidateWorldMatrix()
    {
        Node::_invalidateWorldMatrix();

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::_invalidateColor()
    {
        BaseRender::_invalidateColor();

        m_invalidateLocalVertex2D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Vectorizator::updateLocalVertex2D_() const
    {
        m_invalidateLocalVertex2D = false;

        Color color;
        this->calcTotalColor( color );

        gp_mesh_t mesh;
        if( gp_calculate_mesh_size( m_canvas, &mesh ) == GP_FAILURE )
        {
            MENGINE_ASSERTION_FATAL( false, "gp" );

            return;
        }

        m_renderVertex2D.resize( mesh.vertex_count );
        m_renderIndices.resize( mesh.index_count );

        mesh.color.r = color.getR();
        mesh.color.g = color.getG();
        mesh.color.b = color.getB();
        mesh.color.a = color.getA();

        VectorRenderVertex2D::value_type * vertices = m_renderVertex2D.data();

        mesh.positions_buffer = vertices;
        mesh.positions_offset = offsetof( VectorRenderVertex2D::value_type, position );
        mesh.positions_stride = sizeof( VectorRenderVertex2D::value_type );

        mesh.colors_buffer = vertices;
        mesh.colors_offset = offsetof( VectorRenderVertex2D::value_type, color );
        mesh.colors_stride = sizeof( VectorRenderVertex2D::value_type );

        mesh.uv_buffer = GP_NULLPTR;
        mesh.uv_offset = 0;
        mesh.uv_stride = 0;

        VectorRenderIndex::value_type * indices = m_renderIndices.data();

        mesh.indices_buffer = indices;
        mesh.indices_offset = 0;
        mesh.indices_stride = sizeof( VectorRenderIndex::value_type );

        if( gp_render( m_canvas, &mesh ) == GP_FAILURE )
        {
            MENGINE_ASSERTION_FATAL( false, "gp" );

            return;
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        for( RenderVertex2D & vertex : m_renderVertex2D )
        {
            mt::vec3f pwm;
            mt::mul_v3_v3_m4( pwm, vertex.position, wm );

            vertex.position = pwm;
        }
    }
}
