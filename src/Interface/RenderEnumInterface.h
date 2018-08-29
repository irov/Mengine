#pragma once

#include "Config/Typedef.h"

#   ifndef MENGINE_MAX_TEXTURE_STAGES
#	define MENGINE_MAX_TEXTURE_STAGES 2
#   endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ERenderPlatform
    {
        RP_DX9,
        RP_OPENGL,
        RP_OPENGLES,
        RP_UNKNOWN
    };
    //////////////////////////////////////////////////////////////////////////
    enum EBlendFactor
    {
        BF_ONE = 0,
        BF_ZERO,
        BF_DEST_COLOUR,
        BF_SOURCE_COLOUR,
        BF_ONE_MINUS_DEST_COLOUR,
        BF_ONE_MINUS_SOURCE_COLOUR,
        BF_DEST_ALPHA,
        BF_SOURCE_ALPHA,
        BF_ONE_MINUS_DEST_ALPHA,
        BF_ONE_MINUS_SOURCE_ALPHA,

        BF_MAX_VALUE
    };
    //////////////////////////////////////////////////////////////////////////
    enum EBlendOp
    {
        BOP_ADD = 0,
        BOP_SUBTRACT,
        BOP_REVSUBTRACT,
        BOP_MIN,
        BOP_MAX,

        BOP_MAX_VALUE
    };
    //////////////////////////////////////////////////////////////////////////
    enum EPrimitiveType
    {
        PT_POINTLIST = 0,
        PT_LINELIST,
        PT_LINESTRIP,
        PT_TRIANGLELIST,
        PT_TRIANGLESTRIP,
        PT_TRIANGLEFAN,

        PT_MAX_VALUE
    };
    //////////////////////////////////////////////////////////////////////////
    enum ETextureAddressMode
    {
        TAM_WRAP = 0,
        TAM_MIRROR,
        TAM_CLAMP,
        TAM_BORDER,
        TAM_MIRRORONCE,

        TAM_MAX_VALUE
    };
    //////////////////////////////////////////////////////////////////////////
    enum ECullMode
    {
        CM_CULL_NONE = 0,
        CM_CULL_CW,
        CM_CULL_CCW,

        CM_MAX_VALUE
    };
    //////////////////////////////////////////////////////////////////////////
    enum ECompareFunction
    {
        CMPF_ALWAYS_FAIL = 0,
        CMPF_ALWAYS_PASS,
        CMPF_LESS,
        CMPF_LESS_EQUAL,
        CMPF_EQUAL,
        CMPF_NOT_EQUAL,
        CMPF_GREATER_EQUAL,
        CMPF_GREATER,

        CMPF_MAX_VALUE
    };
    //////////////////////////////////////////////////////////////////////////
    enum EVertexAttributeType
    {
        VAT_FLOAT = 0,
        VAT_UNSIGNED_BYTE
    };
    //////////////////////////////////////////////////////////////////////////
    enum EFillMode
    {
        FM_POINT = 0,
        FM_WIREFRAME,
        FM_SOLID,

        FM_MAX_VALUE
    };
    //////////////////////////////////////////////////////////////////////////
    enum EBufferType
    {
        BT_STATIC,
        BT_STREAM,
        BT_DYNAMIC
    };
    //////////////////////////////////////////////////////////////////////////
    enum EFrameBufferType
    {
        FBT_COLOR = 0x1,
        FBT_DEPTH = 0x2,
        FBT_STENCIL = 0x4,
    };
    //////////////////////////////////////////////////////////////////////////
    enum EShadeType
    {
        SHT_FLAT = 0,
        SHT_GOURAUD,
        SHT_PHONG,

        SHT_MAX_VALUE
    };
    //////////////////////////////////////////////////////////////////////////
    enum ETextureOp
    {
        TOP_DISABLE = 0,
        TOP_SELECTARG1,
        TOP_SELECTARG2,
        TOP_MODULATE,
        TOP_MODULATE2X,
        TOP_MODULATE4X,
        TOP_ADD,
        TOP_SUBTRACT,

        TOP_MAX_VALUE
    };
    //////////////////////////////////////////////////////////////////////////
    enum ETextureArgument
    {
        TARG_CURRENT = 0,
        TARG_DIFFUSE,
        TARG_SPECULAR,
        TARG_TEXTURE,
        TARG_TFACTOR,

        TARG_MAX_VALUE
    };
    //////////////////////////////////////////////////////////////////////////
    enum ETextureFilterType
    {
        TFT_MAGNIFICATION = 0,
        TFT_MINIFICATION,
        TFT_MIPMAP,

        TFT_MAX_VALUE
    };
    //////////////////////////////////////////////////////////////////////////
    enum ETextureFilter
    {
        TF_NONE = 0,
        TF_POINT,
        TF_LINEAR,
        TF_ANISOTROPIC,
        TF_FLATCUBIC,
        TF_GAUSSIANCUBIC,

        TF_MAX_VALUE
    };
    //////////////////////////////////////////////////////////////////////////
    enum EBufferLockFlag
    {
        BLF_LOCK_NONE = 0,
        BLF_LOCK_READONLY,
        BLF_LOCK_DISCARD,
        BLF_LOCK_NOOVERWRITE,
        BLF_LOCK_NOSYSLOCK,

        BLF_MAX_VALUE
    };
    //////////////////////////////////////////////////////////////////////////
    enum ERenderImageMode
    {
        ERIM_NORMAL = 0,
        ERIM_DYNAMIC,
        ERIM_RENDER_TARGET,

        ERIM_MAX_VALUE
    };
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

        EM_TEXTURE_ALPHAMASK_BLEND,

        EM_COLOR_SOLID,
        EM_COLOR_BLEND,
        EM_COLOR_INTENSIVE,
        EM_COLOR_MULTIPLY,
        EM_COLOR_SCREEN,
        EM_MATERIAL_COUNT
    };
    //////////////////////////////////////////////////////////////////////////
    enum ERenderBatchMode
    {
        ERBM_NONE = 0,
        ERBM_NORMAL = 1,
        ERBM_SMART = 2
    };
}
