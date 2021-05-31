#pragma once

#include "Kernel/PixelFormat.h"

#include "Config/Typedef.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE uint32_t getPixelFormatChannels( EPixelFormat _format )
        {
            switch( _format )
            {
            case PF_L8:
                return 1;
            case PF_A8:
                return 1;
            case PF_A4L4:
                return 2;
            case PF_A1R5G5B5:
                return 4;
            case PF_R5G6B5:
                return 3;
            case PF_A4R4G4B4:
                return 4;
            case PF_R8G8B8:
                return 3;
            case PF_A8R8G8B8:
                return 4;
            case PF_X8R8G8B8:
                return 4;
            case PF_SHORT_GR:
                return 2;
            case PF_DXT1:
                return 4;
            case PF_DXT2:
                return 4;
            case PF_DXT3:
                return 4;
            case PF_DXT4:
                return 4;
            case PF_DXT5:
                return 4;
            case PF_UNKNOWN:
                return ~0U;
            default:
                break;
            }

            return ~0U;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE uint32_t getPixelFormatDepth( EPixelFormat _format )
        {
            MENGINE_UNUSED( _format );

            //ToDo

            return 1;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t getTextureMemorySize( uint32_t _width, uint32_t _height, EPixelFormat _format );
        //////////////////////////////////////////////////////////////////////////
        EPixelFormat findBestPixelFormat( uint32_t _channel, uint32_t _depth );
        //////////////////////////////////////////////////////////////////////////
    }
}