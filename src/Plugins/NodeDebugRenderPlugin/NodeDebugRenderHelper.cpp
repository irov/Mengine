#include "NodeDebugRenderHelper.h"

#include "Interface/RenderServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        void nodeDebugRenderLine( const RenderContext * _context, const VectorRenderVertex2D & _vertices )
        {
            const RenderMaterialInterfacePtr & debugMaterial = RENDERMATERIAL_SERVICE()
                ->getDebugLineMaterial();

            RENDER_SERVICE()
                ->addRenderLine( _context
                    , debugMaterial
                    , _vertices.data()
                    , (uint32_t)_vertices.size()
                    , nullptr
                    , true );
        }
        //////////////////////////////////////////////////////////////////////////
        void nodeDebugRenderCircle( const RenderContext * _context, const mt::mat4f & _wm, float _radius, uint32_t _count, uint32_t _color )
        {
            uint32_t numpoints = _count;
            uint32_t vertexCount = numpoints * 2;

            VectorRenderVertex2D & vertices = RENDER_SERVICE()
                ->getDebugRenderVertex2D( vertexCount );

            Vector<mt::vec2f> ring( numpoints );

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

            Helper::nodeDebugRenderLine( _context, vertices );
        }
        //////////////////////////////////////////////////////////////////////////
        void nodeDebugRenderPolygon( const RenderContext * _context, const mt::mat4f & _wm, const Polygon & _polygon, uint32_t _color )
        {
            uint32_t numpoints = _polygon.size();

            if( numpoints == 0 )
            {
                return;
            }

            uint32_t vertexCount = numpoints * 2;

            VectorRenderVertex2D & vertices = RENDER_SERVICE()
                ->getDebugRenderVertex2D( vertexCount );

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

            Helper::nodeDebugRenderLine( _context, vertices );
        }
    }
}