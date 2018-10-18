#pragma once

#include "cURLThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLGetMessageThreadTask
        : public cURLThreadTask
    {
    public:
        cURLGetMessageThreadTask();
        ~cURLGetMessageThreadTask() override;

    public:
        bool initialize( const String & _url );

    protected:
        void _onCURL( CURL * _curl ) override;

    protected:
        String m_url;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLGetMessageThreadTask> cURLGetMessageThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}