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
#include "Kernel/Vector.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    class MemoryService
        : public ServiceBase<MemoryServiceInterface>
    {
    public:
        MemoryService();
        ~MemoryService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

    public:
        UniqueId lockBuffer( size_t _size, void ** const _memory, const DocumentPtr & _doc );
        void unlockBuffer( UniqueId _bufferId );

    public:
        void clearCacheBuffers() override;

    public:
        MemoryBufferInterfacePtr createMemoryBuffer( const DocumentPtr & _doc ) override;
        MemoryProxyInterfacePtr createMemoryProxy( const DocumentPtr & _doc ) override;
        MemoryBufferInterfacePtr createMemoryCacheBuffer( const DocumentPtr & _doc ) override;
        MemoryCacheInputInterfacePtr createMemoryCacheInput( const DocumentPtr & _doc ) override;
        MemoryProxyInputInterfacePtr createMemoryProxyInput( const DocumentPtr & _doc ) override;
        MemoryInputInterfacePtr createMemoryInput( const DocumentPtr & _doc ) override;

    protected:
        UniqueId lockBufferNoMutex_( size_t _size, void ** const _memory, const DocumentPtr & _doc );

    protected:
        struct CacheBufferMemory
        {
            UniqueId id;
            void * memory;
            size_t size;
            bool lock;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentPtr doc;
#endif
        };

        typedef Vector<CacheBufferMemory> VectorCacheBufferMemory;
        VectorCacheBufferMemory m_buffers;

        ThreadMutexInterfacePtr m_memoryFactoryMutex;
        ThreadMutexInterfacePtr m_memoryCacheMutex;

        FactoryWithMutexPtr m_factoryMemoryBuffer;
        FactoryWithMutexPtr m_factoryMemoryProxy;
        FactoryWithMutexPtr m_factoryMemoryCacheBuffer;
        FactoryWithMutexPtr m_factoryMemoryCacheInput;
        FactoryWithMutexPtr m_factoryMemoryProxyInput;
        FactoryWithMutexPtr m_factoryMemoryInput;
    };
}