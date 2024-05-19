#pragma once

#include "AndroidHttpRequestThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidHttpRequestGetMessageThreadTask
        : public AndroidHttpRequestThreadTask
    {
        DECLARE_FACTORABLE( AndroidHttpRequestGetMessageThreadTask );

    public:
        AndroidHttpRequestGetMessageThreadTask();
        ~AndroidHttpRequestGetMessageThreadTask() override;

    public:
        bool initialize();

    protected:
        jobject _onHttp( JNIEnv * _jenv, jobject _basses ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidHttpRequestGetMessageThreadTask, ThreadTaskInterface> AndroidHttpRequestGetMessageThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}