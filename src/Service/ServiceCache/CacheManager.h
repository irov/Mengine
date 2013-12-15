#	pragma once

#	include "Interface/CacheInterface.h"

#	include "CacheBuffer.h"

#	include "Factory/FactoryPool.h"

#	include <list>

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
		CacheBufferInterfacePtr lockBuffer( size_t _size ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;
				
		typedef FactoryPool<CacheBuffer, 4> TFactoryPoolCacheBuffer;
		TFactoryPoolCacheBuffer m_factoryCacheBuffer;

		typedef std::list<CacheBufferMemory> TVectorCacheBufferMemories;
		TVectorCacheBufferMemories m_memories;
	};
}