#pragma once

#include "Interface/UnknownInterface.h"

#include "Config/UniqueId.h"

#if defined(MENGINE_ENVIRONMENT_RENDER_METAL)
#   import <Metal/Metal.h>
#endif

#if defined(__OBJC__)
@class NSWindow;
@class NSView;
#else
typedef struct objc_object NSWindow;
typedef struct objc_object NSView;
#endif

namespace Mengine
{
    class MacOSPlatformServiceExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual void handleApplicationDidBecomeActive() = 0;
        virtual void handleApplicationWillResignActive() = 0;
        virtual void handleApplicationWillTerminate() = 0;

    public:
        virtual void handleWindowDidResize() = 0;
        virtual void handleWindowDidBecomeKey() = 0;
        virtual void handleWindowDidResignKey() = 0;
        virtual void handleWindowDidMiniaturize() = 0;
        virtual void handleWindowDidDeminiaturize() = 0;
        virtual void handleWindowWillClose() = 0;

    public:
        virtual NSWindow * getNSWindow() const = 0;
        virtual NSView * getNSView() const = 0;

#if defined(MENGINE_ENVIRONMENT_RENDER_METAL)
    public:
        virtual id<MTLDevice> getMetalDevice() const = 0;
#endif
    };
}
