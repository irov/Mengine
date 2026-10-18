#pragma once

#include "Environment/MacOS/MacOSPlatformServiceExtensionInterface.h"

#if defined(MENGINE_ENVIRONMENT_RENDER_METAL)
#   import <Metal/Metal.h>
#endif

namespace Mengine
{
    class MacOSRenderPlatformServiceExtensionInterface
        : public MacOSPlatformServiceExtensionInterface
    {
#if defined(MENGINE_ENVIRONMENT_RENDER_METAL)
    public:
        virtual id<MTLDevice> getMetalDevice() const = 0;
#endif
    };
}
