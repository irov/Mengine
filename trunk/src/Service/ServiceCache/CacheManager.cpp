#	include "CacheManager.h"

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

			free( buffer.memory );
		}

		m_buffers.clear();

		m_memoryMutex->unlock();

		m_memoryMutex = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t CacheManager::lockBuffer( size_t _size, void ** _memory )
	{
		m_memoryMutex->lock();

		size_t minSize = (size_t)(0);
		size_t maxSize = (size_t)(-1);

		const size_t INVALID_ID = (size_t)(-1);

		size_t minIndex = INVALID_ID;
		size_t maxIndex = INVALID_ID;

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

		size_t buffer_id = 0;

		if( maxIndex != INVALID_ID )
		{
			CacheBufferMemory & buffer = m_buffers[maxIndex];
			
			buffer.lock = true;

			*_memory = buffer.memory;

			buffer_id = buffer.id;
		}
		else if( minIndex != INVALID_ID )
		{
			CacheBufferMemory & buffer = m_buffers[minIndex];

			buffer.lock = true;
			buffer.memory = realloc( buffer.memory, _size );
			buffer.size = _size;

			*_memory = buffer.memory;

			buffer_id = buffer.id;
		}
		else
		{
			size_t new_id = ++m_enumeratorId;

			CacheBufferMemory buffer;
			buffer.id = new_id;
			buffer.lock = true;
			buffer.memory = malloc( _size );
			buffer.size = _size;

			m_buffers.push_back( buffer );

			*_memory = buffer.memory;

			buffer_id = buffer.id;
		}

		m_memoryMutex->unlock();

		return buffer_id;
	}
	//////////////////////////////////////////////////////////////////////////
	void CacheManager::unlockBuffer( size_t _bufferId )
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
	InputStreamInterfacePtr CacheManager::lockStream( size_t _size, void ** _memory )
	{
		MemoryCacheInputPtr memoryCache = m_factoryPoolMemoryCacheInput.createObjectT();

		void * memory = memoryCache->cacheMemory( _size );

		*_memory = memory;

		return memoryCache;
	}
}