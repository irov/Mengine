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
            "Blur",
            "Satin",
            "Pattern"
        };
        //////////////////////////////////////////////////////////////////////////
        static const Char * const FontEffectSpaceNames[MENGINE_FONTEFFECT_SPACE_MAX] = {
            "Glyph",
            "Font"
        };
        //////////////////////////////////////////////////////////////////////////
        static const Char * const FontEffectBlendModeNames[MENGINE_FONTEFFECT_BLEND_MODE_MAX] = {
            "Normal",
            "Multiply",
            "Screen",
            "Overlay",
            "Darken",
            "Lighten",
            "Add",
            "Subtract",
            "Difference",
            "Erase"
        };
        //////////////////////////////////////////////////////////////////////////
        static const Char * const FontEffectOutlinePositionNames[MENGINE_FONTEFFECT_OUTLINE_POSITION_MAX] = {
            "Outside",
            "Center",
            "Inside"
        };
        //////////////////////////////////////////////////////////////////////////
        static const Char * const FontEffectPatternTileNames[MENGINE_FONTEFFECT_PATTERN_TILE_MAX] = {
            "Tile",
            "Mirror",
            "Clamp",
            "Stretch"
        };
        //////////////////////////////////////////////////////////////////////////
        static const Char * const FontEffectGradientTypeNames[MENGINE_FONTEFFECT_GRADIENT_TYPE_MAX] = {
            "Linear",
            "Reflected",
            "Radial",
            "Angle",
            "Distance"
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
        const Char * getFontEffectSpaceName( EFontEffectSpace _space )
        {
            if( _space >= MENGINE_FONTEFFECT_SPACE_MAX )
            {
                return "";
            }

            const Char * name = Detail::FontEffectSpaceNames[_space];

            return name;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getFontEffectBlendModeName( EFontEffectBlendMode _mode )
        {
            if( _mode >= MENGINE_FONTEFFECT_BLEND_MODE_MAX )
            {
                return "";
            }

            const Char * name = Detail::FontEffectBlendModeNames[_mode];

            return name;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getFontEffectOutlinePositionName( EFontEffectOutlinePosition _position )
        {
            if( _position >= MENGINE_FONTEFFECT_OUTLINE_POSITION_MAX )
            {
                return "";
            }

            const Char * name = Detail::FontEffectOutlinePositionNames[_position];

            return name;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getFontEffectPatternTileName( EFontEffectPatternTile _tile )
        {
            if( _tile >= MENGINE_FONTEFFECT_PATTERN_TILE_MAX )
            {
                return "";
            }

            const Char * name = Detail::FontEffectPatternTileNames[_tile];

            return name;
        }
        //////////////////////////////////////////////////////////////////////////
        const Char * getFontEffectGradientTypeName( EFontEffectGradientType _type )
        {
            if( _type >= MENGINE_FONTEFFECT_GRADIENT_TYPE_MAX )
            {
                return "";
            }

            const Char * name = Detail::FontEffectGradientTypeNames[_type];

            return name;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}
