#pragma once

#include "Kernel/Color.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        uint8_t makeColorChannelF( float _channel );
        float makeColorChannel8( uint8_t _channel );
        //////////////////////////////////////////////////////////////////////////
        ColorValue_ARGB makeRGBA8( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a );
        ColorValue_ARGB makeRGBAF( float _r, float _g, float _b, float _a );
        //////////////////////////////////////////////////////////////////////////
        Color makeColor8( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a );
        Color makeColorF( float _r, float _g, float _b, float _a );
        //////////////////////////////////////////////////////////////////////////
        Color makeColorARGB( ColorValue_ARGB _argb );
        //////////////////////////////////////////////////////////////////////////
    }
}