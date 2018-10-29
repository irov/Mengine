#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MemoryServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "MemoryService" )

    public:
        virtual MemoryBufferInterfacePtr createMemoryBuffer() = 0;
        virtual MemoryProxyInterfacePtr createMemoryProxy() = 0;
        virtual MemoryBufferInterfacePtr createMemoryCacheBuffer() = 0;

    public:
        virtual MemoryCacheInputInterfacePtr createMemoryCacheInput() = 0;
        virtual MemoryProxyInputInterfacePtr createMemoryProxyInput() = 0;
        virtual MemoryInputInterfacePtr createMemoryInput() = 0;

    public:
        virtual void clearCacheBuffers() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define MEMORY_SERVICE()\
	((Mengine::MemoryServiceInterface *)SERVICE_GET(Mengine::MemoryServiceInterface))
//////////////////////////////////////////////////////////////////////////

