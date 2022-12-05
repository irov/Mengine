#pragma once

#include "cURLThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLGetMessageThreadTask
        : public cURLThreadTask
    {
        DECLARE_FACTORABLE( cURLGetMessageThreadTask );

    public:
        cURLGetMessageThreadTask();
        ~cURLGetMessageThreadTask() override;

    public:
        bool initialize();

    protected:
        void _onCURL( CURL * _curl ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLGetMessageThreadTask, ThreadTaskInterface> cURLGetMessageThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}