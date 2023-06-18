#pragma once

#include "cURLHttpRequestThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLHttpRequestDeleteMessageThreadTask
        : public cURLHttpRequestThreadTask
    {
        DECLARE_FACTORABLE( cURLHttpRequestDeleteMessageThreadTask );

    public:
        cURLHttpRequestDeleteMessageThreadTask();
        ~cURLHttpRequestDeleteMessageThreadTask() override;

    public:
        bool initialize();

    protected:
        void _onCURL( CURL * _curl ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLHttpRequestDeleteMessageThreadTask, ThreadTaskInterface> cURLHttpRequestDeleteMessageThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}