#include "MemoryService.h"

#include "Interface/ThreadServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include <algorithm>

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
            ->createMutex( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( m_memoryCacheMutex, false );

        m_factoryMemoryCacheBuffer = new FactoryPool<MemoryCacheBuffer, 16, FactoryWithMutex>();
        m_factoryMemoryCacheInput = new FactoryPool<MemoryCacheInput, 16, FactoryWithMutex>();
        m_factoryMemoryProxyInput = new FactoryPool<MemoryProxyInput, 16, FactoryWithMutex>();
        m_factoryMemoryInput = new FactoryPool<MemoryInput, 16, FactoryWithMutex>();
        m_factoryMemoryBuffer = new FactoryPool<MemoryBuffer, 16, FactoryWithMutex>();
        m_factoryMemoryProxy = new FactoryPool<MemoryProxy, 16, FactoryWithMutex>();

        ThreadMutexInterfacePtr memoryFactoryMutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( memoryFactoryMutex, false );

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
    CacheBufferID MemoryService::lockBuffer( size_t _size, void ** _memory, const Char * _doc )
    {
        m_memoryCacheMutex->lock();

        CacheBufferID buffer_id =
            this->lockBufferNoMutex_( _size, _memory, _doc );

        m_memoryCacheMutex->unlock();

        return buffer_id;
    }
    //////////////////////////////////////////////////////////////////////////
    CacheBufferID MemoryService::lockBufferNoMutex_( size_t _size, void ** _memory, const Char * _doc )
    {
        size_t minSize = 0U;
        size_t maxSize = ~0U;

        const VectorCacheBufferMemory::size_type invalidIndex = ~0U;

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

            buffer.lock = true;

#ifdef MENGINE_DEBUG
            buffer.doc = _doc;
#endif

            *_memory = buffer.memory;

            return buffer.id;
        }
        else if( minIndex != invalidIndex )
        {
            CacheBufferMemory & buffer = m_buffers[minIndex];

            Helper::freeMemory( buffer.memory, MENGINE_DEBUG_ATTRIBUTE( buffer.doc, "" ) );

            void * memory = Helper::allocateMemory( _size, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory, INVALID_CACHE_BUFFER_ID, "invalid realloc %p memory %d to %d"
                , buffer.memory
                , buffer.size
                , _size
            );

            buffer.memory = memory;            
            buffer.size = _size;
            buffer.lock = true;

#ifdef MENGINE_DEBUG
            buffer.doc = _doc;
#endif

            *_memory = buffer.memory;

            return buffer.id;
        }

        uint8_t * memory = (uint8_t *)Helper::allocateMemory( _size, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, INVALID_CACHE_BUFFER_ID, "invalid malloc memory %d"
            , _size
        );

        CacheBufferID new_id = GENERATE_UNIQUE_IDENTITY();

        CacheBufferMemory buffer;
        buffer.id = new_id;
        buffer.memory = memory;
        buffer.size = _size;
        buffer.lock = true;

#ifdef MENGINE_DEBUG
        buffer.doc = _doc;
#endif

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

        m_buffers.erase( std::remove_if( m_buffers.begin(), m_buffers.end(), []( const CacheBufferMemory & _buffer )
        {
            if( _buffer.lock == true )
            {
                return false;
            }

            Helper::freeMemory( _buffer.memory, MENGINE_DEBUG_ATTRIBUTE( _buffer.doc, "" ) );

            return true;
        } ), m_buffers.end() );

        m_memoryCacheMutex->unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryBufferInterfacePtr MemoryService::createMemoryCacheBuffer( const Char * _doc )
    {
        MemoryCacheBufferPtr memoryBuffer = m_factoryMemoryCacheBuffer->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memoryBuffer, nullptr );

        memoryBuffer->setMemoryManager( this );

        return memoryBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryCacheInputInterfacePtr MemoryService::createMemoryCacheInput( const Char * _doc )
    {
        MemoryCacheInputPtr memoryCache = m_factoryMemoryCacheInput->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memoryCache, nullptr );

        memoryCache->setMemoryManager( this );

        return memoryCache;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryProxyInputInterfacePtr MemoryService::createMemoryProxyInput( const Char * _doc )
    {
        MemoryProxyInputPtr memoryProxy = m_factoryMemoryProxyInput->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memoryProxy, nullptr );

        return memoryProxy;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInputInterfacePtr MemoryService::createMemoryInput( const Char * _doc )
    {
        MemoryInputPtr memory = m_factoryMemoryInput->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryBufferInterfacePtr MemoryService::createMemoryBuffer( const Char * _doc )
    {
        MemoryPtr memory = m_factoryMemoryBuffer->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryProxyInterfacePtr MemoryService::createMemoryProxy( const Char * _doc )
    {
        MemoryProxyPtr memory = m_factoryMemoryProxy->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

        return memory;
    }
}
