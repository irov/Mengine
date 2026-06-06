#pragma once

#include "Interface/RenderProgramVariableInterface.h"
#include "Interface/RenderProgramInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenGLRenderProgramVariableInterface
        : public RenderProgramVariableInterface
    {
    public:
        virtual bool initialize( uint32_t _vertexCount, uint32_t _pixelCount ) = 0;
        virtual void finalize() = 0;

    public:
        virtual bool apply( const RenderProgramInterfacePtr & _program ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenGLRenderProgramVariableInterface, RenderProgramVariableInterface> OpenGLRenderProgramVariableInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
