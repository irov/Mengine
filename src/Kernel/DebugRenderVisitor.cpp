#include "DebugRenderVisitor.h"

#include "Interface/RenderServiceInterface.h"

#include "Kernel/RenderVertex2D.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DebugRenderVisitor::DebugRenderVisitor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DebugRenderVisitor::~DebugRenderVisitor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugRenderVisitor::renderLine_( RenderVertex2D * _vertices, uint32_t _count )
    {
        const RenderMaterialInterfacePtr & debugMaterial = RENDERMATERIAL_SERVICE()
            ->getDebugMaterial();

        RENDER_SERVICE()
            ->addRenderLine( m_context
                , debugMaterial
                , _vertices
                , _count
                , nullptr
                , true );
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugRenderVisitor::renderCircle_( const mt::mat4f & _wm, float _radius, uint32_t _count, uint32_t _color )
    {
        uint32_t numpoints = _count;
        uint32_t vertexCount = numpoints * 2;

        RenderVertex2D * vertices = RENDER_SERVICE()
            ->getDebugRenderVertex2D( vertexCount );

        if( vertices == nullptr )
        {
            return;
        }

        std::vector<mt::vec2f> ring( numpoints );

        for( uint32_t i = 0; i != numpoints; ++i )
        {
            float x = _radius * MT_cosf( mt::constant::two_pi / float( numpoints ) * float( i ) );
            float y = _radius * MT_sinf( mt::constant::two_pi / float( numpoints ) * float( i ) );

            ring[i] = mt::vec2f( x, y );
        }

        for( uint32_t i = 0; i != numpoints; ++i )
        {
            uint32_t j = (i + 1) % numpoints;

            mt::vec3f trP0;
            mt::mul_v3_v2_m4( trP0, ring[i], _wm );

            RenderVertex2D & v0 = vertices[i * 2 + 0];

            v0.position = trP0;

            v0.color = _color;

            for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index )
            {
                v0.uv[uv_index].x = 0.f;
                v0.uv[uv_index].y = 0.f;
            }

            mt::vec3f trP1;
            mt::mul_v3_v2_m4( trP1, ring[j], _wm );

            RenderVertex2D & v1 = vertices[i * 2 + 1];

            v1.position = trP1;

            v1.color = _color;

            for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index )
            {
                v1.uv[uv_index].x = 0.f;
                v1.uv[uv_index].y = 0.f;
            }
        }

        this->renderLine_( vertices, vertexCount );
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugRenderVisitor::renderPolygon_( const mt::mat4f & _wm, const Polygon & _polygon, uint32_t _color )
    {
        uint32_t numpoints = _polygon.size();

        if( numpoints == 0 )
        {
            return;
        }

        uint32_t vertexCount = numpoints * 2;

        RenderVertex2D * vertices = RENDER_SERVICE()
            ->getDebugRenderVertex2D( vertexCount );

        if( vertices == nullptr )
        {
            return;
        }

        const VectorPoints & points = _polygon.getPoints();

        for( uint32_t i = 0; i != numpoints; ++i )
        {
            uint32_t j = (i + 1) % numpoints;

            mt::vec3f trP0;
            mt::mul_v3_v2_m4( trP0, points[i], _wm );

            RenderVertex2D & v0 = vertices[i * 2 + 0];

            v0.position = trP0;

            v0.color = _color;

            for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index )
            {
                v0.uv[uv_index].x = 0.f;
                v0.uv[uv_index].y = 0.f;
            }

            mt::vec3f trP1;
            mt::mul_v3_v2_m4( trP1, points[j], _wm );

            RenderVertex2D & v1 = vertices[i * 2 + 1];

            v1.position = trP1;

            v1.color = _color;

            for( uint32_t uv_index = 0; uv_index != MENGINE_RENDER_VERTEX_UV_COUNT; ++uv_index )
            {
                v1.uv[uv_index].x = 0.f;
                v1.uv[uv_index].y = 0.f;
            }
        }

        this->renderLine_( vertices, vertexCount );
    }
}