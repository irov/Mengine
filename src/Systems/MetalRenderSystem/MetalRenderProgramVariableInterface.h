#pragma once

#include "Interface/RenderProgramVariableInterface.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MetalRenderProgramVariableInterface
        : public RenderProgramVariableInterface
    {
    public:
        virtual bool initialize( uint32_t _vertexCount, uint32_t _pixelCount ) = 0;
        virtual void finalize() = 0;

    public:
        virtual bool apply( id<MTLRenderCommandEncoder> _encoder ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MetalRenderProgramVariableInterface, RenderProgramVariableInterface> MetalRenderProgramVariableInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
