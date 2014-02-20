#	include "CacheMemoryBuffer.h"

#	include "Interface/CacheInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CacheMemoryBuffer::CacheMemoryBuffer( ServiceProviderInterface * _serviceProvider, size_t _size )
		: m_serviceProvider(_serviceProvider)
	{
		void * memory = nullptr;
		m_bufferId = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( _size, &memory );

		m_memory = static_cast<unsigned char *>(memory);
	}
	//////////////////////////////////////////////////////////////////////////
	CacheMemoryBuffer::~CacheMemoryBuffer()
	{
		CACHE_SERVICE(m_serviceProvider)
			->unlockBuffer( m_bufferId );
	}	
}