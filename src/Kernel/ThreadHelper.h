#pragma once

#include "Interface/DocumentInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/ThreadEnum.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void()> LambdaThreadUpdate;
        typedef Lambda<void()> LambdaThreadWorker;
        //////////////////////////////////////////////////////////////////////////
        bool createSimpleThreadWorker( const ConstString & _threadName, EThreadPriority _priority, uint32_t _sleep, const LambdaThreadUpdate & _update, const LambdaThreadWorker & _worker, const DocumentInterfacePtr & _doc );
        void destroySimpleThreadWorker( const ConstString & _threadName );
        //////////////////////////////////////////////////////////////////////////
    }
}