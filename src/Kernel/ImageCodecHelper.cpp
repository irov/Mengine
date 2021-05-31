#include "ImageCodecHelper.h"

#include "Kernel/PixelFormatHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        uint32_t getImageCodecDataFullSize( const ImageCodecDataInfo * _info )
        {
            uint32_t full_size = 0;

            uint32_t mipmaps = _info->mipmaps;

            for( uint32_t i = 0; i != mipmaps; ++i )
            {
                uint32_t s = Helper::getImageCodecDataMipMapSize( _info, i );

                full_size += s;
            }

            return full_size;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t getImageCodecDataMipMapSize( const ImageCodecDataInfo * _info, uint32_t _level )
        {
            uint32_t mipmap_width = (_info->width >> _level);
            uint32_t mipmap_height = (_info->height >> _level);

            uint32_t mipmap_size = Helper::getTextureMemorySize( mipmap_width, mipmap_height, _info->format );

            return mipmap_size;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t getImageCodecDataSize( const ImageCodecDataInfo * _info )
        {
            uint32_t size = Helper::getTextureMemorySize( _info->width, _info->height, _info->format );

            return size;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}