#pragma once

#include "cURLThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLPostMessageThreadTask
        : public cURLThreadTask
    {
    public:
        cURLPostMessageThreadTask();
        ~cURLPostMessageThreadTask() override;

    public:
        bool initialize( const String & _url, const cURLPostParams & _params );

    protected:
        void _onCURL( CURL * _curl ) override;
        void _onCURLCleanup( CURL * _curl ) override;

    protected:
        String m_url;
        cURLPostParams m_params;

        struct curl_httppost * m_curl_formpost;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLPostMessageThreadTask> cURLPostMessageThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}