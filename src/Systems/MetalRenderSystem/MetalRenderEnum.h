#pragma once

#include "Interface/RenderSystemInterface.h"

#import <Metal/Metal.h>

namespace Mengine
{
    namespace Helper
    {
        MTLBlendFactor toMTLBlendFactor( EBlendFactor _filter );
        MTLBlendOperation toMTLBlendOp( EBlendOp _filter );
        MTLCompareFunction toMTLCmpFunc( ECompareFunction _function );
        MTLSamplerMinMagFilter toMTLMagFilter( ETextureFilter _magFilter );
        MTLSamplerMinMagFilter toMTLMinFilter( ETextureFilter _minFilter );
        MTLSamplerMipFilter toMTLMipFilter( ETextureFilter _mipFilter );
        MTLPixelFormat toMTLPixelFormat( EPixelFormat _format );
        MTLPrimitiveType toMTLPrimitiveType( EPrimitiveType _mode );
        MTLSamplerAddressMode toMTLAddressMode( ETextureAddressMode _mode );
        MTLTriangleFillMode toMTLFillMode( EFillMode _op );
        MTLVertexFormat toMTLVertexFormat( EVertexAttributeType _type, uint32_t _size, bool _normalized );
        MTLCullMode toMTLCullMode( ECullMode _mode );
        MTLIndexType toMTLIndexType( uint32_t _size );
        MTLResourceOptions toMTLBufferOptions( EBufferType _type );
    }
}
