#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ApplicationLifecycleInterface
        : public Mixin
    {
    public:
        virtual void onApplicationDidBecomeActive() = 0;
        virtual void onApplicationWillEnterForeground() = 0;
        virtual void onApplicationDidEnterBackground() = 0;
        virtual void onApplicationWillResignActive() = 0;
        virtual void onApplicationWillTerminate() = 0;
        virtual void onApplicationDidReceiveMemoryWarning() = 0;
        virtual void onApplicationDidReceiveTrimMemory( int32_t _level ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
