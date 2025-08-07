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
        jobject _onHttp( MengineJNIEnvThread * _jenv, jobject _basses ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidHttpRequestGetMessage, HttpRequestInterface> AndroidHttpRequestGetMessagePtr;
    //////////////////////////////////////////////////////////////////////////
}