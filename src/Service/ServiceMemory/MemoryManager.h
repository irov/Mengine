#	pragma once

#	include "Interface/MemoryInterface.h"
#	include "Interface/MemoryInterface.h"
#	include "Interface/ThreadSystemInterface.h"

#	include "MemoryCacheBuffer.h"
#	include "MemoryCacheInput.h"
#	include "MemoryProxyInput.h"
#	include "MemoryInput.h"
#	include "Memory.h"

#	include "Factory/FactoryStore.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	typedef uint32_t CacheBufferID;

	static const CacheBufferID INVALID_CACHE_BUFFER_ID = 0;

	class MemoryManager
		: public MemoryServiceInterface
	{
	public:
		MemoryManager();
		~MemoryManager();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
		void finalize() override;

	public:
		CacheBufferID lockBuffer( size_t _size, void ** _memory, const char * _doc );
		void unlockBuffer( CacheBufferID _bufferId );

	public:
		void clearCacheBuffers() override;

	public:
		MemoryCacheBufferInterfacePtr createMemoryCacheBuffer() override;
		MemoryCacheInputInterfacePtr createMemoryCacheInput() override;
		MemoryProxyInputInterfacePtr createMemoryProxyInput() override;
		MemoryInputInterfacePtr createMemoryInput() override;
		MemoryInterfacePtr createMemory() override;

	protected:
		CacheBufferID lockBufferNoMutex_( size_t _size, void ** _memory, const char * _doc );
		
	protected:
		ServiceProviderInterface * m_serviceProvider;

		struct CacheBufferMemory
		{
			CacheBufferID id;
			void * memory;
			size_t size;
			const char * doc;
			bool lock;
		};

		typedef stdex::vector<CacheBufferMemory> TVectorCacheBufferMemory;
		TVectorCacheBufferMemory m_buffers;

		CacheBufferID m_enumeratorId;
			
		ThreadMutexInterfacePtr m_memoryCacheMutex;
		ThreadMutexInterfacePtr m_memoryFactoryMutex;

		typedef FactoryPoolStore<MemoryCacheBuffer, 16> TFactoryPoolMemoryCacheBuffer;
		TFactoryPoolMemoryCacheBuffer m_factoryPoolMemoryCacheBuffer;

		typedef FactoryPoolStore<MemoryCacheInput, 16> TFactoryPoolMemoryCacheInput;
		TFactoryPoolMemoryCacheInput m_factoryPoolMemoryCacheInput;

		typedef FactoryPoolStore<MemoryProxyInput, 16> TFactoryPoolMemoryProxyInput;
		TFactoryPoolMemoryProxyInput m_factoryPoolMemoryProxyInput;

		typedef FactoryPoolStore<MemoryInput, 16> TFactoryPoolMemoryInput;
		TFactoryPoolMemoryInput m_factoryPoolMemoryInput;

		typedef FactoryPoolStore<Memory, 16> TFactoryPoolMemory;
		TFactoryPoolMemory m_factoryPoolMemory;		
	};
}