#include "OpenGLRenderEnum.h"
#include "OpenGLRenderExtension.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        GLenum toGLIndexType( uint32_t _size )
        {
            switch( _size )
            {
            case 1: return GL_UNSIGNED_BYTE;
            case 2: return GL_UNSIGNED_SHORT;
            case 3: return GL_UNSIGNED_INT;
            default:;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        GLenum toGLBlendFactor( EBlendFactor _filter )
        {
            switch( _filter )
            {
            case BF_ONE: return GL_ONE;
            case BF_ZERO: return GL_ZERO;
            case BF_DEST_COLOUR: return GL_DST_COLOR;
            case BF_SOURCE_COLOUR: return GL_SRC_COLOR;
            case BF_ONE_MINUS_DEST_COLOUR: return GL_ONE_MINUS_DST_COLOR;
            case BF_ONE_MINUS_SOURCE_COLOUR: return GL_ONE_MINUS_SRC_COLOR;
            case BF_DEST_ALPHA: return GL_DST_ALPHA;
            case BF_SOURCE_ALPHA: return GL_SRC_ALPHA;
            case BF_ONE_MINUS_DEST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
            case BF_ONE_MINUS_SOURCE_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
            default:;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        GLenum toGLBlendOp( EBlendOp _filter )
        {
            switch( _filter )
            {
            case BOP_ADD: return GL_FUNC_ADD;
            case BOP_SUBTRACT: return GL_FUNC_SUBTRACT;
            case BOP_REVSUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;

#ifdef MENGINE_RENDER_OPENGL_ES
            case BOP_MIN: return GL_MIN_EXT;
            case BOP_MAX: return GL_MAX_EXT;
#else
            case BOP_MIN: return GL_MIN;
            case BOP_MAX: return GL_MAX;
#endif
            default:;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        GLenum toGLCmpFunc( ECompareFunction _function )
        {
            switch( _function )
            {
            case CMPF_ALWAYS_FAIL: return GL_NEVER;
            case CMPF_ALWAYS_PASS: return GL_ALWAYS;
            case CMPF_LESS: return GL_LESS;
            case CMPF_LESS_EQUAL: return GL_LEQUAL;
            case CMPF_EQUAL: return GL_EQUAL;
            case CMPF_NOT_EQUAL: return GL_NOTEQUAL;
            case CMPF_GREATER_EQUAL: return GL_GEQUAL;
            case CMPF_GREATER: return GL_GREATER;
            default:;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        GLenum toGLShadeMode( EShadeType _type )
        {
            switch( _type )
            {
            case SHT_FLAT: return GL_FLAT;
            case SHT_GOURAUD: return GL_SMOOTH;
            case SHT_PHONG: return GL_SMOOTH;
            default:;
            }
            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        GLenum toGLMagFilter( ETextureFilter _magFilter )
        {
            switch( _magFilter )
            {
            case TF_NONE:
            case TF_POINT:
                return GL_NEAREST;
                break;
            case TF_LINEAR:
            case TF_ANISOTROPIC:
            case TF_FLATCUBIC:
            case TF_GAUSSIANCUBIC:
                return GL_LINEAR;
                break;
            default:;
            };

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        GLenum toGLMinFilter( ETextureFilter _minFilter, ETextureFilter _mipFilter )
        {
            switch( _minFilter )
            {
            case TF_NONE:
            case TF_POINT:
                switch( _mipFilter )
                {
                case TF_NONE:
                    return GL_NEAREST;
                case TF_POINT:
                    return GL_NEAREST;
                case TF_ANISOTROPIC:
                case TF_LINEAR:
                case TF_FLATCUBIC:
                case TF_GAUSSIANCUBIC:
                    return GL_LINEAR;
                default:;
                }
                break;
            case TF_ANISOTROPIC:
            case TF_LINEAR:
            case TF_FLATCUBIC:
            case TF_GAUSSIANCUBIC:
                switch( _mipFilter )
                {
                case TF_NONE:
                    return GL_LINEAR;
                case TF_POINT:
                    return GL_LINEAR;
                case TF_ANISOTROPIC:
                case TF_LINEAR:
                case TF_FLATCUBIC:
                case TF_GAUSSIANCUBIC:
                    return GL_LINEAR;
                default:;
                }
                break;
            default:;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        GLenum toGLInternalFormat( EPixelFormat _format )
        {
            switch( _format )
            {
            case PF_X8B8G8R8:
            case PF_X8R8G8B8:
            case PF_A8B8G8R8:
            case PF_A8R8G8B8:
            case PF_B8G8R8A8:
            case PF_R8G8B8A8:
                return GL_RGBA;
            case PF_R8G8B8:
                return GL_RGB;
            case PF_A8:
                return GL_ALPHA;
            case PF_L8:
                return GL_LUMINANCE;
            case PF_ETC1:
                return GL_ETC1_RGB8_OES;
            case PF_PVRTC4_RGB:
                return GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
            case PF_PVRTC4_RGBA:
                return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
#ifdef MENGINE_PLATFORM_WINDOWS
            case PF_DXT1:
                return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
#endif
            default:;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        GLenum toGLColorFormat( EPixelFormat _format )
        {
            switch( _format )
            {
            case PF_R8G8B8:
                return GL_RGB;
            case PF_X8R8G8B8:
            case PF_A8R8G8B8:
                return GL_RGBA;
            case PF_A8:
                return GL_ALPHA;

            case PF_ETC1:
                return GL_ETC1_RGB8_OES;
            case PF_PVRTC4_RGB:
                return GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
            case PF_PVRTC4_RGBA:
                return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
#ifdef MENGINE_PLATFORM_WINDOWS
            case PF_DXT1:
                return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
#endif
            default:;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        GLenum toGLColorDataType( EPixelFormat _format )
        {
            switch( _format )
            {
            case PF_R8G8B8:
                return GL_UNSIGNED_BYTE;
            case PF_B8G8R8:
            case PF_B8G8R8A8:
            case PF_X8R8G8B8:
            case PF_A8R8G8B8:
                return GL_UNSIGNED_BYTE;
            case PF_A8:
                return GL_UNSIGNED_BYTE;
            case PF_DXT1:
            case PF_ETC1:
            case PF_PVRTC4_RGB:
            case PF_PVRTC4_RGBA:
                return GL_UNSIGNED_BYTE;
            default:;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        GLenum toGLPrimitiveMode( EPrimitiveType _mode )
        {
            switch( _mode )
            {
            case PT_POINTLIST:
                return GL_POINTS;
            case PT_LINELIST:
                return GL_LINES;
            case PT_LINESTRIP:
                return GL_LINE_STRIP;
            case PT_TRIANGLELIST:
                return GL_TRIANGLES;
            case PT_TRIANGLESTRIP:
                return GL_TRIANGLE_STRIP;
            case PT_TRIANGLEFAN:
                return GL_TRIANGLE_FAN;
            default:;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        GLenum toGLAddressMode( ETextureAddressMode _mode )
        {
            switch( _mode )
            {
            case TAM_CLAMP:
                return GL_CLAMP_TO_EDGE;
            case TAM_WRAP:
                return GL_REPEAT;
            default:;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        GLenum toGLTextureArg( ETextureArgument _arg )
        {
            switch( _arg )
            {
            case TARG_CURRENT:
                return GL_PREVIOUS;
            case TARG_DIFFUSE:
                return GL_PRIMARY_COLOR;
            case TARG_SPECULAR:
                return GL_PRIMARY_COLOR;
            case TARG_TFACTOR:
                return GL_CONSTANT;
            case TARG_TEXTURE:
                return GL_TEXTURE;
            default:;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        GLenum toGLTextureOp( ETextureOp _op )
        {
            switch( _op )
            {
            case TOP_DISABLE:
                return GL_REPLACE;
            case TOP_SELECTARG1:
                return GL_REPLACE;
            case TOP_SELECTARG2:
                return GL_REPLACE;
            case TOP_MODULATE:
                return GL_MODULATE;
            case TOP_ADD:
                return GL_ADD;
            case TOP_SUBTRACT:
                return GL_SUBTRACT;
            default:;
            }

            return 0;
        }
        //////////////////////////////////////////////////////////////////////////
        GLenum toGLFillMode( EFillMode _op )
        {
            MENGINE_UNUSED( _op );

#ifndef MENGINE_RENDER_OPENGL_ES
            switch( _op )
            {
            case FM_POINT:
                return GL_POINT;
            case FM_WIREFRAME:
                return GL_LINE;
            case FM_SOLID:
                return GL_FILL;
            default:;
            }
#endif
            return GL_NONE;
        }
        //////////////////////////////////////////////////////////////////////////
        GLenum toGLVertexAttributeType( EVertexAttributeType _type )
        {
            switch( _type )
            {
            case VAT_FLOAT:
                return GL_FLOAT;
            case VAT_UNSIGNED_BYTE:
                return GL_UNSIGNED_BYTE;
            }

            return GL_NONE;
        }
        //////////////////////////////////////////////////////////////////////////
        GLenum toGLBufferType( EBufferType _type )
        {
            switch( _type )
            {
            case BT_STATIC:
                return GL_STATIC_DRAW;
            case BT_STREAM:
                return GL_STREAM_DRAW;
            case BT_DYNAMIC:
                return GL_DYNAMIC_DRAW;
            }

            return GL_NONE;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
