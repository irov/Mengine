#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    class RenderPlatformInterface
        : public Mixin
    {
    public:
        virtual bool renderPlatform() = 0;
        virtual void notifyVsyncChanged( bool _vsync ) = 0;
    };
}
