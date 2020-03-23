#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    class RenderExternalInterface
        : public Mixin
    {
    public:
        virtual void onRenderExternal() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderExternalInterface> RenderExternalInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}