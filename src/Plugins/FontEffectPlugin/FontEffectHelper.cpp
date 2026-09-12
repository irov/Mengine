#include "FontEffectHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const Char * const FontEffectTypeNames[__EFET_MAX__] = {
            "Fill",
            "Outline",
            "Shadow",
            "Glow",
            "InnerShadow",
            "InnerGlow",
            "Bevel",
            "Blur"
        };
        //////////////////////////////////////////////////////////////////////////
        static const Char * const FontEffectGradientSpaceNames[__EFEGS_MAX__] = {
            "Glyph",
            "Font"
        };
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        const Char * getFontEffectTypeName( EFontEffectType _type )
        {
            if( _type >= __EFET_MAX__ )
            {
                return "";
            }

            const Char * name = Detail::FontEffectTypeNames[_type];

            return name;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getFontEffectGradientSpaceName( EFontEffectGradientSpace _space )
        {
            if( _space >= __EFEGS_MAX__ )
            {
                return "";
            }

            const Char * name = Detail::FontEffectGradientSpaceNames[_space];

            return name;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}
