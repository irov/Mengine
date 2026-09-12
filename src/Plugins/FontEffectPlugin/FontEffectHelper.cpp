#include "FontEffectHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const Char * const FontEffectTypeNames[MENGINE_FONTEFFECT_TYPE_MAX] = {
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
        static const Char * const FontEffectGradientSpaceNames[MENGINE_FONTEFFECT_GRADIENT_SPACE_MAX] = {
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
            if( _type >= MENGINE_FONTEFFECT_TYPE_MAX )
            {
                return "";
            }

            const Char * name = Detail::FontEffectTypeNames[_type];

            return name;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getFontEffectGradientSpaceName( EFontEffectGradientSpace _space )
        {
            if( _space >= MENGINE_FONTEFFECT_GRADIENT_SPACE_MAX )
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
