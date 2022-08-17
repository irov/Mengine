#include "MemoryService.h"

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/EnumeratorHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ThreadMutexScope.h"

#include "Config/Algorithm.h"

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
        m_factoryMemoryCacheBuffer = Helper::makeFactoryPool<MemoryCacheBuffer, 16, FactoryWithMutex>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryMemoryCacheInput = Helper::makeFactoryPool<MemoryCacheInput, 16, FactoryWithMutex>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryMemoryProxyInput = Helper::makeFactoryPool<MemoryProxyInput, 16, FactoryWithMutex>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryMemoryInput = Helper::makeFactoryPool<MemoryInput, 16, FactoryWithMutex>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryMemoryBuffer = Helper::makeFactoryPool<MemoryBuffer, 16, FactoryWithMutex>( MENGINE_DOCUMENT_FACTORABLE );
        m_factoryMemoryProxy = Helper::makeFactoryPool<MemoryProxy, 16, FactoryWithMutex>( MENGINE_DOCUMENT_FACTORABLE );

        ThreadMutexInterfacePtr memoryCacheMutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memoryCacheMutex );

        m_memoryCacheMutex = memoryCacheMutex;

        ThreadMutexInterfacePtr memoryFactoryMutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memoryFactoryMutex );

        m_factoryMemoryBuffer->setMutex( memoryFactoryMutex );
        m_factoryMemoryProxy->setMutex( memoryFactoryMutex );
        m_factoryMemoryCacheBuffer->setMutex( memoryFactoryMutex );
        m_factoryMemoryCacheInput->setMutex( memoryFactoryMutex );
        m_factoryMemoryProxyInput->setMutex( memoryFactoryMutex );
        m_factoryMemoryInput->setMutex( memoryFactoryMutex );

        m_memoryFactoryMutex = memoryFactoryMutex;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryService::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMemoryBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMemoryProxy );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMemoryCacheBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMemoryCacheInput );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMemoryProxyInput );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMemoryInput );

        m_memoryCacheMutex = nullptr;

        m_factoryMemoryBuffer->setMutex( nullptr );
        m_factoryMemoryProxy->setMutex( nullptr );
        m_factoryMemoryCacheBuffer->setMutex( nullptr );
        m_factoryMemoryCacheInput->setMutex( nullptr );
        m_factoryMemoryProxyInput->setMutex( nullptr );
        m_factoryMemoryInput->setMutex( nullptr );
        
        m_memoryFactoryMutex = nullptr;

        m_factoryMemoryBuffer = nullptr;
        m_factoryMemoryProxy = nullptr;
        m_factoryMemoryCacheBuffer = nullptr;
        m_factoryMemoryCacheInput = nullptr;
        m_factoryMemoryProxyInput = nullptr;
        m_factoryMemoryInput = nullptr;

        MENGINE_ASSERTION_ALLOCATOR( "memory" );
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryService::_stopService()
    {
        this->clearCacheBuffers();
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId MemoryService::lockBuffer( size_t _size, void ** const _memory, const DocumentPtr & _doc )
    {        
        MENGINE_THREAD_MUTEX_SCOPE( m_memoryCacheMutex );

        UniqueId buffer_id =
            this->lockBufferNoMutex_( _size, _memory, _doc );

        return buffer_id;
    }
    //////////////////////////////////////////////////////////////////////////
    UniqueId MemoryService::lockBufferNoMutex_( size_t _size, void ** const _memory, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        size_t minSize = 0U;
        size_t maxSize = MENGINE_UNKNOWN_SIZE;

        const VectorCacheBufferMemory::size_type invalidIndex = MENGINE_UNKNOWN_SIZE;

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

#ifdef MENGINE_DOCUMENT_ENABLE
            buffer.doc = _doc;
#endif

            * _memory = buffer.memory;

            return buffer.id;
        }
        else if( minIndex != invalidIndex )
        {
            CacheBufferMemory & buffer = m_buffers[minIndex];

            Helper::deallocateMemory( buffer.memory, "memory" );

            void * memory = Helper::allocateMemory( _size, "memory" );

            MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid realloc %p memory %zu to %zu (doc: %s)"
                , buffer.memory
                , buffer.size
                , _size
                , MENGINE_DOCUMENT_STR( _doc )
            );

            buffer.memory = memory;
            buffer.size = _size;
            buffer.lock = true;

#ifdef MENGINE_DOCUMENT_ENABLE
            buffer.doc = _doc;
#endif

            * _memory = buffer.memory;

            return buffer.id;
        }

        void * memory = Helper::allocateMemory( _size, "memory" );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid malloc memory %zu (doc: %s)"
            , _size
            , MENGINE_DOCUMENT_STR( _doc )
        );

        UniqueId new_id = Helper::generateUniqueIdentity();

        CacheBufferMemory buffer;
        buffer.id = new_id;
        buffer.memory = memory;
        buffer.size = _size;
        buffer.lock = true;

#ifdef MENGINE_DOCUMENT_ENABLE
        buffer.doc = _doc;
#endif

        m_buffers.emplace_back( buffer );

        *_memory = buffer.memory;

        return new_id;
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryService::unlockBuffer( UniqueId _bufferId )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_memoryCacheMutex );

        for( CacheBufferMemory & buffer : m_buffers )
        {
            if( buffer.id != _bufferId )
            {
                continue;
            }

            buffer.lock = false;

            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MemoryService::clearCacheBuffers()
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_memoryCacheMutex );

        m_buffers.erase( Algorithm::remove_if( m_buffers.begin(), m_buffers.end(), []( const CacheBufferMemory & _buffer )
        {
            if( _buffer.lock == true )
            {
                return false;
            }

            Helper::deallocateMemory( _buffer.memory, "memory" );

            return true;
        } ), m_buffers.end() );
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryBufferInterfacePtr MemoryService::createMemoryCacheBuffer( const DocumentPtr & _doc )
    {
        MemoryCacheBufferPtr memoryBuffer = m_factoryMemoryCacheBuffer->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memoryBuffer );

        memoryBuffer->setMemoryManager( this );

        return memoryBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryCacheInputInterfacePtr MemoryService::createMemoryCacheInput( const DocumentPtr & _doc )
    {
        MemoryCacheInputPtr memoryCache = m_factoryMemoryCacheInput->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memoryCache );

        memoryCache->setMemoryManager( this );

        return memoryCache;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryProxyInputInterfacePtr MemoryService::createMemoryProxyInput( const DocumentPtr & _doc )
    {
        MemoryProxyInputPtr memoryProxy = m_factoryMemoryProxyInput->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memoryProxy );

        return memoryProxy;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInputInterfacePtr MemoryService::createMemoryInput( const DocumentPtr & _doc )
    {
        MemoryInputPtr memory = m_factoryMemoryInput->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryBufferInterfacePtr MemoryService::createMemoryBuffer( const DocumentPtr & _doc )
    {
        MemoryPtr memory = m_factoryMemoryBuffer->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryProxyInterfacePtr MemoryService::createMemoryProxy( const DocumentPtr & _doc )
    {
        MemoryProxyPtr memory = m_factoryMemoryProxy->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
}
