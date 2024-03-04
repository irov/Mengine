#pragma once

#include "Interface/RenderEnumInterface.h"

#include "Environment/DirectX9/DX9RenderIncluder.h"

#include "Kernel/RenderIndex.h"
#include "Kernel/PixelFormat.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE uint32_t getPrimitiveCount( EPrimitiveType _pType, uint32_t _indexCount )
        {
            switch( _pType )
            {
            case PT_POINTLIST:
                return _indexCount;
            case PT_LINELIST:
                return _indexCount / 2;
            case PT_LINESTRIP:
                return _indexCount - 1;
            case PT_TRIANGLELIST:
                return _indexCount / 3;
            case PT_TRIANGLESTRIP:
            case PT_TRIANGLEFAN:
                return _indexCount - 2;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3DMULTISAMPLE_TYPE getMultiSampleType( uint32_t _count )
        {
            switch( _count )
            {
            case 0:
                return D3DMULTISAMPLE_NONE;
            case 1:
                return D3DMULTISAMPLE_NONE;
            case 2:
                return D3DMULTISAMPLE_2_SAMPLES;
            case 3:
                return D3DMULTISAMPLE_3_SAMPLES;
            case 4:
                return D3DMULTISAMPLE_4_SAMPLES;
            case 5:
                return D3DMULTISAMPLE_5_SAMPLES;
            case 6:
                return D3DMULTISAMPLE_6_SAMPLES;
            case 7:
                return D3DMULTISAMPLE_7_SAMPLES;
            case 8:
                return D3DMULTISAMPLE_8_SAMPLES;
            default:
                break;
            }

            return D3DMULTISAMPLE_NONE;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE DWORD toD3DBufferLock( EBufferLockFlag _flag )
        {
            switch( _flag )
            {
            case BLF_LOCK_NONE:
                return 0;
            case BLF_LOCK_READONLY:
                return D3DLOCK_READONLY;
            case BLF_LOCK_DISCARD:
                return D3DLOCK_DISCARD;
            case BLF_LOCK_NOOVERWRITE:
                return D3DLOCK_NOOVERWRITE;
            case BLF_LOCK_NOSYSLOCK:
                return D3DLOCK_NOSYSLOCK;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3DFORMAT toD3DFormat( EPixelFormat _format )
        {
            switch( _format )
            {
            case PF_L8:
                return D3DFMT_L8;
            case PF_A8:
                return D3DFMT_A8;
            case PF_A4L4:
                return D3DFMT_A4L4;
            case PF_BYTE_LA:
                return D3DFMT_A8L8; // Assume little endian here
            case PF_R3G3B2:
                return D3DFMT_R3G3B2;
            case PF_A1R5G5B5:
                return D3DFMT_A1R5G5B5;
            case PF_R5G6B5:
                return D3DFMT_R5G6B5;
            case PF_A4R4G4B4:
                return D3DFMT_A4R4G4B4;
            case PF_R8G8B8:
                return D3DFMT_R8G8B8;
            case PF_A8R8G8B8:
                return D3DFMT_A8R8G8B8;
            case PF_X8R8G8B8:
                return D3DFMT_X8R8G8B8;
            case PF_SHORT_GR:
                return D3DFMT_G16R16;
            case PF_DXT1:
                return D3DFMT_DXT1;
            case PF_DXT2:
                return D3DFMT_DXT2;
            case PF_DXT3:
                return D3DFMT_DXT3;
            case PF_DXT4:
                return D3DFMT_DXT4;
            case PF_DXT5:
                return D3DFMT_DXT5;
            case PF_UNKNOWN:
                return D3DFMT_UNKNOWN;
            default:
                break;
            }

            return D3DFMT_UNKNOWN;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE EPixelFormat fromD3DFormat( D3DFORMAT _format )
        {
            switch( _format )
            {
            case D3DFMT_L8:
                return PF_L8;
            case D3DFMT_A8:
                return PF_A8;
            case D3DFMT_A4L4:
                return PF_A4L4;
            case D3DFMT_A8L8:
                return PF_BYTE_LA; // Assume little endian here
            case D3DFMT_R3G3B2:
                return PF_R3G3B2;
            case D3DFMT_A1R5G5B5:
                return PF_A1R5G5B5;
            case D3DFMT_R5G6B5:
                return PF_R5G6B5;
            case D3DFMT_A4R4G4B4:
                return PF_A4R4G4B4;
            case D3DFMT_R8G8B8:
                return PF_R8G8B8;
            case D3DFMT_A8R8G8B8:
                return PF_A8R8G8B8;
            case D3DFMT_X8R8G8B8:
                return PF_X8R8G8B8;
            case D3DFMT_G16R16:
                return PF_SHORT_GR;
            case D3DFMT_DXT1:
                return PF_DXT1;
            case D3DFMT_DXT2:
                return PF_DXT2;
            case D3DFMT_DXT3:
                return PF_DXT3;
            case D3DFMT_DXT4:
                return PF_DXT4;
            case D3DFMT_DXT5:
                return PF_DXT5;
            case D3DFMT_UNKNOWN:
                return PF_UNKNOWN;
            default:
                break;
            }

            return PF_UNKNOWN;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE DWORD toD3DBlendFactor( EBlendFactor _blend )
        {
            switch( _blend )
            {
            case BF_ONE:
                return D3DBLEND_ONE;
            case BF_ZERO:
                return D3DBLEND_ZERO;
            case BF_DEST_COLOUR:
                return D3DBLEND_DESTCOLOR;
            case BF_SOURCE_COLOUR:
                return D3DBLEND_SRCCOLOR;
            case BF_ONE_MINUS_DEST_COLOUR:
                return D3DBLEND_INVDESTCOLOR;
            case BF_ONE_MINUS_SOURCE_COLOUR:
                return D3DBLEND_INVSRCCOLOR;
            case BF_DEST_ALPHA:
                return D3DBLEND_DESTALPHA;
            case BF_SOURCE_ALPHA:
                return D3DBLEND_SRCALPHA;
            case BF_ONE_MINUS_DEST_ALPHA:
                return D3DBLEND_INVDESTALPHA;
            case BF_ONE_MINUS_SOURCE_ALPHA:
                return D3DBLEND_INVSRCALPHA;
            }

            return D3DBLEND_ZERO;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE DWORD toD3DBlendOp( EBlendOp _blend )
        {
            switch( _blend )
            {
            case BOP_ADD:
                return D3DBLENDOP_ADD;
            case BOP_SUBTRACT:
                return D3DBLENDOP_SUBTRACT;
            case BOP_REVSUBTRACT:
                return D3DBLENDOP_REVSUBTRACT;
            case BOP_MIN:
                return D3DBLENDOP_MIN;
            case BOP_MAX:
                return D3DBLENDOP_MAX;
            }

            return D3DBLENDOP_ADD;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3DPRIMITIVETYPE toD3DPrimitiveType( EPrimitiveType _type )
        {
            switch( _type )
            {
            case PT_POINTLIST:
                return D3DPT_POINTLIST;
            case PT_LINELIST:
                return D3DPT_LINELIST;
            case PT_LINESTRIP:
                return D3DPT_LINESTRIP;
            case PT_TRIANGLELIST:
                return D3DPT_TRIANGLELIST;
            case PT_TRIANGLESTRIP:
                return D3DPT_TRIANGLESTRIP;
            case PT_TRIANGLEFAN:
                return D3DPT_TRIANGLEFAN;
            }

            return D3DPT_POINTLIST;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3DTEXTUREADDRESS toD3DTextureAddress( ETextureAddressMode _mode )
        {
            switch( _mode )
            {
            case TAM_WRAP:
                return D3DTADDRESS_WRAP;
            case TAM_MIRROR:
                return D3DTADDRESS_MIRROR;
            case TAM_CLAMP:
                return D3DTADDRESS_CLAMP;
            case TAM_BORDER:
                return D3DTADDRESS_BORDER;
            case TAM_MIRRORONCE:
                return D3DTADDRESS_MIRRORONCE;
            }

            return D3DTADDRESS_CLAMP;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3DCULL toD3DCullMode( ECullMode _mode )
        {
            switch( _mode )
            {
            case CM_CULL_NONE:
                return D3DCULL_NONE;
            case CM_CULL_CW:
                return D3DCULL_CW;
            case CM_CULL_CCW:
                return D3DCULL_CCW;
            }

            return D3DCULL_NONE;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3DCMPFUNC toD3DCmpFunc( ECompareFunction _func )
        {
            switch( _func )
            {
            case CMPF_ALWAYS_FAIL:
                return D3DCMP_NEVER;
            case CMPF_LESS:
                return D3DCMP_LESS;
            case CMPF_EQUAL:
                return D3DCMP_EQUAL;
            case CMPF_LESS_EQUAL:
                return D3DCMP_LESSEQUAL;
            case CMPF_GREATER:
                return D3DCMP_GREATER;
            case CMPF_NOT_EQUAL:
                return D3DCMP_NOTEQUAL;
            case CMPF_GREATER_EQUAL:
                return D3DCMP_GREATEREQUAL;
            case CMPF_ALWAYS_PASS:
                return D3DCMP_ALWAYS;
            }

            return D3DCMP_NEVER;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3DFILLMODE toD3DFillMode( EFillMode _mode )
        {
            switch( _mode )
            {
            case FM_POINT:
                return D3DFILL_POINT;
            case FM_WIREFRAME:
                return D3DFILL_WIREFRAME;
            case FM_SOLID:
                return D3DFILL_SOLID;
            }

            return D3DFILL_POINT;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3DSHADEMODE toD3DShadeMode( EShadeType _type )
        {
            switch( _type )
            {
            case SHT_FLAT:
                return D3DSHADE_FLAT;
            case SHT_GOURAUD:
                return D3DSHADE_GOURAUD;
            case SHT_PHONG:
                return D3DSHADE_PHONG;
            }

            return D3DSHADE_FLAT;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3DTEXTUREOP toD3DTextureOp( ETextureOp _textureOp )
        {
            switch( _textureOp )
            {
            case TOP_DISABLE:
                return D3DTOP_DISABLE;
            case TOP_SELECTARG1:
                return D3DTOP_SELECTARG1;
            case TOP_SELECTARG2:
                return D3DTOP_SELECTARG2;
            case TOP_MODULATE:
                return D3DTOP_MODULATE;
            case TOP_MODULATE2X:
                return D3DTOP_MODULATE2X;
            case TOP_MODULATE4X:
                return D3DTOP_MODULATE4X;
            case TOP_ADD:
                return D3DTOP_ADD;
            case TOP_SUBTRACT:
                return D3DTOP_SUBTRACT;
            }

            return D3DTOP_DISABLE;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE DWORD toD3DTextureArg( ETextureArgument _texArg )
        {
            switch( _texArg )
            {
            case TARG_CURRENT:
                return D3DTA_CURRENT;
            case TARG_DIFFUSE:
                return D3DTA_DIFFUSE;
            case TARG_SPECULAR:
                return D3DTA_SPECULAR;
            case TARG_TEXTURE:
                return D3DTA_TEXTURE;
            case TARG_TFACTOR:
                return D3DTA_TFACTOR;
            }

            return D3DTA_DIFFUSE;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3DSAMPLERSTATETYPE toD3DTextureFilterType( ETextureFilterType _filterType )
        {
            switch( _filterType )
            {
            case TFT_MAGNIFICATION:
                return D3DSAMP_MAGFILTER;
            case TFT_MINIFICATION:
                return D3DSAMP_MINFILTER;
            case TFT_MIPMAP:
                return D3DSAMP_MIPFILTER;
            }

            return D3DSAMP_MAGFILTER;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3DTEXTUREFILTERTYPE toD3DTextureFilter( ETextureFilter _filter )
        {
            switch( _filter )
            {
            case TF_NONE:
                return D3DTEXF_NONE;
            case TF_POINT:
                return D3DTEXF_POINT;
            case TF_LINEAR:
                return D3DTEXF_LINEAR;
            case TF_ANISOTROPIC:
                return D3DTEXF_ANISOTROPIC;
            case TF_FLATCUBIC:
                return D3DTEXF_PYRAMIDALQUAD;
            case TF_GAUSSIANCUBIC:
                return D3DTEXF_GAUSSIANQUAD;
            case __MAX_TEXTURE_FILTER__:
                break;
            }

            return D3DTEXF_NONE;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE uint32_t getD3DFormatRange( D3DFORMAT _format )
        {
            switch( _format )
            {
            case D3DFMT_R5G6B5:
                return 1;
            case D3DFMT_X1R5G5B5:
                return 2;
            case D3DFMT_A1R5G5B5:
                return 3;
            case D3DFMT_X8R8G8B8:
                return 4;
            case D3DFMT_A8R8G8B8:
                return 5;
            default:
                break;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE bool lessD3DFormats( D3DFORMAT _format1, D3DFORMAT _format2 )
        {
            uint32_t format_id1 = Helper::getD3DFormatRange( _format1 );
            uint32_t format_id2 = Helper::getD3DFormatRange( _format2 );

            return format_id1 < format_id2;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE const Char * getD3DFormatName( D3DFORMAT _format )
        {
            switch( _format )
            {
            case D3DFMT_R5G6B5:
                return "R5G6B5";
            case D3DFMT_X1R5G5B5:
                return "X1R5G5B5";
            case D3DFMT_A1R5G5B5:
                return "A1R5G5B5";
            case D3DFMT_X8R8G8B8:
                return "X8R8G8B8";
            case D3DFMT_A8R8G8B8:
                return "A8R8G8B8";
            default:
                break;
            }

            return "UNKNOWN";
        };
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE uint32_t getD3DFormatBits( D3DFORMAT _format )
        {
            switch( _format )
            {
            case D3DFMT_R5G6B5:
                return 16;
            case D3DFMT_X1R5G5B5:
                return 16;
            case D3DFMT_A1R5G5B5:
                return 16;
            case D3DFMT_X8R8G8B8:
                return 32;
            case D3DFMT_A8R8G8B8:
                return 32;
            default:
                break;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3DFORMAT getD3DIndexFormat()
        {
            switch( sizeof( RenderIndex ) )
            {
            case 4:
                return D3DFMT_INDEX32;
            case 2:
                return D3DFMT_INDEX16;
            default:
                break;
            }

            return D3DFMT_UNKNOWN;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}