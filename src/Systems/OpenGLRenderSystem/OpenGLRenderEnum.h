#pragma once

#include "Interface/RenderSystemInterface.h"

#include "Environment/OpenGL/OpenGLRenderIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GLenum s_getGLIndexType( uint32_t _size );
    GLenum s_toGLBlendFactor( EBlendFactor _filter );
    GLenum s_toGLBlendFactor( EBlendOp _filter );
    GLenum s_toGLCmpFunc( ECompareFunction _function );
    GLenum s_toGLShadeMode( EShadeType _type );
    GLenum s_toMagFilter( ETextureFilter _magFilter );
    GLenum s_toGLMinFilter( ETextureFilter _minFilter, ETextureFilter _mipFilter );
    GLenum s_toGLInternalFormat( EPixelFormat _format );
    GLenum s_toGLColorFormat( EPixelFormat _format );
    GLenum s_getGLColorDataType( EPixelFormat _format );
    GLenum s_getGLPrimitiveMode( EPrimitiveType _mode );
    GLenum s_getGLAddressMode( ETextureAddressMode _mode );
    GLenum s_getGLTextureArg( ETextureArgument _arg );
    GLenum s_getGLTextureOp( ETextureOp _op );
    GLenum s_getGLFillMode( EFillMode _op );
    GLenum s_getGLVertexAttributeType( EVertexAttributeType _type );
    GLenum s_getGLBufferType( EBufferType _type );
}
