#pragma once

#include "Kernel/Color.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        ColorValue_ARGB makeRGBA8( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a );
        ColorValue_ARGB makeRGBA( float _r, float _g, float _b, float _a );
        //////////////////////////////////////////////////////////////////////////
        Color makeColor( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a );
        Color makeColorARGB( ColorValue_ARGB _argb );
        //////////////////////////////////////////////////////////////////////////
    }
}
