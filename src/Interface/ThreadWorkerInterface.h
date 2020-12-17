#pragma once

#include "Interface/Interface.h"

#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ThreadWorkerInterface
        : public Interface
    {
    public:
        virtual void onThreadWorkerUpdate( uint32_t _id ) = 0;
        virtual bool onThreadWorkerWork( uint32_t _id ) = 0;
        virtual void onThreadWorkerDone( uint32_t _id ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadWorkerInterface> ThreadWorkerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}