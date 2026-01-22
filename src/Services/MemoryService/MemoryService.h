#pragma once

#include "Interface/MemoryServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"
#include "Interface/FactoryInterface.h"

#include "Kernel/ServiceBase.h"

#include "MemoryBuffer.h"
#include "MemoryProxy.h"
#include "MemoryCacheBuffer.h"
#include "MemoryCacheInput.h"
#include "MemoryProxyInput.h"
#include "MemoryInput.h"

#include "Kernel/Vector.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    class MemoryService
        : public ServiceBase<MemoryServiceInterface>
    {
        DECLARE_FACTORABLE( MemoryService );

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

        ThreadMutexInterfacePtr m_memoryCacheMutex;

        FactoryInterfacePtr m_factoryMemoryBuffer;
        FactoryInterfacePtr m_factoryMemoryProxy;
        FactoryInterfacePtr m_factoryMemoryCacheBuffer;
        FactoryInterfacePtr m_factoryMemoryCacheInput;
        FactoryInterfacePtr m_factoryMemoryProxyInput;
        FactoryInterfacePtr m_factoryMemoryInput;
    };
}