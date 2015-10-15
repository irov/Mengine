#	include "MemoryManager.h"

#	include "Logger/Logger.h"

#	include <malloc.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( MemoryService, Menge::MemoryServiceInterface, Menge::MemoryManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MemoryManager::MemoryManager()
		: m_serviceProvider(nullptr)
		, m_enumeratorId(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryManager::~MemoryManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * MemoryManager::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MemoryManager::initialize()
	{
		m_memoryMutex = THREAD_SERVICE(m_serviceProvider)
			->createMutex();

		m_factoryPoolMemoryCacheBuffer.setMutex( m_memoryMutex );
		m_factoryPoolMemoryCacheInput.setMutex( m_memoryMutex );
		m_factoryPoolMemoryProxyInput.setMutex( m_memoryMutex );
		m_factoryPoolMemoryInput.setMutex( m_memoryMutex );
		m_factoryPoolMemory.setMutex( m_memoryMutex );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryManager::finalize()
	{
		this->clearCacheBuffers();

		m_memoryMutex = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	CacheBufferID MemoryManager::lockBuffer( size_t _size, void ** _memory, const char * _doc )
	{
		m_memoryMutex->lock();

		CacheBufferID buffer_id = 
			this->lockBufferNoMutex_( _size, _memory, _doc );

		m_memoryMutex->unlock();

		return buffer_id;
	}
	//////////////////////////////////////////////////////////////////////////
	CacheBufferID MemoryManager::lockBufferNoMutex_( size_t _size, void ** _memory, const char * _doc )
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
			
			buffer.doc = _doc;
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
				LOGGER_ERROR(m_serviceProvider)("CacheManager::lockBuffer invalid realloc %p memory %d to %d"
					, buffer.memory
					, buffer.size
					, _size
					);
				
				return INVALID_CACHE_BUFFER_ID;
			}
					
			buffer.memory = memory;
			buffer.size = _size;
			buffer.doc = _doc;
			buffer.lock = true;

			*_memory = buffer.memory;
			
			return buffer.id;
		}
		
		unsigned char * memory = Helper::allocateMemory<unsigned char>( _size );

		if( memory == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("CacheManager::lockBuffer invalid malloc memory %d"
				, _size
				);

			return INVALID_CACHE_BUFFER_ID;
		}

		CacheBufferID new_id = ++m_enumeratorId;

		CacheBufferMemory buffer;
		buffer.id = new_id;
		buffer.memory = memory;
		buffer.size = _size;
		buffer.doc = _doc;		
		buffer.lock = true;

		m_buffers.push_back( buffer );

		*_memory = buffer.memory;

		return new_id;
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryManager::unlockBuffer( CacheBufferID _bufferId )
	{
		m_memoryMutex->lock();

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

		m_memoryMutex->unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryManager::clearCacheBuffers()
	{
		m_memoryMutex->lock();

		for( TVectorCacheBufferMemory::iterator
			it = m_buffers.begin(),
			it_end = m_buffers.end();
		it != it_end;
		++it )
		{
			const CacheBufferMemory & buffer = *it;

			if( buffer.lock == true )
			{
				LOGGER_ERROR( m_serviceProvider )("CacheManager::finalize dont unlock buffer %d size %d"
					, buffer.id
					, buffer.size
					);
			}

			Helper::freeMemory( buffer.memory );
		}

		m_buffers.clear();

		m_memoryMutex->unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryCacheBufferInterfacePtr MemoryManager::createMemoryCacheBuffer()
	{
		MemoryCacheBuffer * memoryBuffer = m_factoryPoolMemoryCacheBuffer.createObjectT();

		memoryBuffer->setServiceProvider( m_serviceProvider );

		memoryBuffer->setMemoryManager( this );

		return memoryBuffer;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryCacheInputInterfacePtr MemoryManager::createMemoryCacheInput()
	{
		MemoryCacheInput * memoryCache = m_factoryPoolMemoryCacheInput.createObjectT();

		memoryCache->setServiceProvider( m_serviceProvider );

		memoryCache->setMemoryManager( this );

		return memoryCache;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryProxyInputInterfacePtr MemoryManager::createMemoryProxyInput()
	{
		MemoryProxyInput * memoryProxy = m_factoryPoolMemoryProxyInput.createObjectT();

		memoryProxy->setServiceProvider( m_serviceProvider );

		return memoryProxy;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryInputInterfacePtr MemoryManager::createMemoryInput()
	{
		MemoryInput * memory = m_factoryPoolMemoryInput.createObjectT();

		memory->setServiceProvider( m_serviceProvider );

		return memory;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryInterfacePtr MemoryManager::createMemory()
	{
		Memory * memory = m_factoryPoolMemory.createObjectT();

		memory->setServiceProvider( m_serviceProvider );

		return memory;
	}
}