#	pragma once

#	include "Interface/MemoryInterface.h"
#	include "Interface/MemoryInterface.h"
#	include "Interface/ThreadSystemInterface.h"

#	include "Core/MemoryCacheBuffer.h"
#	include "Core/MemoryCacheInput.h"
#	include "Core/MemoryProxyInput.h"
#	include "Core/MemoryInput.h"
#	include "Core/Memory.h"

#	include "Factory/FactoryStore.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
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
		CacheBufferID lockBuffer( size_t _size, void ** _memory, const char * _doc ) override;
		void unlockBuffer( CacheBufferID _bufferId ) override;

	public:
		void clearBuffers() override;

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
			
		ThreadMutexInterfacePtr m_memoryMutex;

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