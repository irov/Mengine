#pragma once

#include "Config/Config.h"

#ifndef MENGINE_MAX_TEXTURE_STAGES
#define MENGINE_MAX_TEXTURE_STAGES 4
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ERenderPlatform
    {
        RP_DX9,
        RP_DX11,
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
        BF_ONE_MINUS_SOURCE_ALPHA
    };
    //////////////////////////////////////////////////////////////////////////
    enum EBlendOp
    {
        BOP_ADD = 0,
        BOP_SUBTRACT,
        BOP_REVSUBTRACT,
        BOP_MIN,
        BOP_MAX
    };
    //////////////////////////////////////////////////////////////////////////
    enum EPrimitiveType
    {
        PT_POINTLIST = 0,
        PT_LINELIST,
        PT_LINESTRIP,
        PT_TRIANGLELIST,
        PT_TRIANGLESTRIP,
        PT_TRIANGLEFAN
    };
    //////////////////////////////////////////////////////////////////////////
    enum ETextureAddressMode
    {
        TAM_WRAP = 0,
        TAM_MIRROR,
        TAM_CLAMP,
        TAM_BORDER,
        TAM_MIRRORONCE
    };
    //////////////////////////////////////////////////////////////////////////
    enum ECullMode
    {
        CM_CULL_NONE = 0,
        CM_CULL_CW, // D3D11 using FrontCounterClockwise for order of polygons
        CM_CULL_CCW,
		CM_CULL_BACK,
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
        CMPF_GREATER
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
        FM_SOLID
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
        SHT_PHONG
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
        TOP_SUBTRACT
    };
    //////////////////////////////////////////////////////////////////////////
    enum ETextureArgument
    {
        TARG_CURRENT = 0,
        TARG_DIFFUSE,
        TARG_SPECULAR,
        TARG_TEXTURE,
        TARG_TFACTOR
    };
    //////////////////////////////////////////////////////////////////////////
    enum ETextureFilterType
    {
        TFT_MAGNIFICATION = 0,
        TFT_MINIFICATION,
        TFT_MIPMAP
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

        __MAX_TEXTURE_FILTER__
    };
    //////////////////////////////////////////////////////////////////////////
    enum EBufferLockFlag
    {
        BLF_LOCK_NONE = 0,
        BLF_LOCK_READONLY,
        BLF_LOCK_DISCARD,
        BLF_LOCK_NOOVERWRITE,
        BLF_LOCK_NOSYSLOCK,
		BLF_LOCK_WRITE, // D3D11_MAP_WRITE
    };
    //////////////////////////////////////////////////////////////////////////
    enum ERenderBatchMode
    {
        ERBM_NONE = 0,
        ERBM_NORMAL = 1,
        ERBM_SMART = 2
    };
}
