#pragma once

#include "Interface/MemoryServiceInterface.h"

#include "Kernel/ServiceBase.h"

#include "MemoryBuffer.h"
#include "MemoryProxy.h"
#include "MemoryCacheBuffer.h"
#include "MemoryCacheInput.h"
#include "MemoryProxyInput.h"
#include "MemoryInput.h"

#include "Kernel/FactoryWithMutex.h"

#include "Config/Vector.h"

namespace Mengine
{
    typedef uint32_t CacheBufferID;

    static const CacheBufferID INVALID_CACHE_BUFFER_ID = 0;

    class MemoryService
        : public ServiceBase<MemoryServiceInterface>
    {
    public:
        MemoryService();
        ~MemoryService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        CacheBufferID lockBuffer( size_t _size, void ** _memory, const Char * _doc );
        void unlockBuffer( CacheBufferID _bufferId );

    public:
        void clearCacheBuffers() override;

    public:
        MemoryBufferInterfacePtr createMemoryBuffer( const Char * _doc ) override;
        MemoryProxyInterfacePtr createMemoryProxy( const Char * _doc ) override;
        MemoryBufferInterfacePtr createMemoryCacheBuffer( const Char * _doc ) override;
        MemoryCacheInputInterfacePtr createMemoryCacheInput( const Char * _doc ) override;
        MemoryProxyInputInterfacePtr createMemoryProxyInput( const Char * _doc ) override;
        MemoryInputInterfacePtr createMemoryInput( const Char * _doc ) override;

    protected:
        CacheBufferID lockBufferNoMutex_( size_t _size, void ** _memory, const Char * _doc );

    protected:
        struct CacheBufferMemory
        {
            CacheBufferID id;
            void * memory;
            size_t size;
            bool lock;

#ifdef MENGINE_DEBUG
            const Char * doc;
#endif
        };

        typedef Vector<CacheBufferMemory> VectorCacheBufferMemory;
        VectorCacheBufferMemory m_buffers;

        ThreadMutexInterfacePtr m_memoryCacheMutex;

        FactoryWithMutexPtr m_factoryMemoryBuffer;
        FactoryWithMutexPtr m_factoryMemoryProxy;
        FactoryWithMutexPtr m_factoryMemoryCacheBuffer;
        FactoryWithMutexPtr m_factoryMemoryCacheInput;
        FactoryWithMutexPtr m_factoryMemoryProxyInput;
        FactoryWithMutexPtr m_factoryMemoryInput;
    };
}