#pragma once

#include "Interface/RenderEnumInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

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
        MENGINE_INLINE uint32_t getMultiSampleType( uint32_t _count )
        {
            return _count;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3D11_MAP toD3DBufferLock( EBufferLockFlag _flag )
        {
            switch( _flag )
            {
            case BLF_LOCK_NONE:
                return (D3D11_MAP)0;
            case BLF_LOCK_READONLY:
                return D3D11_MAP_READ;
            case BLF_LOCK_DISCARD:
                return D3D11_MAP_WRITE_DISCARD;
            case BLF_LOCK_NOOVERWRITE:
                return D3D11_MAP_WRITE_NO_OVERWRITE;
            case BLF_LOCK_NOSYSLOCK:
                return (D3D11_MAP)0;
            }

            return (D3D11_MAP)0;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE DXGI_FORMAT toD3DFormat( EPixelFormat _format )
        {
            switch( _format )
            {
            case PF_L8:
                return DXGI_FORMAT_R8_UNORM;
            case PF_A8:
                return DXGI_FORMAT_A8_UNORM;
            case PF_A4L4:
                return DXGI_FORMAT_UNKNOWN;
            case PF_BYTE_LA:
                return DXGI_FORMAT_UNKNOWN; // Assume little endian here
            case PF_R3G3B2:
                return DXGI_FORMAT_UNKNOWN;
            case PF_A1R5G5B5:
                return DXGI_FORMAT_B5G5R5A1_UNORM;
            case PF_R5G6B5:
                return DXGI_FORMAT_B5G6R5_UNORM;
            case PF_A4R4G4B4:
                return DXGI_FORMAT_B4G4R4A4_UNORM;
            case PF_R8G8B8:
                return DXGI_FORMAT_B8G8R8X8_UNORM;
            case PF_A8R8G8B8:
                return DXGI_FORMAT_B8G8R8A8_UNORM;
            case PF_X8R8G8B8:
                return DXGI_FORMAT_B8G8R8X8_UNORM;
            case PF_SHORT_GR:
                return DXGI_FORMAT_R16G16_UNORM;
            case PF_DXT1:
                return DXGI_FORMAT_BC1_UNORM; // to support SRGB needs to use DXGI_FORMAT_BC1_UNORM_SRGB instead
            case PF_DXT2:
                return DXGI_FORMAT_BC2_UNORM;
            case PF_DXT3:
                return DXGI_FORMAT_BC3_UNORM;
            case PF_DXT4:
                return DXGI_FORMAT_BC4_UNORM;
            case PF_DXT5:
                return DXGI_FORMAT_BC5_UNORM;
            case PF_UNKNOWN:
                return DXGI_FORMAT_UNKNOWN;
            default:
                break;
            }

            return DXGI_FORMAT_UNKNOWN;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE EPixelFormat toPixelFormat( DXGI_FORMAT _dxgiFormat )
        {
            switch( _dxgiFormat )
            {
            case DXGI_FORMAT_R8_UNORM:
                return PF_L8;
            case DXGI_FORMAT_A8_UNORM:
                return PF_A8;
            case DXGI_FORMAT_B5G5R5A1_UNORM:
                return PF_A1R5G5B5;
            case DXGI_FORMAT_B5G6R5_UNORM:
                return PF_R5G6B5;
            case DXGI_FORMAT_B4G4R4A4_UNORM:
                return PF_A4R4G4B4;
            case DXGI_FORMAT_B8G8R8A8_UNORM:
                return PF_A8R8G8B8;
            case DXGI_FORMAT_B8G8R8X8_UNORM:
                return PF_X8R8G8B8;
            case DXGI_FORMAT_R16G16_UNORM:
                return PF_SHORT_GR;
            case DXGI_FORMAT_BC1_UNORM:
                return PF_DXT1; // to support SRGB needs to use DXGI_FORMAT_BC1_UNORM_SRGB instead
            case DXGI_FORMAT_BC2_UNORM:
                return PF_DXT2;
            case DXGI_FORMAT_BC3_UNORM:
                return PF_DXT3;
            case DXGI_FORMAT_BC4_UNORM:
                return PF_DXT4;
            case DXGI_FORMAT_BC5_UNORM:
                return PF_DXT5;
            default:
                break;
            }

            return PF_UNKNOWN;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3D11_BLEND toD3DBlendFactor( EBlendFactor _blend )
        {
            switch( _blend )
            {
            case BF_ONE:
                return D3D11_BLEND_ONE;
            case BF_ZERO:
                return D3D11_BLEND_ZERO;
            case BF_DEST_COLOUR:
                return D3D11_BLEND_DEST_COLOR;
            case BF_SOURCE_COLOUR:
                return D3D11_BLEND_SRC_COLOR;
            case BF_ONE_MINUS_DEST_COLOUR:
                return D3D11_BLEND_INV_DEST_COLOR;
            case BF_ONE_MINUS_SOURCE_COLOUR:
                return D3D11_BLEND_INV_SRC_COLOR;
            case BF_DEST_ALPHA:
                return D3D11_BLEND_DEST_ALPHA;
            case BF_SOURCE_ALPHA:
                return D3D11_BLEND_SRC_ALPHA;
            case BF_ONE_MINUS_DEST_ALPHA:
                return D3D11_BLEND_INV_DEST_ALPHA;
            case BF_ONE_MINUS_SOURCE_ALPHA:
                return D3D11_BLEND_INV_SRC_ALPHA;
            }

            return D3D11_BLEND_ZERO;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3D11_BLEND_OP toD3DBlendOp( EBlendOp _blend )
        {
            switch( _blend )
            {
            case BOP_ADD:
                return D3D11_BLEND_OP_ADD;
            case BOP_SUBTRACT:
                return D3D11_BLEND_OP_SUBTRACT;
            case BOP_REVSUBTRACT:
                return D3D11_BLEND_OP_REV_SUBTRACT;
            case BOP_MIN:
                return D3D11_BLEND_OP_MIN;
            case BOP_MAX:
                return D3D11_BLEND_OP_MAX;
            }

            return D3D11_BLEND_OP_ADD;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3D11_PRIMITIVE_TOPOLOGY toD3DPrimitiveType( EPrimitiveType _type )
        {
            switch( _type )
            {
            case PT_POINTLIST:
                return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
            case PT_LINELIST:
                return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
            case PT_LINESTRIP:
                return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
            case PT_TRIANGLELIST:
                return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            case PT_TRIANGLESTRIP:
                return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            case PT_TRIANGLEFAN:
                return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
            }

            return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3D11_TEXTURE_ADDRESS_MODE toD3DTextureAddress( ETextureAddressMode _mode )
        {
            switch( _mode )
            {
            case TAM_WRAP:
                return D3D11_TEXTURE_ADDRESS_WRAP;
            case TAM_MIRROR:
                return D3D11_TEXTURE_ADDRESS_MIRROR;
            case TAM_CLAMP:
                return D3D11_TEXTURE_ADDRESS_CLAMP;
            case TAM_BORDER:
                return D3D11_TEXTURE_ADDRESS_BORDER;
            case TAM_MIRRORONCE:
                return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
            }

            return D3D11_TEXTURE_ADDRESS_CLAMP;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3D11_CULL_MODE toD3DCullMode( ECullMode _mode )
        {
            switch( _mode )
            {
            case CM_CULL_NONE:
                return D3D11_CULL_NONE;
            case CM_CULL_CW:
                return D3D11_CULL_FRONT;
            case CM_CULL_CCW:
                return D3D11_CULL_BACK;
            }

            return D3D11_CULL_NONE;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3D11_COMPARISON_FUNC toD3DCmpFunc( ECompareFunction _func )
        {
            switch( _func )
            {
            case CMPF_ALWAYS_FAIL:
                return D3D11_COMPARISON_NEVER;
            case CMPF_LESS:
                return D3D11_COMPARISON_LESS;
            case CMPF_EQUAL:
                return D3D11_COMPARISON_EQUAL;
            case CMPF_LESS_EQUAL:
                return D3D11_COMPARISON_LESS_EQUAL;
            case CMPF_GREATER:
                return D3D11_COMPARISON_GREATER;
            case CMPF_NOT_EQUAL:
                return D3D11_COMPARISON_NOT_EQUAL;
            case CMPF_GREATER_EQUAL:
                return D3D11_COMPARISON_GREATER_EQUAL;
            case CMPF_ALWAYS_PASS:
                return D3D11_COMPARISON_ALWAYS;
            }

            return D3D11_COMPARISON_NEVER;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3D11_FILL_MODE toD3DFillMode( EFillMode _mode )
        {
            switch( _mode )
            {
            case FM_POINT:
                return D3D11_FILL_SOLID;
            case FM_WIREFRAME:
                return D3D11_FILL_WIREFRAME;
            case FM_SOLID:
                return D3D11_FILL_SOLID;
            }

            return D3D11_FILL_SOLID;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE D3D11_FILTER toD3DTextureFilter( ETextureFilter _filter )
        {
            switch( _filter )
            {
            case TF_NONE:
                return D3D11_FILTER_MIN_MAG_MIP_POINT;
            case TF_POINT:
                return D3D11_FILTER_MIN_MAG_MIP_POINT;
            case TF_LINEAR:
                return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            case TF_ANISOTROPIC:
                return D3D11_FILTER_ANISOTROPIC;
            case TF_FLATCUBIC:
                return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            case TF_GAUSSIANCUBIC:
                return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            case __MAX_TEXTURE_FILTER__:
                break;
            }

            return D3D11_FILTER_MIN_MAG_MIP_POINT;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE uint32_t getD3DFormatRange( DXGI_FORMAT _format )
        {
            switch( _format )
            {
            case DXGI_FORMAT_B5G6R5_UNORM:
                return 1;
            case DXGI_FORMAT_B5G5R5A1_UNORM:
                return 2;
            case DXGI_FORMAT_B8G8R8X8_UNORM:
                return 3;
            case DXGI_FORMAT_B8G8R8A8_UNORM:
                return 4;
            default:
                break;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE bool lessD3DFormats( DXGI_FORMAT _format1, DXGI_FORMAT _format2 )
        {
            uint32_t format_id1 = getD3DFormatRange( _format1 );
            uint32_t format_id2 = getD3DFormatRange( _format2 );

            return format_id1 < format_id2;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE const Char * getD3DFormatName( DXGI_FORMAT _format )
        {
            switch( _format )
            {
            case DXGI_FORMAT_B5G6R5_UNORM:
                return "R5G6B5";
            case DXGI_FORMAT_B5G5R5A1_UNORM:
                return "A1R5G5B5";
            case DXGI_FORMAT_B8G8R8X8_UNORM:
                return "X8R8G8B8";
            case DXGI_FORMAT_B8G8R8A8_UNORM:
                return "A8R8G8B8";
            default:
                break;
            }

            return "UNKNOWN";
        };
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE uint32_t getD3DFormatBits( DXGI_FORMAT _format )
        {
            switch( _format )
            {
            case DXGI_FORMAT_B5G6R5_UNORM:
                return 16;
            case DXGI_FORMAT_B5G5R5A1_UNORM:
                return 16;
            case DXGI_FORMAT_B8G8R8X8_UNORM:
                return 32;
            case DXGI_FORMAT_B8G8R8A8_UNORM:
                return 32;
            default:
                break;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE DXGI_FORMAT getD3DIndexFormat()
        {
            switch( sizeof( RenderIndex ) )
            {
            case 4:
                return DXGI_FORMAT_R32_UINT;
            case 2:
                return DXGI_FORMAT_R16_UINT;
            default:
                break;
            }

            return DXGI_FORMAT_UNKNOWN;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}