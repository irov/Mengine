#pragma once

#include "AppleHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleHttpRequestGetMessage
        : public AppleHttpRequest
    {
        DECLARE_FACTORABLE( AppleHttpRequestGetMessage );

    public:
        AppleHttpRequestGetMessage();
        ~AppleHttpRequestGetMessage() override;

    public:
        bool initialize();

    protected:
        jobject _onHttp( JNIEnv * _jenv, jobject _basses ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequestGetMessage, HttpRequestInterface> AppleHttpRequestGetMessagePtr;
    //////////////////////////////////////////////////////////////////////////
}