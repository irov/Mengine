#pragma once

#include "Interface/ThreadWorkerInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/ThreadEnum.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool createSimpleThreadWorker( const ConstString & _threadName, EThreadPriority _priority, uint32_t _sleep, const ThreadWorkerInterfacePtr & _worker, const DocumentInterfacePtr & _doc );
        void destroySimpleThreadWorker( const ConstString & _threadName );
        //////////////////////////////////////////////////////////////////////////
    }
}