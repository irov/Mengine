#pragma once

#include "AppleHttpRequestThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleHttpRequestDeleteMessageThreadTask
        : public AppleHttpRequestThreadTask
    {
        DECLARE_FACTORABLE( AppleHttpRequestDeleteMessageThreadTask );

    public:
        AppleHttpRequestDeleteMessageThreadTask();
        ~AppleHttpRequestDeleteMessageThreadTask() override;

    public:
        bool initialize();

    protected:
        jobject _onHttp( JNIEnv * _jenv, jobject _basses ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequestDeleteMessageThreadTask, ThreadTaskInterface> AppleHttpRequestDeleteMessageThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}