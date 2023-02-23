#include "RenderHelper.h"

#include "Kernel/PixelFormatHelper.h"

#include "math/convex8.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void makeRenderBoundingBox( mt::box2f * _bb, const RenderVertex2D * _vertices, uint32_t _verticesCount )
        {
            if( _verticesCount == 0 )
            {
                mt::reset( *_bb, 0.f, 0.f );

                return;
            }

            const RenderVertex2D & v0 = _vertices[0];
            mt::reset( *_bb, v0.position.x, v0.position.y );

            for( uint32_t i = 1; i != _verticesCount; ++i )
            {
                const RenderVertex2D & v = _vertices[i];

                mt::add_internal_point( *_bb, v.position.x, v.position.y );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t getImageMemorySize( const RenderImageInterfacePtr & _image )
        {
            uint32_t hwWidth = _image->getHWWidth();
            uint32_t hwHeight = _image->getHWHeight();
            EPixelFormat hwPixelFormat = _image->getHWPixelFormat();

            uint32_t memorySize = Helper::getTextureMemorySize( hwWidth, hwHeight, hwPixelFormat );

            return memorySize;
        }
        //////////////////////////////////////////////////////////////////////////
        void multiplyColorBuffer( const Color & _color, uint8_t * const _out, const uint8_t * _in )
        {
            ColorValue_ARGB argb = _color.getAsARGB();

#ifdef MENGINE_RENDER_COLOR_RGBA
            uint8_t a8 = (argb >> 24) & 0xFF;
            uint8_t b8 = (argb >> 16) & 0xFF;
            uint8_t g8 = (argb >> 8) & 0xFF;
            uint8_t r8 = (argb >> 0) & 0xFF;
#else
            uint8_t a8 = (argb >> 24) & 0xFF;
            uint8_t r8 = (argb >> 16) & 0xFF;
            uint8_t g8 = (argb >> 8) & 0xFF;
            uint8_t b8 = (argb >> 0) & 0xFF;
#endif

            uint16_t aa = _in[0] * a8 / 255;
            uint16_t rr = _in[1] * r8 / 255;
            uint16_t gg = _in[2] * g8 / 255;
            uint16_t bb = _in[3] * b8 / 255;

            _out[0] = (uint8_t)aa;
            _out[1] = (uint8_t)rr;
            _out[2] = (uint8_t)gg;
            _out[3] = (uint8_t)bb;
        }
        //////////////////////////////////////////////////////////////////////////
        double calcRenderQuadSquare( const RenderVertex2D * _vertex, uint32_t _vertexCount, const Viewport & _viewport )
        {
            double fillrate = 0.0;

            uint32_t triangleCount2 = (_vertexCount / 4);

            for( uint32_t i = 0; i != triangleCount2; ++i )
            {
                const RenderVertex2D & v0 = _vertex[i * 4 + 0];
                const RenderVertex2D & v1 = _vertex[i * 4 + 1];
                const RenderVertex2D & v2 = _vertex[i * 4 + 2];
                const RenderVertex2D & v3 = _vertex[i * 4 + 3];

                mt::convex8 tri1;
                tri1.add( v0.position.to_vec2f() );
                tri1.add( v1.position.to_vec2f() );
                tri1.add( v2.position.to_vec2f() );

                mt::convex8 tri2;
                tri2.add( v0.position.to_vec2f() );
                tri2.add( v2.position.to_vec2f() );
                tri2.add( v3.position.to_vec2f() );

                mt::convex8 cv;
                cv.add( mt::vec2f( _viewport.begin.x, _viewport.begin.y ) );
                cv.add( mt::vec2f( _viewport.end.x, _viewport.begin.y ) );
                cv.add( mt::vec2f( _viewport.end.x, _viewport.end.y ) );
                cv.add( mt::vec2f( _viewport.begin.x, _viewport.end.y ) );

                mt::convex8 tric1;
                mt::convex8_intersect( tri1, cv, tric1 );

                mt::convex8 tric2;
                mt::convex8_intersect( tri2, cv, tric2 );

                float tric1_area = mt::convex8_area( tric1 );
                float tric2_area = mt::convex8_area( tric2 );

                fillrate += tric1_area;
                fillrate += tric2_area;
            }

            return fillrate;
        }
        //////////////////////////////////////////////////////////////////////////
        double calcRenderMeshSquare( const RenderVertex2D * _vertex, uint32_t _vertexCount, const RenderIndex * _indices, uint32_t _indicesCount, const Viewport & _viewport )
        {
            MENGINE_UNUSED( _vertexCount );

            double fillrate = 0.0;

            for( uint32_t i = 0; i != (_indicesCount / 3); ++i )
            {
                RenderIndex i0 = _indices[i + 0];
                RenderIndex i1 = _indices[i + 1];
                RenderIndex i2 = _indices[i + 2];

                const RenderVertex2D & v0 = _vertex[i0];
                const RenderVertex2D & v1 = _vertex[i1];
                const RenderVertex2D & v2 = _vertex[i2];

                mt::convex8 tri;
                tri.add( v0.position.to_vec2f() );
                tri.add( v1.position.to_vec2f() );
                tri.add( v2.position.to_vec2f() );

                mt::convex8 cv;
                cv.add( mt::vec2f( _viewport.begin.x, _viewport.begin.y ) );
                cv.add( mt::vec2f( _viewport.end.x, _viewport.begin.y ) );
                cv.add( mt::vec2f( _viewport.end.x, _viewport.end.y ) );
                cv.add( mt::vec2f( _viewport.begin.x, _viewport.end.y ) );

                mt::convex8 tric;
                mt::convex8_intersect( tri, cv, tric );

                float tric_area = mt::convex8_area( tric );

                fillrate += tric_area;
            }

            return fillrate;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}