#pragma once

#include "AndroidHttpRequestThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidHttpRequestPostMessageThreadTask
        : public AndroidHttpRequestThreadTask
    {
        DECLARE_FACTORABLE( AndroidHttpRequestPostMessageThreadTask );

    public:
        AndroidHttpRequestPostMessageThreadTask();
        ~AndroidHttpRequestPostMessageThreadTask() override;

    public:
        bool initialize( const HttpRequestPostProperties & _properties );

    protected:
        jobject _onHttp( JNIEnv * _jenv, jobject _jrequest ) override;

    protected:
        HttpRequestPostProperties m_properties;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidHttpRequestPostMessageThreadTask, ThreadTaskInterface> AndroidHttpRequestPostMessageThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}