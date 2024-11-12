#pragma once

#include "AndroidHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidHttpRequestGetMessage
        : public AndroidHttpRequest
    {
        DECLARE_FACTORABLE( AndroidHttpRequestGetMessage );

    public:
        AndroidHttpRequestGetMessage();
        ~AndroidHttpRequestGetMessage() override;

    protected:
        jobject _onHttp( JNIEnv * _jenv, jobject _basses ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidHttpRequestGetMessage, HttpRequestInterface> AndroidHttpRequestGetMessagePtr;
    //////////////////////////////////////////////////////////////////////////
}