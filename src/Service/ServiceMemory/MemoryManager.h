#	pragma once

#	include "Interface/MemoryInterface.h"
#	include "Interface/MemoryInterface.h"
#	include "Interface/ThreadSystemInterface.h"

#   include "Core/ServiceBase.h"

#	include "MemoryCacheBuffer.h"
#	include "MemoryCacheInput.h"
#	include "MemoryProxyInput.h"
#	include "MemoryInput.h"
#	include "Memory.h"

#	include "Factory/Factory.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	typedef uint32_t CacheBufferID;

	static const CacheBufferID INVALID_CACHE_BUFFER_ID = 0;

	class MemoryManager
		: public ServiceBase<MemoryServiceInterface>
	{
	public:
		MemoryManager();
		~MemoryManager();

	public:
		bool _initialize() override;
		void _finalize() override;

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

		FactoryPtr m_factoryPoolMemoryCacheBuffer;
		FactoryPtr m_factoryPoolMemoryCacheInput;
		FactoryPtr m_factoryPoolMemoryProxyInput;
		FactoryPtr m_factoryPoolMemoryInput;
		FactoryPtr m_factoryPoolMemory;
	};
}