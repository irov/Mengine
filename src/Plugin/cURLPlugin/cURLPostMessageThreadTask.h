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
        bool initialize( const String & _url, const MapParams & _params );

    protected:
        void _onCURL( CURL * _curl ) override;

    protected:
        String m_url;
        MapParams m_params;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLPostMessageThreadTask> cURLPostMessageThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}