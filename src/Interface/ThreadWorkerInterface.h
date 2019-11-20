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
        virtual void onUpdate( uint32_t _id ) = 0;
        virtual bool onWork( uint32_t _id ) = 0;
        virtual void onDone( uint32_t _id ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ThreadWorkerInterface> ThreadWorkerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}