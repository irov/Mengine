#pragma once

#include "AndroidHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidHttpRequestPostMessage
        : public AndroidHttpRequest
    {
        DECLARE_FACTORABLE(AndroidHttpRequestPostMessage );

    public:
        AndroidHttpRequestPostMessage();
        ~AndroidHttpRequestPostMessage() override;

    public:
        void setPostProperties( const HttpRequestPostProperties & _properties );
        const HttpRequestPostProperties & getPostProperties() const;

    protected:
        jobject _onHttp( JNIEnv * _jenv, jobject _jrequest ) override;

    protected:
        HttpRequestPostProperties m_properties;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidHttpRequestPostMessage, HttpRequestInterface> AndroidHttpRequestPostMessagePtr;
    //////////////////////////////////////////////////////////////////////////
}