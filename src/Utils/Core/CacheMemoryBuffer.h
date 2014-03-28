#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Config/Typedef.h"

namespace Menge
{
	class CacheMemoryBuffer
	{
	public:
		CacheMemoryBuffer( ServiceProviderInterface * _serviceProvider, size_t _size, const char * _doc );
		~CacheMemoryBuffer();

	public:
		inline void * getMemory() const
		{
			return m_memory;
		}

		template<class T>
		inline T * getMemoryT() const
		{
			return static_cast<T *>(m_memory);
		}

	public:
		ServiceProviderInterface * m_serviceProvider;

		void * m_memory;
		size_t m_bufferId;
	};
}