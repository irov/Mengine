#	pragma once

#	include "Interface/CacheInterface.h"
#	include "Interface/ThreadSystemInterface.h"

#	include "MemoryCacheInput.h"

#	include "Factory/FactoryStore.h"

#	include <vector>

namespace Menge
{
	class CacheManager
		: public CacheServiceInterface
	{
	public:
		CacheManager();
		~CacheManager();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
		void finalize() override;

	public:
		size_t lockBuffer( size_t _size, void ** _memory ) override;
		void unlockBuffer( size_t _bufferId ) override;

	public:
		InputStreamInterfacePtr lockStream( size_t _size, void ** _memory ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		struct CacheBufferMemory
		{
			size_t id;
			void * memory;
			size_t size;
			bool lock;
		};

		typedef std::vector<CacheBufferMemory> TVectorCacheBufferMemory;
		TVectorCacheBufferMemory m_buffers;

		size_t m_enumeratorId;
				
		ThreadMutexInterfacePtr m_memoryMutex;

		typedef FactoryPoolStore<MemoryCacheInput, 16> TFactoryPoolMemoryCacheInput;
		TFactoryPoolMemoryCacheInput m_factoryPoolMemoryCacheInput;
	};
}