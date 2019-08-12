#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"

namespace Mengine
{
    class MemoryServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "MemoryService" )

    public:
        virtual MemoryBufferInterfacePtr createMemoryBuffer( const Char * _doc ) = 0;
        virtual MemoryProxyInterfacePtr createMemoryProxy( const Char * _doc ) = 0;
        virtual MemoryBufferInterfacePtr createMemoryCacheBuffer( const Char * _doc ) = 0;

    public:
        virtual MemoryCacheInputInterfacePtr createMemoryCacheInput( const Char * _doc ) = 0;
        virtual MemoryProxyInputInterfacePtr createMemoryProxyInput( const Char * _doc ) = 0;
        virtual MemoryInputInterfacePtr createMemoryInput( const Char * _doc ) = 0;

    public:
        virtual void clearCacheBuffers() = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define MEMORY_SERVICE()\
	((Mengine::MemoryServiceInterface *)SERVICE_GET(Mengine::MemoryServiceInterface))
//////////////////////////////////////////////////////////////////////////