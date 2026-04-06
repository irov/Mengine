#pragma once

#include "Interface/UnknownInterface.h"

#import <UIKit/UIKit.h>

#if defined(MENGINE_ENVIRONMENT_RENDER_METAL)
#   import <Metal/Metal.h>
#endif

#include "Config/UniqueId.h"

namespace Mengine
{
    class iOSPlatformServiceExtensionInterface
        : virtual public UnknownInterface
    {
    public:
        virtual void handleApplicationDidBecomeActive() = 0;
        virtual void handleApplicationWillEnterForeground() = 0;
        virtual void handleApplicationDidEnterBackground() = 0;
        virtual void handleApplicationWillResignActive() = 0;
        virtual void handleApplicationWillTerminate() = 0;

    public:
        virtual UIWindow * getUIWindow() const = 0;

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
    public:
        virtual EAGLContext * getEAGLContext() const = 0;
#elif defined(MENGINE_ENVIRONMENT_RENDER_METAL)
    public:
        virtual id<MTLDevice> getMetalDevice() const = 0;
        virtual UIView * getMetalView() const = 0;
#endif

    public:
        virtual void handleTouchBegan( NSSet<UITouch *> * _touches, UIView * _view ) = 0;
        virtual void handleTouchMoved( NSSet<UITouch *> * _touches, UIView * _view ) = 0;
        virtual void handleTouchEnded( NSSet<UITouch *> * _touches, UIView * _view ) = 0;
        virtual void handleTouchCancelled( NSSet<UITouch *> * _touches, UIView * _view ) = 0;
    };
}
