#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/MemoryInterface.h"

#   include "Factory/FactorablePtr.h"

#	include "Core/FilePath.h"
#	include "Core/ConstString.h"

namespace Menge
{
	class MemoryManager;

	class MemoryCacheBuffer
		: public MemoryCacheBufferInterface
	{
	public:
		MemoryCacheBuffer();
		~MemoryCacheBuffer();
		
	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );
		void setMemoryManager( MemoryManager * _memoryManager );

    public:
        void * cacheMemory( size_t _size, const char * _doc ) override;
		
		inline void * getMemory() const override;

		inline size_t getSize() const override;

	protected:
		void uncache_();

	protected:
		ServiceProviderInterface * m_serviceProvider;
		MemoryManager * m_memoryManager;

		uint32_t m_bufferId;

		unsigned char * m_data;
		size_t m_size;
	};
	//////////////////////////////////////////////////////////////////////////	
	void * MemoryCacheBuffer::getMemory() const
	{
		return m_data;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t MemoryCacheBuffer::getSize() const
	{
		return m_size;
	}
}	// namespace Menge


