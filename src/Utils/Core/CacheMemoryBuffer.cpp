#	include "CacheMemoryBuffer.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CacheMemoryBuffer::CacheMemoryBuffer( ServiceProviderInterface * _serviceProvider, size_t _size, const char * _doc )
		: m_serviceProvider(_serviceProvider)
		, m_size(_size)
		, m_bufferId(INVALID_CACHE_BUFFER_ID)		
		, m_memory(nullptr)
	{
		void * memory = nullptr;
		CacheBufferID bufferId = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( m_size, &memory, _doc );

		if( bufferId != INVALID_CACHE_BUFFER_ID )
		{
			m_bufferId = bufferId;
			m_memory = static_cast<unsigned char *>(memory);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	CacheMemoryBuffer::~CacheMemoryBuffer()
	{
		if( m_bufferId != INVALID_CACHE_BUFFER_ID )
		{
			CACHE_SERVICE(m_serviceProvider)
				->unlockBuffer( m_bufferId );
		}
	}
}