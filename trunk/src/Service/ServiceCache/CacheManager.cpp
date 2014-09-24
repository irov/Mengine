#	include "CacheManager.h"

#	include "Core/CacheMemoryBuffer.h"
#	include "Core/Memory.h"
#	include "Core/CRC32.h"

#	include "Logger/Logger.h"

#	include <malloc.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( CacheService, Menge::CacheServiceInterface, Menge::CacheManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CacheManager::CacheManager()
		: m_serviceProvider(nullptr)
		, m_enumeratorId(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	CacheManager::~CacheManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void CacheManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * CacheManager::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool CacheManager::initialize()
	{
		m_memoryMutex = THREAD_SERVICE(m_serviceProvider)
			->createMutex();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void CacheManager::finalize()
	{
		m_memoryMutex->lock();
		//MutexGuard guard(m_memoryMutex);

		for( TVectorCacheBufferMemory::iterator
			it = m_buffers.begin(),
			it_end = m_buffers.end();
		it != it_end;
		++it )
		{
			const CacheBufferMemory & buffer = *it;

			if( buffer.lock == true )
			{
				LOGGER_ERROR(m_serviceProvider)("CacheManager::finalize dont unlock buffer %d size %d"
					, buffer.id
					, buffer.size
					);
			}

			Helper::freeMemory( buffer.memory );
		}

		m_buffers.clear();

		m_memoryMutex->unlock();

		m_memoryMutex = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	CacheBufferID CacheManager::lockBuffer( size_t _size, void ** _memory, const char * _doc )
	{
		m_memoryMutex->lock();

		CacheBufferID buffer_id = this->lockBufferNoMutex_( _size, _memory, _doc );

		m_memoryMutex->unlock();

		return buffer_id;
	}
	//////////////////////////////////////////////////////////////////////////
	CacheBufferID CacheManager::lockBufferNoMutex_( size_t _size, void ** _memory, const char * _doc )
	{
		size_t minSize = (size_t)(0);
		size_t maxSize = (size_t)(-1);

		const CacheBufferID INVALID_ID = (size_t)(-1);

		CacheBufferID minIndex = INVALID_ID;
		CacheBufferID maxIndex = INVALID_ID;

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
		
		if( maxIndex != INVALID_ID )
		{
			CacheBufferMemory & buffer = m_buffers[maxIndex];
			
			buffer.doc = _doc;
			buffer.lock = true;			

			*_memory = buffer.memory;

			return buffer.id;
		}
		else if( minIndex != INVALID_ID )
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
				
				return 0;
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

			return 0;
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
	void CacheManager::unlockBuffer( CacheBufferID _bufferId )
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
	MemoryCacheInputPtr CacheManager::createMemoryCacheInput()
	{
		MemoryCacheInput * memoryCache = m_factoryPoolMemoryCacheInput.createObjectT();

		memoryCache->setServiceProvider( m_serviceProvider );

		return memoryCache;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryProxyInputPtr CacheManager::createMemoryProxyInput()
	{
		MemoryProxyInput * memoryProxy = m_factoryPoolMemoryProxyInput.createObjectT();

		memoryProxy->setServiceProvider( m_serviceProvider );

		return memoryProxy;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryInputPtr CacheManager::createMemoryInput()
	{
		MemoryInput * memory = m_factoryPoolMemoryInput.createObjectT();

		memory->setServiceProvider( m_serviceProvider );

		return memory;
	}
}