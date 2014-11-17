#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/CacheInterface.h"

#	include "Config/Typedef.h"

namespace Menge
{
	class CacheMemoryStream
	{
	public:
		CacheMemoryStream( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const char * _doc );
		CacheMemoryStream( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, size_t _size, const char * _doc );
		~CacheMemoryStream();

	public:
		inline size_t getSize() const
		{
			return m_size;
		}

	public:
		inline const void * getMemory() const
		{
			return m_memory;
		}

		template<class T>
		inline const T * getMemoryT() const
		{
			return static_cast<const T *>(m_memory);
		}

	public:
		ServiceProviderInterface * m_serviceProvider;
		size_t m_size;

		void * m_memory;
		CacheBufferID m_bufferId;
	};
}