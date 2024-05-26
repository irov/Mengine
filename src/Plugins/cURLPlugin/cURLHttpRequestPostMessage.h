#pragma once

#include "cURLHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLHttpRequestPostMessage
        : public cURLHttpRequest
    {
        DECLARE_FACTORABLE( cURLHttpRequestPostMessage );

    public:
        cURLHttpRequestPostMessage();
        ~cURLHttpRequestPostMessage() override;

    public:
        void setPostProperties( const HttpRequestPostProperties & _params );
        const HttpRequestPostProperties & getPostProperties() const;

    protected:
        void _onCURL( CURL * _curl ) override;
        void _onCURLCleanup( CURL * _curl ) override;

    protected:
        HttpRequestPostProperties m_params;

        struct curl_httppost * m_curl_formpost;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLHttpRequestPostMessage, HttpRequestInterface> cURLHttpRequestPostMessagePtr;
    //////////////////////////////////////////////////////////////////////////
}