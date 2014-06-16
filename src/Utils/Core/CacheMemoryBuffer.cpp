#	include "CacheMemoryBuffer.h"

#	include "Interface/CacheInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CacheMemoryBuffer::CacheMemoryBuffer( ServiceProviderInterface * _serviceProvider, size_t _size, const char * _doc )
		: m_serviceProvider(_serviceProvider)
		, m_size(_size)
		, m_bufferId(0)		
		, m_memory(nullptr)
	{
		void * memory = nullptr;
		size_t bufferId = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( m_size, &memory, _doc );

		if( bufferId != 0 )
		{
			m_bufferId = bufferId;
			m_memory = static_cast<unsigned char *>(memory);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	CacheMemoryBuffer::~CacheMemoryBuffer()
	{
		if( m_bufferId != 0 )
		{
			CACHE_SERVICE(m_serviceProvider)
				->unlockBuffer( m_bufferId );
		}
	}
}