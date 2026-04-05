#include "MetalRenderEnum.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        MTLBlendFactor toMTLBlendFactor( EBlendFactor _filter )
        {
            switch( _filter )
            {
            case BF_ONE: return MTLBlendFactorOne;
            case BF_ZERO: return MTLBlendFactorZero;
            case BF_DEST_COLOUR: return MTLBlendFactorDestinationColor;
            case BF_SOURCE_COLOUR: return MTLBlendFactorSourceColor;
            case BF_ONE_MINUS_DEST_COLOUR: return MTLBlendFactorOneMinusDestinationColor;
            case BF_ONE_MINUS_SOURCE_COLOUR: return MTLBlendFactorOneMinusSourceColor;
            case BF_DEST_ALPHA: return MTLBlendFactorDestinationAlpha;
            case BF_SOURCE_ALPHA: return MTLBlendFactorSourceAlpha;
            case BF_ONE_MINUS_DEST_ALPHA: return MTLBlendFactorOneMinusDestinationAlpha;
            case BF_ONE_MINUS_SOURCE_ALPHA: return MTLBlendFactorOneMinusSourceAlpha;
            default:;
            }

            return MTLBlendFactorZero;
        }
        //////////////////////////////////////////////////////////////////////////
        MTLBlendOperation toMTLBlendOp( EBlendOp _filter )
        {
            switch( _filter )
            {
            case BOP_ADD: return MTLBlendOperationAdd;
            case BOP_SUBTRACT: return MTLBlendOperationSubtract;
            case BOP_REVSUBTRACT: return MTLBlendOperationReverseSubtract;
            case BOP_MIN: return MTLBlendOperationMin;
            case BOP_MAX: return MTLBlendOperationMax;
            default:;
            }

            return MTLBlendOperationAdd;
        }
        //////////////////////////////////////////////////////////////////////////
        MTLCompareFunction toMTLCmpFunc( ECompareFunction _function )
        {
            switch( _function )
            {
            case CMPF_ALWAYS_FAIL: return MTLCompareFunctionNever;
            case CMPF_ALWAYS_PASS: return MTLCompareFunctionAlways;
            case CMPF_LESS: return MTLCompareFunctionLess;
            case CMPF_LESS_EQUAL: return MTLCompareFunctionLessEqual;
            case CMPF_EQUAL: return MTLCompareFunctionEqual;
            case CMPF_NOT_EQUAL: return MTLCompareFunctionNotEqual;
            case CMPF_GREATER_EQUAL: return MTLCompareFunctionGreaterEqual;
            case CMPF_GREATER: return MTLCompareFunctionGreater;
            default:;
            }

            return MTLCompareFunctionAlways;
        }
        //////////////////////////////////////////////////////////////////////////
        MTLSamplerMinMagFilter toMTLMagFilter( ETextureFilter _magFilter )
        {
            switch( _magFilter )
            {
            case TF_NONE:
            case TF_POINT:
                return MTLSamplerMinMagFilterNearest;
            case TF_LINEAR:
            case TF_ANISOTROPIC:
            case TF_FLATCUBIC:
            case TF_GAUSSIANCUBIC:
                return MTLSamplerMinMagFilterLinear;
            default:;
            }

            return MTLSamplerMinMagFilterNearest;
        }
        //////////////////////////////////////////////////////////////////////////
        MTLSamplerMinMagFilter toMTLMinFilter( ETextureFilter _minFilter )
        {
            switch( _minFilter )
            {
            case TF_NONE:
            case TF_POINT:
                return MTLSamplerMinMagFilterNearest;
            case TF_LINEAR:
            case TF_ANISOTROPIC:
            case TF_FLATCUBIC:
            case TF_GAUSSIANCUBIC:
                return MTLSamplerMinMagFilterLinear;
            default:;
            }

            return MTLSamplerMinMagFilterNearest;
        }
        //////////////////////////////////////////////////////////////////////////
        MTLSamplerMipFilter toMTLMipFilter( ETextureFilter _mipFilter )
        {
            switch( _mipFilter )
            {
            case TF_NONE:
                return MTLSamplerMipFilterNotMipmapped;
            case TF_POINT:
                return MTLSamplerMipFilterNearest;
            case TF_LINEAR:
            case TF_ANISOTROPIC:
            case TF_FLATCUBIC:
            case TF_GAUSSIANCUBIC:
                return MTLSamplerMipFilterLinear;
            default:;
            }

            return MTLSamplerMipFilterNotMipmapped;
        }
        //////////////////////////////////////////////////////////////////////////
        MTLPixelFormat toMTLPixelFormat( EPixelFormat _format )
        {
            switch( _format )
            {
            case PF_A8R8G8B8:
            case PF_X8R8G8B8:
                return MTLPixelFormatBGRA8Unorm;
            case PF_R8G8B8A8:
            case PF_A8B8G8R8:
            case PF_X8B8G8R8:
                return MTLPixelFormatRGBA8Unorm;
            case PF_B8G8R8A8:
                return MTLPixelFormatBGRA8Unorm;
            case PF_R8G8B8:
                return MTLPixelFormatRGBA8Unorm;
            case PF_A8:
                return MTLPixelFormatA8Unorm;
            case PF_L8:
                return MTLPixelFormatR8Unorm;
#if defined(MENGINE_PLATFORM_IOS) || defined(MENGINE_PLATFORM_MACOS)
            case PF_PVRTC4_RGB:
            case PF_PVRTC4_RGBA:
                return MTLPixelFormatPVRTC_RGBA_4BPP;
#endif
            default:;
            }

            return MTLPixelFormatInvalid;
        }
        //////////////////////////////////////////////////////////////////////////
        MTLPrimitiveType toMTLPrimitiveType( EPrimitiveType _mode )
        {
            switch( _mode )
            {
            case PT_POINTLIST: return MTLPrimitiveTypePoint;
            case PT_LINELIST: return MTLPrimitiveTypeLine;
            case PT_LINESTRIP: return MTLPrimitiveTypeLineStrip;
            case PT_TRIANGLELIST: return MTLPrimitiveTypeTriangle;
            case PT_TRIANGLESTRIP: return MTLPrimitiveTypeTriangleStrip;
            case PT_TRIANGLEFAN: return MTLPrimitiveTypeTriangle;
            default:;
            }

            return MTLPrimitiveTypeTriangle;
        }
        //////////////////////////////////////////////////////////////////////////
        MTLSamplerAddressMode toMTLAddressMode( ETextureAddressMode _mode )
        {
            switch( _mode )
            {
            case TAM_WRAP: return MTLSamplerAddressModeRepeat;
            case TAM_MIRROR: return MTLSamplerAddressModeMirrorRepeat;
            case TAM_CLAMP: return MTLSamplerAddressModeClampToEdge;
            case TAM_BORDER: return MTLSamplerAddressModeClampToZero;
            case TAM_MIRRORONCE: return MTLSamplerAddressModeMirrorClampToEdge;
            default:;
            }

            return MTLSamplerAddressModeClampToEdge;
        }
        //////////////////////////////////////////////////////////////////////////
        MTLTriangleFillMode toMTLFillMode( EFillMode _op )
        {
            switch( _op )
            {
            case FM_POINT:
            case FM_WIREFRAME:
                return MTLTriangleFillModeLines;
            case FM_SOLID:
                return MTLTriangleFillModeFill;
            default:;
            }

            return MTLTriangleFillModeFill;
        }
        //////////////////////////////////////////////////////////////////////////
        MTLVertexFormat toMTLVertexFormat( EVertexAttributeType _type, uint32_t _size )
        {
            switch( _type )
            {
            case VAT_FLOAT:
                switch( _size )
                {
                case 1: return MTLVertexFormatFloat;
                case 2: return MTLVertexFormatFloat2;
                case 3: return MTLVertexFormatFloat3;
                case 4: return MTLVertexFormatFloat4;
                default: return MTLVertexFormatInvalid;
                }
            case VAT_UNSIGNED_BYTE:
                switch( _size )
                {
                case 4: return MTLVertexFormatUChar4Normalized;
                default: return MTLVertexFormatInvalid;
                }
            default:;
            }

            return MTLVertexFormatInvalid;
        }
        //////////////////////////////////////////////////////////////////////////
        MTLCullMode toMTLCullMode( ECullMode _mode )
        {
            switch( _mode )
            {
            case CM_CULL_NONE: return MTLCullModeNone;
            case CM_CULL_CW: return MTLCullModeFront;
            case CM_CULL_CCW: return MTLCullModeBack;
            default:;
            }

            return MTLCullModeNone;
        }
        //////////////////////////////////////////////////////////////////////////
        MTLIndexType toMTLIndexType( uint32_t _size )
        {
            switch( _size )
            {
            case 2: return MTLIndexTypeUInt16;
            case 4: return MTLIndexTypeUInt32;
            default:;
            }

            return MTLIndexTypeUInt16;
        }
        //////////////////////////////////////////////////////////////////////////
        MTLResourceOptions toMTLBufferOptions( EBufferType _type )
        {
            switch( _type )
            {
            case BT_STATIC:
                return MTLResourceStorageModeShared;
            case BT_STREAM:
            case BT_DYNAMIC:
                return MTLResourceStorageModeShared | MTLResourceCPUCacheModeWriteCombined;
            }

            return MTLResourceStorageModeShared;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
