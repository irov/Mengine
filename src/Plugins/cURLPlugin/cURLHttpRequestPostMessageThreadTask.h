#pragma once

#include "cURLHttpRequestThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLHttpRequestPostMessageThreadTask
        : public cURLHttpRequestThreadTask
    {
        DECLARE_FACTORABLE( cURLHttpRequestPostMessageThreadTask );

    public:
        cURLHttpRequestPostMessageThreadTask();
        ~cURLHttpRequestPostMessageThreadTask() override;

    public:
        bool initialize( const HttpRequestPostProperties & _params );

    protected:
        void _onCURL( CURL * _curl ) override;
        void _onCURLCleanup( CURL * _curl ) override;

    protected:
        HttpRequestPostProperties m_params;

        struct curl_httppost * m_curl_formpost;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLHttpRequestPostMessageThreadTask, ThreadTaskInterface> cURLHttpRequestPostMessageThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}