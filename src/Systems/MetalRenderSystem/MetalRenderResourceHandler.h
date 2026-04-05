#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusiveLinked.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MetalRenderResourceHandler
        : public Mixin
        , public IntrusiveLinked<MetalRenderResourceHandler>
    {
    public:
        MetalRenderResourceHandler();
        ~MetalRenderResourceHandler() override;

    public:
        virtual void onRenderReset() = 0;
        virtual bool onRenderRestore() = 0;
    };
}
