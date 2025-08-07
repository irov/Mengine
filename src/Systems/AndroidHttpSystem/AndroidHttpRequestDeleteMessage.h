#pragma once

#include "AndroidHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidHttpRequestDeleteMessage
        : public AndroidHttpRequest
    {
        DECLARE_FACTORABLE( AndroidHttpRequestDeleteMessage );

    public:
        AndroidHttpRequestDeleteMessage();
        ~AndroidHttpRequestDeleteMessage() override;

    protected:
        jobject _onHttp( MengineJNIEnvThread * _jenv, jobject _basses ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidHttpRequestDeleteMessage, HttpRequestInterface> AndroidHttpRequestDeleteMessagePtr;
    //////////////////////////////////////////////////////////////////////////
}