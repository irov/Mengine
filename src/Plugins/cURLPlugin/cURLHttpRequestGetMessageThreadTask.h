#pragma once

#include "cURLHttpRequestThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLHttpRequestGetMessageThreadTask
        : public cURLHttpRequestThreadTask
    {
        DECLARE_FACTORABLE( cURLHttpRequestGetMessageThreadTask );

    public:
        cURLHttpRequestGetMessageThreadTask();
        ~cURLHttpRequestGetMessageThreadTask() override;

    public:
        bool initialize();

    protected:
        void _onCURL( CURL * _curl ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLHttpRequestGetMessageThreadTask, ThreadTaskInterface> cURLHttpRequestGetMessageThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}