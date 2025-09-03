#pragma once

#include "AndroidHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidHttpRequestPing
        : public AndroidHttpRequest
    {
        DECLARE_FACTORABLE( AndroidHttpRequestPing );

    public:
        AndroidHttpRequestPing();
        ~AndroidHttpRequestPing() override;

    protected:
        jobject _onHttp( JNIEnv * _jenv, jobject _basses ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidHttpRequestPing, HttpRequestInterface> AndroidHttpRequestPingPtr;
    //////////////////////////////////////////////////////////////////////////
}