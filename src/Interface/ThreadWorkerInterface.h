#pragma once

#include "Interface/Interface.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadWorkerInterface
        : public Interface
    {
    public:
        virtual void onThreadWorkerUpdate( UniqueId _id ) = 0;
        virtual bool onThreadWorkerWork( UniqueId _id ) = 0;
        virtual void onThreadWorkerDone( UniqueId _id ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadWorkerInterface> ThreadWorkerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}