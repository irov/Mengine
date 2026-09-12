#pragma once

#include "Kernel/Color.h"
#include "Kernel/Vector.h"

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
        EFET_BLUR
    };
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_FONTEFFECT_TYPE_MAX (Mengine::EFET_BLUR + 1)
    //////////////////////////////////////////////////////////////////////////
    enum EFontEffectSpace : uint32_t
    {
        EFES_GLYPH = 0,
        EFES_FONT
    };
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_FONTEFFECT_SPACE_MAX (Mengine::EFES_FONT + 1)
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
        float angle = 90.f;
        EFontEffectSpace space = EFES_GLYPH;
        VectorFontEffectGradientStops stops;
    };
    //////////////////////////////////////////////////////////////////////////
    struct FontEffectStyleDesc
    {
        EFontEffectType type = EFET_FILL;
        bool enabled = true;
        float opacity = 1.f;

        Color color = Color( 1.f, 1.f, 1.f, 1.f );

        FontEffectGradientDesc gradient;

        float width = 1.f;
        float sharpness = 0.f;

        mt::vec2f offset = mt::vec2f( 0.f, 0.f );

        float blur = 0.f;
        float spread = 0.f;

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
