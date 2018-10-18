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
        bool initialize( const String & _url, const VectorString & _headers, const String & _data );

    protected:
        void _onCURL( CURL * _curl ) override;

    protected:
        String m_url;
        VectorString m_headers;
        String m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLHeaderDataThreadTask> cURLHeaderDataThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}