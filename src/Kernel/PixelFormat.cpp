#include "PixelFormat.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        uint32_t getTextureMemorySize( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _format )
        {
            switch( _format )
            {
            case PF_L8:
            case PF_A8:
                {
                    return _width * _height * _depth * 1;
                }break;

            case PF_R8G8B8:
            case PF_B8G8R8:
                {
                    return _width * _height * _depth * 3;
                }break;

            case PF_A8R8G8B8:
            case PF_A8B8G8R8:
            case PF_B8G8R8A8:
            case PF_R8G8B8A8:
            case PF_X8R8G8B8:
            case PF_X8B8G8R8:
                {
                    return _width * _height * _depth * 4;
                }break;

            case PF_DXT1:
                {
                    uint32_t w = (_width + 3) / 4;
                    uint32_t h = (_height + 3) / 4;

                    uint32_t size = w * h * _depth * 8;

                    return size;
                }break;

            case PF_DXT2:
            case PF_DXT3:
            case PF_DXT4:
            case PF_DXT5:
                {
                    uint32_t w = (_width + 3) / 4;
                    uint32_t h = (_height + 3) / 4;

                    uint32_t size = w * h * _depth * 16;

                    return size;
                }break;

            case PF_PVRTC4_RGB:
            case PF_PVRTC4_RGBA:
                {
                    uint32_t blockSize = 4 * 4;
                    uint32_t widthBlocks = _width / 4;
                    uint32_t heightBlocks = _height / 4;
                    uint32_t bpp = 4;

                    widthBlocks = widthBlocks > 2 ? widthBlocks : 2;
                    heightBlocks = heightBlocks > 2 ? heightBlocks : 2;

                    uint32_t size = widthBlocks * heightBlocks * ((blockSize * bpp) / 8);

                    return size;
                }break;

            case PF_ETC1:
                {
                    uint32_t w = (_width + 3) / 4;
                    uint32_t h = (_height + 3) / 4;

                    uint32_t size = w * h * _depth * 8;

                    return size;
                }break;

            case PF_UNKNOWN:
                {
                    return _width * _height * _depth * _channels;
                }break;

            default:
                {
                }break;
            }

            return 0;
        }
    }
}