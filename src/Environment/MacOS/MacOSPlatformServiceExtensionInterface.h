#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/DocumentHelper.h"

#include "Config/Lambda.h"
#include "Config/UniqueId.h"

#if defined(MENGINE_ENVIRONMENT_RENDER_METAL)
#   import <Metal/Metal.h>
#endif

#import <AppKit/AppKit.h>

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

    public:
        typedef Lambda<bool( NSView * _view, NSEvent * _event )> LambdaMacOSEventHandler;

        virtual UniqueId addMacOSEventHandler( const LambdaMacOSEventHandler & _lambda, const DocumentInterfacePtr & _doc ) = 0;
        virtual void removeMacOSEventHandler( UniqueId _id ) = 0;

#if defined(MENGINE_ENVIRONMENT_RENDER_METAL)
    public:
        virtual id<MTLDevice> getMetalDevice() const = 0;
#endif
    };
}
