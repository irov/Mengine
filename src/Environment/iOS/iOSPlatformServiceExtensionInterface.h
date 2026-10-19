#pragma once

#include "Interface/UnknownInterface.h"

#import <UIKit/UIKit.h>

namespace Mengine
{
    class iOSPlatformServiceExtensionInterface
        : public UnknownInterface
    {
    public:
        virtual void handleApplicationDidBecomeActive() = 0;
        virtual void handleApplicationWillEnterForeground() = 0;
        virtual void handleApplicationDidEnterBackground() = 0;
        virtual void handleApplicationWillResignActive() = 0;
        virtual void handleApplicationWillTerminate() = 0;

    public:
        virtual UIWindow * getUIWindow() const = 0;
        virtual UIView * getUIView() const = 0;
    };
}
