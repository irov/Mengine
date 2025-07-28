#pragma once

#include "Kernel/Color.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        uint8_t makeColorChannelF( float _channel ) MENGINE_NOEXCEPT;
        float makeColorChannel8( uint8_t _channel ) MENGINE_NOEXCEPT;
        //////////////////////////////////////////////////////////////////////////
        ColorValue_ARGB makeRGBA8( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a ) MENGINE_NOEXCEPT;
        ColorValue_ARGB makeRGBAF( float _r, float _g, float _b, float _a ) MENGINE_NOEXCEPT;
        //////////////////////////////////////////////////////////////////////////
        Color makeColor8( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a ) MENGINE_NOEXCEPT;
        Color makeColorF( float _r, float _g, float _b, float _a ) MENGINE_NOEXCEPT;
        //////////////////////////////////////////////////////////////////////////
        Color makeColorARGB( ColorValue_ARGB _argb ) MENGINE_NOEXCEPT;
        //////////////////////////////////////////////////////////////////////////
    }
}