#pragma once

#include "AppleHttpRequestThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleHttpRequestGetMessageThreadTask
        : public AppleHttpRequestThreadTask
    {
        DECLARE_FACTORABLE( AppleHttpRequestGetMessageThreadTask );

    public:
        AppleHttpRequestGetMessageThreadTask();
        ~AppleHttpRequestGetMessageThreadTask() override;

    public:
        bool initialize();

    protected:
        jobject _onHttp( JNIEnv * _jenv, jobject _basses ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequestGetMessageThreadTask, ThreadTaskInterface> AppleHttpRequestGetMessageThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}