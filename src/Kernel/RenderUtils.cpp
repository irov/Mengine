#include "RenderUtils.h"

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
            uint32_t HWWidth = _image->getHWWidth();
            uint32_t HWHeight = _image->getHWHeight();
            uint32_t HWChannels = _image->getHWChannels();
            uint32_t HWDepth = _image->getHWDepth();
            EPixelFormat HWPixelFormat = _image->getHWPixelFormat();

            uint32_t memorySize = Helper::getTextureMemorySize( HWWidth, HWHeight, HWChannels, HWDepth, HWPixelFormat );

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
    }
}