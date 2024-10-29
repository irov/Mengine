#pragma once

#include "Interface/ThreadMutexInterface.h"
#include "Interface/ThreadSharedMutexInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    namespace Helper
    {
        ThreadMutexInterfacePtr createThreadMutex( const DocumentInterfacePtr & _doc );
        ThreadSharedMutexInterfacePtr createThreadSharedMutex( const DocumentInterfacePtr & _doc );
    }
}