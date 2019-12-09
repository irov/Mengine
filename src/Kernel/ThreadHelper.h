#pragma once

#include "Kernel/ConstString.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void()> LambdaThreadUpdate;
        typedef Lambda<void()> LambdaThreadWorker;
        //////////////////////////////////////////////////////////////////////////
        bool createSimpleThreadWorker( const ConstString & _threadName, uint32_t _sleep, const LambdaThreadUpdate & _update, const LambdaThreadWorker & _worker, const DocumentPtr & _doc );
        void destroySimpleThreadWorker( const ConstString & _threadName );
        //////////////////////////////////////////////////////////////////////////
    }
}