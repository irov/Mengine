#pragma once

#include "cURLThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLPostMessageThreadTask
        : public cURLThreadTask
    {
        DECLARE_FACTORABLE( cURLPostMessageThreadTask );

    public:
        cURLPostMessageThreadTask();
        ~cURLPostMessageThreadTask() override;

    public:
        bool initialize( const cURLPostParams & _params );

    protected:
        void _onCURL( CURL * _curl ) override;
        void _onCURLCleanup( CURL * _curl ) override;

    protected:
        cURLPostParams m_params;

        struct curl_httppost * m_curl_formpost;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLPostMessageThreadTask, ThreadTaskInterface> cURLPostMessageThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}