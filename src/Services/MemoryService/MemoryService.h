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
        CacheBufferID lockBuffer( size_t _size, void ** _memory, const Char * _doc, const Char * _file, uint32_t _line );
        void unlockBuffer( CacheBufferID _bufferId );

    public:
        void clearCacheBuffers() override;

    public:
        MemoryBufferInterfacePtr createMemoryBuffer() override;
        MemoryProxyInterfacePtr createMemoryProxy() override;
        MemoryBufferInterfacePtr createMemoryCacheBuffer() override;
        MemoryCacheInputInterfacePtr createMemoryCacheInput() override;
        MemoryProxyInputInterfacePtr createMemoryProxyInput() override;
        MemoryInputInterfacePtr createMemoryInput() override;

    protected:
        CacheBufferID lockBufferNoMutex_( size_t _size, void ** _memory, const Char * _doc, const Char * _file, uint32_t _line );

    protected:
        struct CacheBufferMemory
        {
            CacheBufferID id;
            void * memory;
            const Char * doc;
            size_t size;
            const Char * file;
            uint32_t line;
            bool lock;
        };

        typedef Vector<CacheBufferMemory> VectorCacheBufferMemory;
        VectorCacheBufferMemory m_buffers;

        CacheBufferID m_enumeratorId;

        ThreadMutexInterfacePtr m_memoryCacheMutex;

        FactoryWithMutexPtr m_factoryMemoryBuffer;
        FactoryWithMutexPtr m_factoryMemoryProxy;
        FactoryWithMutexPtr m_factoryMemoryCacheBuffer;
        FactoryWithMutexPtr m_factoryMemoryCacheInput;
        FactoryWithMutexPtr m_factoryMemoryProxyInput;
        FactoryWithMutexPtr m_factoryMemoryInput;
    };
}