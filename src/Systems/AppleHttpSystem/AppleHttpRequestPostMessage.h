#pragma once

#include "AppleHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleHttpRequestPostMessage
        : public AppleHttpRequest
    {
        DECLARE_FACTORABLE( AppleHttpRequestPostMessage );

    public:
        AppleHttpRequestPostMessage();
        ~AppleHttpRequestPostMessage() override;

    public:
        bool initialize( const HttpRequestPostProperties & _properties );

    protected:
        jobject _onHttp( JNIEnv * _jenv, jobject _jrequest ) override;

    protected:
        HttpRequestPostProperties m_properties;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequestPostMessage, ThreadTaskInterface> AppleHttpRequestPostMessagePtr;
    //////////////////////////////////////////////////////////////////////////
}