#pragma once

#include "cURLThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLHeaderDataThreadTask
        : public cURLThreadTask
    {
    public:
        cURLHeaderDataThreadTask();
        ~cURLHeaderDataThreadTask() override;

    public:
        bool initialize( const String & _data );

    protected:
        void _onCURL( CURL * _curl ) override;
        void _onCURLCleanup( CURL * _curl ) override;

    protected:        
        String m_data;

        struct curl_slist * m_curl_header_list;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLHeaderDataThreadTask> cURLHeaderDataThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}