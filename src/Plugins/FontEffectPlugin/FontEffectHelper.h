#pragma once

#include "FontEffectDesc.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        const Char * getFontEffectTypeName( EFontEffectType _type );
        const Char * getFontEffectSpaceName( EFontEffectSpace _space );
        const Char * getFontEffectBlendModeName( EFontEffectBlendMode _mode );
        const Char * getFontEffectOutlinePositionName( EFontEffectOutlinePosition _position );
        const Char * getFontEffectPatternTileName( EFontEffectPatternTile _tile );
        const Char * getFontEffectGradientTypeName( EFontEffectGradientType _type );
        //////////////////////////////////////////////////////////////////////////
    }
}
