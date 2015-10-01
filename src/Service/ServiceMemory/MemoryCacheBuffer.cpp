#	include "MemoryCacheBuffer.h"

#	include "Interface/MemoryInterface.h"
#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MemoryCacheBuffer::MemoryCacheBuffer()
		: m_serviceProvider(nullptr)
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
	void MemoryCacheBuffer::uncache_()
	{
		if( m_bufferId != 0 )
		{
			MEMORY_SERVICE(m_serviceProvider)
				->unlockBuffer( m_bufferId );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void * MemoryCacheBuffer::cacheMemory( size_t _size, const char * _doc )
	{
		this->uncache_();

		void * memory;
		uint32_t bufferId = MEMORY_SERVICE(m_serviceProvider)
			->lockBuffer( _size, &memory, _doc );

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
