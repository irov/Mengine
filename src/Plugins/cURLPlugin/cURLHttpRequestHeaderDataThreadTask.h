#pragma once

#include "cURLHttpRequestThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLHttpRequestHeaderDataThreadTask
        : public cURLHttpRequestThreadTask
    {
        DECLARE_FACTORABLE( cURLHttpRequestHeaderDataThreadTask );

    public:
        cURLHttpRequestHeaderDataThreadTask();
        ~cURLHttpRequestHeaderDataThreadTask() override;

    public:
        bool initialize( const String & _data );

    protected:
        void _onCURL( CURL * _curl ) override;
        void _onCURLCleanup( CURL * _curl ) override;

    protected:
        String m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLHttpRequestHeaderDataThreadTask, ThreadTaskInterface> cURLHttpRequestHeaderDataThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}