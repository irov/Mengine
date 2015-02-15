#	include "CacheMemoryStream.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CacheMemoryStream::CacheMemoryStream( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const char * _doc )
		: m_serviceProvider(_serviceProvider)
		, m_bufferId(INVALID_CACHE_BUFFER_ID)
		, m_memory(nullptr)
	{
		size_t size = _stream->size();

		void * memory = nullptr;
		CacheBufferID bufferId = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( size, &memory, _doc );

		if( bufferId == INVALID_CACHE_BUFFER_ID )
		{
			return;
		}
		
		m_bufferId = bufferId;
		m_memory = static_cast<unsigned char *>(memory);
		m_size = size;

		size_t read_byte = _stream->read( m_memory, m_size );

		if( read_byte != m_size )
		{
			CACHE_SERVICE(m_serviceProvider)
				->unlockBuffer( m_bufferId );
			
			m_bufferId = INVALID_CACHE_BUFFER_ID;
			m_memory = nullptr;
			m_size = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	CacheMemoryStream::CacheMemoryStream( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, size_t _size, const char * _doc )
		: m_serviceProvider(_serviceProvider)
		, m_bufferId(INVALID_CACHE_BUFFER_ID)
		, m_memory(nullptr)
	{
		void * memory = nullptr;
		CacheBufferID bufferId = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( _size, &memory, _doc );

		if( bufferId == INVALID_CACHE_BUFFER_ID )
		{
			return;
		}
				
		m_bufferId = bufferId;
		m_memory = static_cast<unsigned char *>(memory);
		m_size = _size;

		size_t read_byte = _stream->read( m_memory, m_size );

		if( read_byte != m_size )
		{
			CACHE_SERVICE(m_serviceProvider)
				->unlockBuffer( m_bufferId );

			m_bufferId = INVALID_CACHE_BUFFER_ID;
			m_memory = nullptr;
			m_size = 0;
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	CacheMemoryStream::~CacheMemoryStream()
	{
		if( m_bufferId != INVALID_CACHE_BUFFER_ID )
		{
			CACHE_SERVICE(m_serviceProvider)
				->unlockBuffer( m_bufferId );
		}
	}
}