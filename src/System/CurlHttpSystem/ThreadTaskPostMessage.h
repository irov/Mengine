#pragma once

#include "ThreadTaskCurl.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadTaskPostMessage
        : public ThreadTaskCurl
    {
    public:
        ThreadTaskPostMessage();

    public:
        bool initialize( const String & _url, const MapParams & _params );

    protected:
        void _onCURL( CURL * _curl ) override;

    protected:
        String m_url;
        MapParams m_params;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadTaskPostMessage> ThreadTaskPostMessagePtr;
    //////////////////////////////////////////////////////////////////////////
}