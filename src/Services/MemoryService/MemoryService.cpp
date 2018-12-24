#include "MemoryService.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( MemoryService, Mengine::MemoryService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MemoryService::MemoryService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryService::~MemoryService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MemoryService::_initializeService()
    {
        m_memoryCacheMutex = THREAD_SERVICE()
            ->createMutex( __FILE__, __LINE__ );

        m_factoryMemoryCacheBuffer = new FactoryPool<MemoryCacheBuffer, 16, FactoryWithMutex>();
        m_factoryMemoryCacheInput = new FactoryPool<MemoryCacheInput, 16, FactoryWithMutex>();
        m_factoryMemoryProxyInput = new FactoryPool<MemoryProxyInput, 16, FactoryWithMutex>();
        m_factoryMemoryInput = new FactoryPool<MemoryInput, 16, FactoryWithMutex>();
        m_factoryMemoryBuffer = new FactoryPool<MemoryBuffer, 16, FactoryWithMutex>();
        m_factoryMemoryProxy = new FactoryPool<MemoryProxy, 16, FactoryWithMutex>();

        ThreadMutexInterfacePtr memoryFactoryMutex = THREAD_SERVICE()
            ->createMutex( __FILE__, __LINE__ );

        m_factoryMemoryBuffer->setMutex( memoryFactoryMutex );
        m_factoryMemoryProxy->setMutex( memoryFactoryMutex );
        m_factoryMemoryCacheBuffer->setMutex( memoryFactoryMutex );
        m_factoryMemoryCacheInput->setMutex( memoryFactoryMutex );
        m_factoryMemoryProxyInput->setMutex( memoryFactoryMutex );
        m_factoryMemoryInput->setMutex( memoryFactoryMutex );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryService::_finalizeService()
    {
        this->clearCacheBuffers();

        m_memoryCacheMutex = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMemoryBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMemoryProxy );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMemoryCacheBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMemoryCacheInput );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMemoryProxyInput );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMemoryInput );

        m_factoryMemoryBuffer = nullptr;
        m_factoryMemoryProxy = nullptr;
        m_factoryMemoryCacheBuffer = nullptr;
        m_factoryMemoryCacheInput = nullptr;
        m_factoryMemoryProxyInput = nullptr;
        m_factoryMemoryInput = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    CacheBufferID MemoryService::lockBuffer( size_t _size, void ** _memory, const Char * _doc, const Char * _file, uint32_t _line )
    {
        m_memoryCacheMutex->lock();

        CacheBufferID buffer_id =
            this->lockBufferNoMutex_( _size, _memory, _doc, _file, _line );

        m_memoryCacheMutex->unlock();

        return buffer_id;
    }
    //////////////////////////////////////////////////////////////////////////
    CacheBufferID MemoryService::lockBufferNoMutex_( size_t _size, void ** _memory, const Char * _doc, const Char * _file, uint32_t _line )
    {
        size_t minSize = (size_t)(0);
        size_t maxSize = (size_t)(-1);

        const VectorCacheBufferMemory::size_type invalidIndex = (VectorCacheBufferMemory::size_type) - 1;

        VectorCacheBufferMemory::size_type minIndex = invalidIndex;
        VectorCacheBufferMemory::size_type maxIndex = invalidIndex;

        for( VectorCacheBufferMemory::size_type
            it = 0,
            it_end = m_buffers.size();
            it != it_end;
            ++it )
        {
            CacheBufferMemory & buffer = m_buffers[it];

            if( buffer.lock == true )
            {
                continue;
            }

            if( buffer.size > minSize && buffer.size <= _size )
            {
                minSize = buffer.size;
                minIndex = it;
            }

            if( buffer.size < maxSize && buffer.size >= _size )
            {
                maxSize = buffer.size;
                maxIndex = it;
            }
        }

        if( maxIndex != invalidIndex )
        {
            CacheBufferMemory & buffer = m_buffers[maxIndex];

            buffer.file = _file;
            buffer.line = _line;
            buffer.lock = true;

            *_memory = buffer.memory;

            return buffer.id;
        }
        else if( minIndex != invalidIndex )
        {
            CacheBufferMemory & buffer = m_buffers[minIndex];

            Helper::freeMemory( buffer.memory, buffer.doc );
            void * memory = Helper::allocateMemory( _size, _doc );

            if( memory == nullptr )
            {
                LOGGER_ERROR( "invalid realloc %p memory %d to %d"
                    , buffer.memory
                    , buffer.size
                    , _size
                );

                return INVALID_CACHE_BUFFER_ID;
            }

            buffer.memory = memory;
            buffer.doc = _doc;
            buffer.size = _size;
            buffer.file = _file;
            buffer.line = _line;
            buffer.lock = true;

            *_memory = buffer.memory;

            return buffer.id;
        }

        uint8_t * memory = (uint8_t *)Helper::allocateMemory( _size, _doc );

        if( memory == nullptr )
        {
            LOGGER_ERROR( "invalid malloc memory %d"
                , _size
            );

            return INVALID_CACHE_BUFFER_ID;
        }

        CacheBufferID new_id = GENERATE_UNIQUE_IDENTITY();

        CacheBufferMemory buffer;
        buffer.id = new_id;
        buffer.memory = memory;
        buffer.doc = _doc;
        buffer.size = _size;
        buffer.file = _file;
        buffer.line = _line;
        buffer.lock = true;

        m_buffers.emplace_back( buffer );

        *_memory = buffer.memory;

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryService::unlockBuffer( CacheBufferID _bufferId )
    {
        m_memoryCacheMutex->lock();

        for( CacheBufferMemory & buffer : m_buffers )
        {
            if( buffer.id != _bufferId )
            {
                continue;
            }

            buffer.lock = false;

            break;
        }

        m_memoryCacheMutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryService::clearCacheBuffers()
    {
        m_memoryCacheMutex->lock();

        for( const CacheBufferMemory & buffer : m_buffers )
        {
            if( buffer.lock == true )
            {
                LOGGER_ERROR( "CacheManager::finalize dont unlock buffer %d size %d"
                    , buffer.id
                    , buffer.size
                );
            }

            Helper::freeMemory( buffer.memory, buffer.doc );
        }

        m_buffers.clear();

        m_memoryCacheMutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryBufferInterfacePtr MemoryService::createMemoryCacheBuffer()
    {
        MemoryCacheBufferPtr memoryBuffer = m_factoryMemoryCacheBuffer->createObject();

        memoryBuffer->setMemoryManager( this );

        return memoryBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryCacheInputInterfacePtr MemoryService::createMemoryCacheInput()
    {
        MemoryCacheInputPtr memoryCache = m_factoryMemoryCacheInput->createObject();

        memoryCache->setMemoryManager( this );

        return memoryCache;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryProxyInputInterfacePtr MemoryService::createMemoryProxyInput()
    {
        MemoryProxyInputPtr memoryProxy = m_factoryMemoryProxyInput->createObject();

        return memoryProxy;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInputInterfacePtr MemoryService::createMemoryInput()
    {
        MemoryInputPtr memory = m_factoryMemoryInput->createObject();

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryBufferInterfacePtr MemoryService::createMemoryBuffer()
    {
        MemoryPtr memory = m_factoryMemoryBuffer->createObject();

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryProxyInterfacePtr MemoryService::createMemoryProxy()
    {
        MemoryProxyPtr memory = m_factoryMemoryProxy->createObject();

        return memory;
    }
}
