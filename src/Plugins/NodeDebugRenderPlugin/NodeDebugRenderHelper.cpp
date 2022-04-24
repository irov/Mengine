#include "NodeDebugRenderHelper.h"

#include "Interface/RenderServiceInterface.h"

#include "Kernel/DocumentHelper.h"

#include "Config/StdMath.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void nodeDebugRenderLine( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const VectorRenderVertex2D & _vertices, const DocumentPtr & _doc )
        {
            const RenderMaterialInterfacePtr & debugMaterial = RENDERMATERIAL_SERVICE()
                ->getDebugLineMaterial();

            _renderPipeline->addRenderLine( _context
                    , debugMaterial
                    , _vertices.data()
                    , (uint32_t)_vertices.size()
                    , nullptr
                    , true, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void nodeDebugRenderLineByToPoints( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const mt::mat4f & _wm, const mt::vec2f & _startPoint, const mt::vec2f & _finishPoint, ColorValue_ARGB _color, const DocumentPtr & _doc )
        {
            VectorRenderVertex2D & vertices = RENDER_SERVICE()
                ->getDebugRenderVertex2D( 4 );

            mt::vec3f p0_wm;
            mt::mul_v3_v2_m4( p0_wm, _startPoint, _wm );

            mt::vec3f p1_wm;
            mt::mul_v3_v2_m4( p1_wm, _finishPoint, _wm );

            vertices[0].position = p0_wm;
            vertices[0].color = _color;

            vertices[1].position = p1_wm;
            vertices[1].color = _color;

            Helper::nodeDebugRenderLine( _renderPipeline, _context, vertices, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void nodeDebugRenderCircle( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const mt::mat4f & _wm, float _radius, uint32_t _count, ColorValue_ARGB _color, const DocumentPtr & _doc )
        {
            uint32_t numpoints = _count;
            uint32_t vertexCount = numpoints * 2;

            VectorRenderVertex2D & vertices = RENDER_SERVICE()
                ->getDebugRenderVertex2D( vertexCount );

            Vector<mt::vec2f> ring( numpoints );

            for( uint32_t i = 0; i != numpoints; ++i )
            {
                float x = _radius * MENGINE_COSF( mt::constant::two_pi / float( numpoints ) * float( i ) );
                float y = _radius * MENGINE_SINF( mt::constant::two_pi / float( numpoints ) * float( i ) );

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

            Helper::nodeDebugRenderLine( _renderPipeline, _context, vertices, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void nodeDebugRenderPolygon( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const mt::mat4f & _wm, const Polygon & _polygon, ColorValue_ARGB _color, const DocumentPtr & _doc )
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

            Helper::nodeDebugRenderLine( _renderPipeline, _context, vertices, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
        void nodeDebugRenderPoint( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const mt::mat4f & _wm, const mt::vec2f & _point, ColorValue_ARGB _color, float _length, const DocumentPtr & _doc )
        {
            VectorRenderVertex2D & vertices = RENDER_SERVICE()
                ->getDebugRenderVertex2D( 4 );

            mt::vec3f p0_wm;
            mt::mul_v3_v2_m4( p0_wm, _point + mt::vec2f( 0.f, -_length ), _wm );

            mt::vec3f p1_wm;
            mt::mul_v3_v2_m4( p1_wm, _point + mt::vec2f( 0.f, _length ), _wm );

            mt::vec3f p2_wm;
            mt::mul_v3_v2_m4( p2_wm, _point + mt::vec2f( -_length, 0.f ), _wm );

            mt::vec3f p3_wm;
            mt::mul_v3_v2_m4( p3_wm, _point + mt::vec2f( _length, 0.f ), _wm );

            vertices[0].position = p0_wm;
            vertices[0].color = _color;

            vertices[1].position = p1_wm;
            vertices[1].color = _color;

            vertices[2].position = p2_wm;
            vertices[2].color = _color;

            vertices[3].position = p3_wm;
            vertices[3].color = _color;

            Helper::nodeDebugRenderLine( _renderPipeline, _context, vertices, _doc );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}