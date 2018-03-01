#	include "MemoryManager.h"

#	include "Factory/FactoryPool.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( MemoryService, Menge::MemoryManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MemoryManager::MemoryManager()
		: m_enumeratorId(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryManager::~MemoryManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryManager::_initialize()
	{
        m_memoryCacheMutex = THREAD_SERVICE()
            ->createMutex( __FILE__, __LINE__ );

		m_factoryPoolMemoryCacheBuffer = new FactoryPool<MemoryCacheBuffer, 16, FactoryWithMutex>();
		m_factoryPoolMemoryCacheInput = new FactoryPool<MemoryCacheInput, 16, FactoryWithMutex>();
		m_factoryPoolMemoryProxyInput = new FactoryPool<MemoryProxyInput, 16, FactoryWithMutex>();
		m_factoryPoolMemoryInput = new FactoryPool<MemoryInput, 16, FactoryWithMutex>();
		m_factoryPoolMemory = new FactoryPool<Memory, 16, FactoryWithMutex>();
        
        ThreadMutexInterfacePtr memoryFactoryMutex = THREAD_SERVICE()
			->createMutex( __FILE__, __LINE__ );

		m_factoryPoolMemoryCacheBuffer->setMutex( memoryFactoryMutex );
		m_factoryPoolMemoryCacheInput->setMutex( memoryFactoryMutex );
		m_factoryPoolMemoryProxyInput->setMutex( memoryFactoryMutex );
		m_factoryPoolMemoryInput->setMutex( memoryFactoryMutex );
		m_factoryPoolMemory->setMutex( memoryFactoryMutex );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryManager::_finalize()
	{
		this->clearCacheBuffers();

		m_memoryCacheMutex = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPoolMemoryCacheBuffer );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPoolMemoryCacheInput );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPoolMemoryProxyInput );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPoolMemoryInput );
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPoolMemory );

        m_factoryPoolMemoryCacheBuffer = nullptr;
        m_factoryPoolMemoryCacheInput = nullptr;
        m_factoryPoolMemoryProxyInput = nullptr;
        m_factoryPoolMemoryInput = nullptr;
        m_factoryPoolMemory = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	CacheBufferID MemoryManager::lockBuffer( size_t _size, void ** _memory, const char * _file, uint32_t _line )
	{
		m_memoryCacheMutex->lock();

		CacheBufferID buffer_id = 
			this->lockBufferNoMutex_( _size, _memory, _file, _line );

		m_memoryCacheMutex->unlock();

		return buffer_id;
	}
	//////////////////////////////////////////////////////////////////////////
	CacheBufferID MemoryManager::lockBufferNoMutex_( size_t _size, void ** _memory, const char * _file, uint32_t _line )
	{
		size_t minSize = (size_t)(0);
		size_t maxSize = (size_t)(-1);

		const TVectorCacheBufferMemory::size_type invalidIndex = (TVectorCacheBufferMemory::size_type)-1;
		
		TVectorCacheBufferMemory::size_type minIndex = invalidIndex;
		TVectorCacheBufferMemory::size_type maxIndex = invalidIndex;

		for( TVectorCacheBufferMemory::size_type
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
						
			unsigned char * memory = Helper::reallocateMemory<unsigned char>( buffer.memory, _size );

			if( memory == nullptr )
			{
				LOGGER_ERROR("CacheManager::lockBuffer invalid realloc %p memory %d to %d"
					, buffer.memory
					, buffer.size
					, _size
					);
				
				return INVALID_CACHE_BUFFER_ID;
			}
					
			buffer.memory = memory;
			buffer.size = _size;
			buffer.file = _file;
            buffer.line = _line;
			buffer.lock = true;

			*_memory = buffer.memory;
			
			return buffer.id;
		}
		
		unsigned char * memory = Helper::allocateMemory<unsigned char>( _size );

		if( memory == nullptr )
		{
			LOGGER_ERROR("CacheManager::lockBuffer invalid malloc memory %d"
				, _size
				);

			return INVALID_CACHE_BUFFER_ID;
		}

		CacheBufferID new_id = ++m_enumeratorId;

		CacheBufferMemory buffer;
		buffer.id = new_id;
		buffer.memory = memory;
		buffer.size = _size;
        buffer.file = _file;
        buffer.line = _line;
		buffer.lock = true;

		m_buffers.push_back( buffer );

		*_memory = buffer.memory;

		return new_id;
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryManager::unlockBuffer( CacheBufferID _bufferId )
	{
		m_memoryCacheMutex->lock();

		for( TVectorCacheBufferMemory::iterator
			it = m_buffers.begin(),
			it_end = m_buffers.end();
		it != it_end;
		++it )
		{
			CacheBufferMemory & buffer = *it;

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
	void MemoryManager::clearCacheBuffers()
	{
		m_memoryCacheMutex->lock();

		for( TVectorCacheBufferMemory::iterator
			it = m_buffers.begin(),
			it_end = m_buffers.end();
		it != it_end;
		++it )
		{
			const CacheBufferMemory & buffer = *it;

			if( buffer.lock == true )
			{
				LOGGER_ERROR("CacheManager::finalize dont unlock buffer %d size %d"
					, buffer.id
					, buffer.size
					);
			}

			Helper::freeMemory( buffer.memory );
		}

		m_buffers.clear();

		m_memoryCacheMutex->unlock();
	}
	//////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr MemoryManager::createMemoryCacheBuffer()
	{
		MemoryCacheBuffer * memoryBuffer = m_factoryPoolMemoryCacheBuffer->createObject();

		memoryBuffer->setMemoryManager( this );

		return memoryBuffer;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryCacheInputInterfacePtr MemoryManager::createMemoryCacheInput()
	{
		MemoryCacheInput * memoryCache = m_factoryPoolMemoryCacheInput->createObject();

		memoryCache->setMemoryManager( this );

		return memoryCache;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryProxyInputInterfacePtr MemoryManager::createMemoryProxyInput()
	{
		MemoryProxyInput * memoryProxy = m_factoryPoolMemoryProxyInput->createObject();

		return memoryProxy;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryInputInterfacePtr MemoryManager::createMemoryInput()
	{
		MemoryInput * memory = m_factoryPoolMemoryInput->createObject();

		return memory;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryInterfacePtr MemoryManager::createMemory()
	{
		Memory * memory = m_factoryPoolMemory->createObject();

		return memory;
	}
}
