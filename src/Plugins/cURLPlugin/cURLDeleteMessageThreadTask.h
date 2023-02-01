#pragma once

#include "cURLThreadTask.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLDeleteMessageThreadTask
        : public cURLThreadTask
    {
        DECLARE_FACTORABLE( cURLDeleteMessageThreadTask );

    public:
        cURLDeleteMessageThreadTask();
        ~cURLDeleteMessageThreadTask() override;

    public:
        bool initialize();

    protected:
        void _onCURL( CURL * _curl ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLDeleteMessageThreadTask, ThreadTaskInterface> cURLDeleteMessageThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}