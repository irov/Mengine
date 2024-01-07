#pragma once

#include "Interface/ThreadMutexInterface.h"
#include "Interface/DocumentInterface.h"

namespace Mengine
{
    namespace Helper
    {
        ThreadMutexInterfacePtr createThreadMutex( const DocumentInterfacePtr & _doc );
    }
}