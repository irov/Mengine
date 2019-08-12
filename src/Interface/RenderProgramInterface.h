#pragma once

#include "Interface/RenderVertexAttributeInterface.h"
#include "Interface/RenderVertexShaderInterface.h"
#include "Interface/RenderFragmentShaderInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    class RenderProgramInterface
        : public Mixin
    {
    public:
        virtual const ConstString & getName() const = 0;

    public:
        virtual RenderVertexAttributeInterfacePtr getVertexAttribute() const = 0;
        virtual RenderVertexShaderInterfacePtr getVertexShader() const = 0;
        virtual RenderFragmentShaderInterfacePtr getFragmentShader() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderProgramInterface> RenderProgramInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
