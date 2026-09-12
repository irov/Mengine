#pragma once

#include "Kernel/Color.h"
#include "Kernel/Vector.h"
#include "Kernel/FilePath.h"
#include "Kernel/ConstString.h"

#include "math/vec2.h"

#ifndef MENGINE_FONTEFFECT_MAX_LAYERS
#define MENGINE_FONTEFFECT_MAX_LAYERS 4
#endif

#ifndef MENGINE_FONTEFFECT_MAX_OFFSET
#define MENGINE_FONTEFFECT_MAX_OFFSET 256.f
#endif

#ifndef MENGINE_FONTEFFECT_MAX_SIZE
#define MENGINE_FONTEFFECT_MAX_SIZE 256.f
#endif

#ifndef MENGINE_FONTEFFECT_MAX_ANGLE
#define MENGINE_FONTEFFECT_MAX_ANGLE 36000.f
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EFontEffectType : uint32_t
    {
        EFET_FILL = 0,
        EFET_OUTLINE,
        EFET_SHADOW,
        EFET_GLOW,
        EFET_INNER_SHADOW,
        EFET_INNER_GLOW,
        EFET_BEVEL,
        EFET_BLUR,
        EFET_SATIN,
        EFET_PATTERN
    };
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_FONTEFFECT_TYPE_MAX (Mengine::EFET_PATTERN + 1)
    //////////////////////////////////////////////////////////////////////////
    enum EFontEffectSpace : uint32_t
    {
        EFES_GLYPH = 0,
        EFES_FONT
    };
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_FONTEFFECT_SPACE_MAX (Mengine::EFES_FONT + 1)
    //////////////////////////////////////////////////////////////////////////
    enum EFontEffectBlendMode : uint32_t
    {
        EFEBM_NORMAL = 0,
        EFEBM_MULTIPLY,
        EFEBM_SCREEN,
        EFEBM_OVERLAY,
        EFEBM_DARKEN,
        EFEBM_LIGHTEN,
        EFEBM_ADD,
        EFEBM_SUBTRACT,
        EFEBM_DIFFERENCE,
        EFEBM_ERASE
    };
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_FONTEFFECT_BLEND_MODE_MAX (Mengine::EFEBM_ERASE + 1)
    //////////////////////////////////////////////////////////////////////////
    enum EFontEffectOutlinePosition : uint32_t
    {
        EFEOP_OUTSIDE = 0,
        EFEOP_CENTER,
        EFEOP_INSIDE
    };
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_FONTEFFECT_OUTLINE_POSITION_MAX (Mengine::EFEOP_INSIDE + 1)
    //////////////////////////////////////////////////////////////////////////
    enum EFontEffectPatternTile : uint32_t
    {
        EFEPT_TILE = 0,
        EFEPT_MIRROR,
        EFEPT_CLAMP,
        EFEPT_STRETCH
    };
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_FONTEFFECT_PATTERN_TILE_MAX (Mengine::EFEPT_STRETCH + 1)
    //////////////////////////////////////////////////////////////////////////
    enum EFontEffectGradientType : uint32_t
    {
        EFEGT_LINEAR = 0,
        EFEGT_REFLECTED,
        EFEGT_RADIAL,
        EFEGT_ANGLE,
        EFEGT_DISTANCE
    };
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_FONTEFFECT_GRADIENT_TYPE_MAX (Mengine::EFEGT_DISTANCE + 1)
    //////////////////////////////////////////////////////////////////////////
    struct FontEffectGradientStop
    {
        float t = 0.f;
        Color color;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<FontEffectGradientStop> VectorFontEffectGradientStops;
    //////////////////////////////////////////////////////////////////////////
    struct FontEffectGradientDesc
    {
        bool enabled = false;
        EFontEffectGradientType type = EFEGT_LINEAR;
        float angle = 90.f;
        mt::vec2f center = mt::vec2f( 0.f, 0.f );
        float scale = 1.f;
        bool reverse = false;
        bool dither = false;
        EFontEffectSpace space = EFES_GLYPH;
        VectorFontEffectGradientStops stops;
    };
    //////////////////////////////////////////////////////////////////////////
    struct FontEffectPatternDesc
    {
        FilePath filePath = FilePath::none();
        ConstString codecType = ConstString::none();
        EFontEffectSpace space = EFES_GLYPH;
        EFontEffectPatternTile tile = EFEPT_TILE;
        float scale = 1.f;
        mt::vec2f offset = mt::vec2f( 0.f, 0.f );
        float angle = 0.f;
    };
    //////////////////////////////////////////////////////////////////////////
    struct FontEffectStyleDesc
    {
        EFontEffectType type = EFET_FILL;
        bool enabled = true;
        float opacity = 1.f;
        EFontEffectBlendMode blendMode = EFEBM_NORMAL;

        Color color = Color( 1.f, 1.f, 1.f, 1.f );

        FontEffectGradientDesc gradient;
        FontEffectPatternDesc pattern;

        float width = 1.f;
        float sharpness = 0.f;
        EFontEffectOutlinePosition position = EFEOP_OUTSIDE;

        mt::vec2f offset = mt::vec2f( 0.f, 0.f );

        float blur = 0.f;
        float spread = 0.f;

        float distance = 0.f;
        bool invert = false;

        float depth = 1.f;
        float size = 2.f;
        float soften = 0.f;
        float angle = 120.f;
        float altitude = 30.f;
        Color highlight = Color( 1.f, 1.f, 1.f, 0.75f );
        Color shadow = Color( 0.f, 0.f, 0.f, 0.5f );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<FontEffectStyleDesc> VectorFontEffectStyleDescs;
    //////////////////////////////////////////////////////////////////////////
    struct FontEffectLayerDesc
    {
        bool enabled = true;
        float opacity = 1.f;
        EFontEffectBlendMode blendMode = EFEBM_NORMAL;
        bool knockout = false;
        bool merge = false;
        VectorFontEffectStyleDescs styles;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<FontEffectLayerDesc> VectorFontEffectLayerDescs;
    //////////////////////////////////////////////////////////////////////////
    struct FontEffectDesc
    {
        VectorFontEffectLayerDescs layers;
    };
    //////////////////////////////////////////////////////////////////////////
}
