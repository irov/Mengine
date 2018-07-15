#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderFragmentShaderInterface
        : public ServantInterface
    {
    public:
        virtual const ConstString & getName() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderFragmentShaderInterface> RenderFragmentShaderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
