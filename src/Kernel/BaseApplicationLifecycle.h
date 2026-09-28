#pragma once

#include "Interface/ApplicationLifecycleInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BaseApplicationLifecycle
        : public ApplicationLifecycleInterface
    {
    public:
        BaseApplicationLifecycle();
        ~BaseApplicationLifecycle() override;

    protected:
        virtual void _onApplicationDidBecomeActive();
        virtual void _onApplicationWillEnterForeground();
        virtual void _onApplicationDidEnterBackground();
        virtual void _onApplicationWillResignActive();
        virtual void _onApplicationWillTerminate();
        virtual void _onApplicationDidReceiveMemoryWarning();
        virtual void _onApplicationDidReceiveTrimMemory( int32_t _level );

    private:
        void onApplicationDidBecomeActive() override;
        void onApplicationWillEnterForeground() override;
        void onApplicationDidEnterBackground() override;
        void onApplicationWillResignActive() override;
        void onApplicationWillTerminate() override;
        void onApplicationDidReceiveMemoryWarning() override;
        void onApplicationDidReceiveTrimMemory( int32_t _level ) override;
    };
    //////////////////////////////////////////////////////////////////////////
}
