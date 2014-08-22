#	pragma once

#	include "Interface/CacheInterface.h"
#	include "Interface/ThreadSystemInterface.h"

#	include "Factory/FactoryStore.h"

#	include <stdex/stl_vector.h>

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
		size_t lockBuffer( size_t _size, void ** _memory, const char * _doc ) override;
		void unlockBuffer( size_t _bufferId ) override;

	public:
		size_t getArchiveData( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, unsigned char ** _data, size_t & _size ) override;

	public:
		MemoryCacheInputPtr createMemoryCacheInput() override;
		MemoryProxyInputPtr createMemoryProxyInput() override;
		MemoryInputPtr createMemoryInput() override;

	protected:
		size_t lockBufferNoMutex_( size_t _size, void ** _memory, const char * _doc );
		
	protected:
		ServiceProviderInterface * m_serviceProvider;

		struct CacheBufferMemory
		{
			size_t id;
			void * memory;
			size_t size;
			const char * doc;
			bool lock;
		};

		typedef stdex::vector<CacheBufferMemory> TVectorCacheBufferMemory;
		TVectorCacheBufferMemory m_buffers;

		size_t m_enumeratorId;
			
		ThreadMutexInterfacePtr m_memoryMutex;

		typedef FactoryPoolStore<MemoryCacheInput, 16> TFactoryPoolMemoryCacheInput;
		TFactoryPoolMemoryCacheInput m_factoryPoolMemoryCacheInput;

		typedef FactoryPoolStore<MemoryProxyInput, 16> TFactoryPoolMemoryProxyInput;
		TFactoryPoolMemoryProxyInput m_factoryPoolMemoryProxyInput;

		typedef FactoryPoolStore<MemoryInput, 16> TFactoryPoolMemoryInput;
		TFactoryPoolMemoryInput m_factoryPoolMemoryInput;
	};
}