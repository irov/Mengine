#pragma once

#include "AppleHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleHttpRequestDeleteMessage
        : public AppleHttpRequest
    {
        DECLARE_FACTORABLE( AppleHttpRequestDeleteMessage );

    public:
        AppleHttpRequestDeleteMessage();
        ~AppleHttpRequestDeleteMessage() override;

    public:
        bool initialize();

    protected:
        jobject _onHttp( JNIEnv * _jenv, jobject _basses ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequestDeleteMessage, HttpRequestInterface> AppleHttpRequestDeleteMessageThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}