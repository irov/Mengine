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
        UniqueId lockBuffer( size_t _size, void ** const _memory, const DocumentInterfacePtr & _doc );
        void unlockBuffer( UniqueId _bufferId );

    public:
        void clearCacheBuffers() override;

    public:
        MemoryBufferInterfacePtr createMemoryBuffer( const DocumentInterfacePtr & _doc ) override;
        MemoryProxyInterfacePtr createMemoryProxy( const DocumentInterfacePtr & _doc ) override;
        MemoryBufferInterfacePtr createMemoryCacheBuffer( const DocumentInterfacePtr & _doc ) override;
        MemoryCacheInputInterfacePtr createMemoryCacheInput( const DocumentInterfacePtr & _doc ) override;
        MemoryProxyInputInterfacePtr createMemoryProxyInput( const DocumentInterfacePtr & _doc ) override;
        MemoryInputInterfacePtr createMemoryInput( const DocumentInterfacePtr & _doc ) override;

    protected:
        UniqueId lockBufferNoMutex_( size_t _size, void ** const _memory, const DocumentInterfacePtr & _doc );

    protected:
        struct CacheBufferMemory
        {
            UniqueId id;
            void * memory;
            size_t size;
            bool lock;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
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