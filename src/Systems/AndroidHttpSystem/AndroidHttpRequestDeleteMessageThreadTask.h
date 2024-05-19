#pragma once

#include "AndroidHttpRequestThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidHttpRequestDeleteMessageThreadTask
        : public AndroidHttpRequestThreadTask
    {
        DECLARE_FACTORABLE( AndroidHttpRequestDeleteMessageThreadTask );

    public:
        AndroidHttpRequestDeleteMessageThreadTask();
        ~AndroidHttpRequestDeleteMessageThreadTask() override;

    public:
        bool initialize();

    protected:
        jobject _onHttp( JNIEnv * _jenv, jobject _basses ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidHttpRequestDeleteMessageThreadTask, ThreadTaskInterface> AndroidHttpRequestDeleteMessageThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}