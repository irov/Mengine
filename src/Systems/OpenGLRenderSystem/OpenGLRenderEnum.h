#pragma once

#include "Interface/RenderSystemInterface.h"

#include "Environment/OpenGL/OpenGLRenderIncluder.h"

namespace Mengine
{
    namespace Helper
    {
        GLenum toGLIndexType( uint32_t _size );
        GLenum toGLBlendFactor( EBlendFactor _filter );
        GLenum toGLBlendOp( EBlendOp _filter );
        GLenum toGLCmpFunc( ECompareFunction _function );
        GLenum toGLMagFilter( ETextureFilter _magFilter );
        GLenum toGLMinFilter( ETextureFilter _minFilter, ETextureFilter _mipFilter );
        GLenum toGLInternalFormat( EPixelFormat _format );
        GLenum toGLColorFormat( EPixelFormat _format );
        GLenum toGLColorDataType( EPixelFormat _format );
        GLenum toGLPrimitiveMode( EPrimitiveType _mode );
        GLenum toGLAddressMode( ETextureAddressMode _mode );
        GLenum toGLFillMode( EFillMode _op );
        GLenum toGLVertexAttributeType( EVertexAttributeType _type );
        GLenum toGLBufferType( EBufferType _type );
    }
}
