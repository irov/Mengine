#pragma once

#include "AppleHttpRequestThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleHttpRequestPostMessageThreadTask
        : public AppleHttpRequestThreadTask
    {
        DECLARE_FACTORABLE( AppleHttpRequestPostMessageThreadTask );

    public:
        AppleHttpRequestPostMessageThreadTask();
        ~AppleHttpRequestPostMessageThreadTask() override;

    public:
        bool initialize( const HttpRequestPostProperties & _properties );

    protected:
        jobject _onHttp( JNIEnv * _jenv, jobject _jrequest ) override;

    protected:
        HttpRequestPostProperties m_properties;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequestPostMessageThreadTask, ThreadTaskInterface> AppleHttpRequestPostMessageThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}