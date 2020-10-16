#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusiveLinked.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenGLRenderResourceHandler
        : public Mixin
        , public IntrusiveLinked<OpenGLRenderResourceHandler>
    {
    public:
        OpenGLRenderResourceHandler();
        ~OpenGLRenderResourceHandler() override;

    public:
        virtual void onRenderReset() = 0;
        virtual bool onRenderRestore() = 0;
    };
}