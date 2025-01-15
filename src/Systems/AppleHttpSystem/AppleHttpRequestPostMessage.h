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
        void setPostProperties( const HttpRequestPostProperties & _properties );
        const HttpRequestPostProperties & getPostProperties() const;

    protected:
        AppleHttpResponseParam * _onHttp( AppleHttpRequestParam * request ) override;

    protected:
        HttpRequestPostProperties m_properties;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequestPostMessage, ThreadTaskInterface> AppleHttpRequestPostMessagePtr;
    //////////////////////////////////////////////////////////////////////////
}
