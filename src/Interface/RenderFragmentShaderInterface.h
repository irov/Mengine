#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderFragmentShaderInterface
        : public Mixin
    {
    public:
        virtual const ConstString & getName() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderFragmentShaderInterface> RenderFragmentShaderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
