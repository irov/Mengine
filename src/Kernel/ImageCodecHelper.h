#pragma once

#include "Interface/ImageCodecInterface.h"

namespace Mengine
{
    namespace Helper
    {
        uint32_t getImageCodecDataFullSize( const ImageCodecDataInfo * _info );
        uint32_t getImageCodecDataMipMapSize( const ImageCodecDataInfo * _info, uint32_t _level );
        uint32_t getImageCodecDataSize( const ImageCodecDataInfo * _info );
    }
}