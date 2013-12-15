#	include "CacheManager.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( CacheService, Menge::CacheServiceInterface, Menge::CacheManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CacheManager::CacheManager()
		: m_serviceProvider(nullptr)
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
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void CacheManager::finalize()
	{
		for( TVectorCacheBufferMemories::iterator
			it = m_memories.begin(),
			it_end = m_memories.end();
		it != it_end;
		++it )
		{
			const CacheBufferMemory & memory = *it;

			delete [] memory.buffer;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	CacheBufferInterfacePtr CacheManager::lockBuffer( size_t _size )
	{
		size_t size_found = (size_t)-1;
		size_t size_unlock = 0;
		TVectorCacheBufferMemories::iterator it_found = m_memories.end();

		TVectorCacheBufferMemories::iterator it_unlock = m_memories.end();
		
		for( TVectorCacheBufferMemories::iterator
			it = m_memories.begin(),
			it_end = m_memories.end();
		it != it_end;
		++it )
		{
			const CacheBufferMemory & memory = *it;
			
			if( memory.lock == true )
			{
				continue;
			}

			if( size_unlock < memory.size )
			{
				size_unlock = memory.size;
				it_unlock = it;
			}

			if( memory.size < _size )
			{
				continue;
			}

			if( size_found > memory.size )
			{
				size_found = memory.size;
				it_found = it;
			}
		}

		if( it_found != m_memories.end() )
		{
		} 
		else if( it_unlock != m_memories.end() )
		{
			CacheBufferMemory & memory = *it_unlock;

			memory.buffer = (TBlobject::value_type *)realloc( memory.buffer, _size );
			memory.size = _size;

			it_found = it_unlock;
		}
		else
		{
			CacheBufferMemory memory;
			memory.buffer = (TBlobject::value_type *)malloc(_size);
			memory.size = _size;
			memory.lock = false;

			it_found = m_memories.insert( m_memories.end(), memory );
		}

		CacheBufferMemory & memory = *it_found;
		memory.lock = true;

		CacheBuffer * buffer = m_factoryCacheBuffer.createObjectT();
		buffer->setBuffer( &memory );

		return buffer;
	}
}