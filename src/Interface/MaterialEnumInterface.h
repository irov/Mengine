#pragma once

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EMaterial
    {
        EM_DEBUG,

        EM_TEXTURE_SOLID,
        EM_TEXTURE_BLEND,
        EM_TEXTURE_BLEND_A8,
        EM_TEXTURE_BLEND_PREMULTIPLY,
        EM_TEXTURE_BLEND_WC,
        EM_TEXTURE_BLEND_WW,
        EM_TEXTURE_BLEND_CW,
        EM_TEXTURE_INTENSIVE,
        EM_TEXTURE_INTENSIVE_PREMULTIPLY,
        EM_TEXTURE_MULTIPLY,
        EM_TEXTURE_MULTIPLY_PREMULTIPLY,
        EM_TEXTURE_SCREEN,
        EM_TEXTURE_SCREEN_PREMULTIPLY,

        EM_TEXTURE_BLEND_ONLYCOLOR,
        EM_TEXTURE_INTENSIVE_ONLYCOLOR,
        EM_TEXTURE_MULTIPLY_ONLYCOLOR,
        EM_TEXTURE_SCREEN_ONLYCOLOR,

        EM_TEXTURE_BLEND_EXTERNAL_ALPHA,
        EM_TEXTURE_BLEND_EXTERNAL_ALPHA_WC,
        EM_TEXTURE_BLEND_EXTERNAL_ALPHA_WW,
        EM_TEXTURE_BLEND_EXTERNAL_ALPHA_CW,
        EM_TEXTURE_INTENSIVE_EXTERNAL_ALPHA,
        EM_TEXTURE_MULTIPLY_EXTERNAL_ALPHA,
        EM_TEXTURE_SCREEN_EXTERNAL_ALPHA,

        EM_TEXTURE_BLEND_EXTERNAL_ALPHA_ONLYCOLOR,
        EM_TEXTURE_INTENSIVE_EXTERNAL_ALPHA_ONLYCOLOR,
        EM_TEXTURE_MULTIPLY_EXTERNAL_ALPHA_ONLYCOLOR,
        EM_TEXTURE_SCREEN_EXTERNAL_ALPHA_ONLYCOLOR,

        EM_TEXTURE_TRACKMATTE_BLEND,
        EM_TEXTURE_TRACKMATTE_BLEND_PREMULTIPLY,
        EM_TEXTURE_TRACKMATTE_INTENSIVE,
        EM_TEXTURE_TRACKMATTE_INTENSIVE_PREMULTIPLY,
        EM_TEXTURE_TRACKMATTE_INVERTED_BLEND,
        EM_TEXTURE_TRACKMATTE_INVERTED_BLEND_PREMULTIPLY,
        EM_TEXTURE_TRACKMATTE_INVERTED_INTENSIVE,
        EM_TEXTURE_TRACKMATTE_INVERTED_INTENSIVE_PREMULTIPLY,

        EM_TEXTURE_TRACKMATTE_BLEND_EXTERNAL_ALPHA,
        EM_TEXTURE_TRACKMATTE_BLEND_PREMULTIPLY_EXTERNAL_ALPHA,
        EM_TEXTURE_TRACKMATTE_INTENSIVE_EXTERNAL_ALPHA,
        EM_TEXTURE_TRACKMATTE_INTENSIVE_PREMULTIPLY_EXTERNAL_ALPHA,
        EM_TEXTURE_TRACKMATTE_INVERTED_BLEND_EXTERNAL_ALPHA,
        EM_TEXTURE_TRACKMATTE_INVERTED_BLEND_PREMULTIPLY_EXTERNAL_ALPHA,
        EM_TEXTURE_TRACKMATTE_INVERTED_INTENSIVE_EXTERNAL_ALPHA,
        EM_TEXTURE_TRACKMATTE_INVERTED_INTENSIVE_PREMULTIPLY_EXTERNAL_ALPHA,

        EM_TEXTURE_ALPHAMASK_BLEND,

        EM_COLOR_SOLID,
        EM_COLOR_BLEND,
        EM_COLOR_INTENSIVE,
        EM_COLOR_MULTIPLY,
        EM_COLOR_SCREEN,
        EM_MATERIAL_COUNT
    };
    //////////////////////////////////////////////////////////////////////////
    enum EMaterialBlendMode
    {
        EMB_NORMAL,
        EMB_ADD,
        EMB_SCREEN,
        EMB_MULTIPLY
    };
}
