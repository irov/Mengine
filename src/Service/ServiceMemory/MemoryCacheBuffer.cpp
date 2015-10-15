#	include "MemoryCacheBuffer.h"

#	include "MemoryManager.h"

#	include "Interface/MemoryInterface.h"
#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MemoryCacheBuffer::MemoryCacheBuffer()
		: m_serviceProvider(nullptr)
		, m_memoryManager(nullptr)
		, m_bufferId(0)
		, m_data(nullptr)
		, m_size(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryCacheBuffer::~MemoryCacheBuffer()
	{
		this->uncache_();
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryCacheBuffer::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryCacheBuffer::setMemoryManager( MemoryManager * _memoryManager )
	{
		m_memoryManager = _memoryManager;
	}
	//////////////////////////////////////////////////////////////////////////
	void MemoryCacheBuffer::uncache_()
	{
		if( m_bufferId != 0 )
		{
			m_memoryManager->unlockBuffer( m_bufferId );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void * MemoryCacheBuffer::cacheMemory( size_t _size, const char * _doc )
	{
		this->uncache_();

		void * memory;
		uint32_t bufferId = m_memoryManager->lockBuffer( _size, &memory, _doc );

		if( bufferId == INVALID_CACHE_BUFFER_ID )
		{
			return nullptr;
		}

		m_bufferId = bufferId;

		m_data = reinterpret_cast<unsigned char *>(memory);
		m_size = _size;

        return m_data;
	}
}	// namespace Menge
