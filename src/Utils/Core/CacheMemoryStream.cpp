#	include "CacheMemoryStream.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CacheMemoryStream::CacheMemoryStream( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const char * _doc )
		: m_serviceProvider(_serviceProvider)
		, m_bufferId(0)
		, m_memory(nullptr)
	{
		m_size = _stream->size();

		void * memory = nullptr;
		CacheBufferID bufferId = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( m_size, &memory, _doc );

		if( bufferId != 0 )
		{
			m_bufferId = bufferId;
			m_memory = static_cast<unsigned char *>(memory);

			size_t read_byte = _stream->read( m_memory, m_size );

			if( read_byte != m_size )
			{
				m_memory = nullptr;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	CacheMemoryStream::CacheMemoryStream( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, size_t _size, const char * _doc )
		: m_serviceProvider(_serviceProvider)
		, m_bufferId(0)
		, m_memory(nullptr)
	{
		m_size = _size;

		void * memory = nullptr;
		CacheBufferID bufferId = CACHE_SERVICE(m_serviceProvider)
			->lockBuffer( m_size, &memory, _doc );

		if( bufferId != 0 )
		{
			m_bufferId = bufferId;
			m_memory = static_cast<unsigned char *>(memory);

			size_t read_byte = _stream->read( m_memory, m_size );

			if( read_byte != m_size )
			{
				m_memory = nullptr;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	CacheMemoryStream::~CacheMemoryStream()
	{
		if( m_bufferId != 0 )
		{
			CACHE_SERVICE(m_serviceProvider)
				->unlockBuffer( m_bufferId );
		}
	}
}