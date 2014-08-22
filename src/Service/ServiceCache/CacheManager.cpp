#	include "CacheManager.h"

#	include "Core/CacheMemoryBuffer.h"
//#	include "Core/MutexGuard.h"

#	include "Logger/Logger.h"

#	include <malloc.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( CacheService, Menge::CacheServiceInterface, Menge::CacheManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	CacheManager::CacheManager()
		: m_serviceProvider(nullptr)
		, m_enumeratorId(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	CacheManager::~CacheManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void CacheManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * CacheManager::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool CacheManager::initialize()
	{
		m_memoryMutex = THREAD_SERVICE(m_serviceProvider)
			->createMutex();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void CacheManager::finalize()
	{
		m_memoryMutex->lock();
		//MutexGuard guard(m_memoryMutex);

		for( TVectorCacheBufferMemory::iterator
			it = m_buffers.begin(),
			it_end = m_buffers.end();
		it != it_end;
		++it )
		{
			const CacheBufferMemory & buffer = *it;

			if( buffer.lock == true )
			{
				LOGGER_ERROR(m_serviceProvider)("CacheManager::finalize dont unlock buffer %d size %d"
					, buffer.id
					, buffer.size
					);
			}

			free( buffer.memory );
		}

		m_buffers.clear();

		m_memoryMutex->unlock();

		m_memoryMutex = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t CacheManager::lockBuffer( size_t _size, void ** _memory, const char * _doc )
	{
		m_memoryMutex->lock();

		size_t buffer_id = this->lockBufferNoMutex_( _size, _memory, _doc );

		m_memoryMutex->unlock();

		return buffer_id;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t CacheManager::lockBufferNoMutex_( size_t _size, void ** _memory, const char * _doc )
	{
		size_t minSize = (size_t)(0);
		size_t maxSize = (size_t)(-1);

		const size_t INVALID_ID = (size_t)(-1);

		size_t minIndex = INVALID_ID;
		size_t maxIndex = INVALID_ID;

		for( TVectorCacheBufferMemory::size_type
			it = 0,
			it_end = m_buffers.size();
		it != it_end;
		++it )
		{
			CacheBufferMemory & buffer = m_buffers[it];

			if( buffer.lock == true )
			{
				continue;
			}

			if( buffer.size > minSize && buffer.size <= _size )
			{
				minSize = buffer.size;
				minIndex = it;
			}

			if( buffer.size < maxSize && buffer.size >= _size )
			{
				maxSize = buffer.size;
				maxIndex = it;
			}
		}
		
		if( maxIndex != INVALID_ID )
		{
			CacheBufferMemory & buffer = m_buffers[maxIndex];
			
			buffer.doc = _doc;
			buffer.lock = true;			

			*_memory = buffer.memory;

			return buffer.id;
		}
		else if( minIndex != INVALID_ID )
		{
			CacheBufferMemory & buffer = m_buffers[minIndex];
						
			void * memory = realloc( buffer.memory, _size );

			if( memory == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("CacheManager::lockBuffer invalid realloc %p memory %d to %d"
					, buffer.memory
					, buffer.size
					, _size
					);
				
				return 0;
			}

			LOGGER_ERROR(m_serviceProvider)("CacheManager::lockBuffer realloc %d memory %d to %d"
				, buffer.id
				, buffer.size
				, _size
				);
						
			buffer.memory = memory;
			buffer.size = _size;
			buffer.doc = _doc;
			buffer.lock = true;

			*_memory = buffer.memory;
			
			return buffer.id;
		}
		
		void * memory = malloc( _size );

		if( memory == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("CacheManager::lockBuffer invalid malloc memory %d"
				, _size
				);

			return 0;
		}

		size_t new_id = ++m_enumeratorId;

		CacheBufferMemory buffer;
		buffer.id = new_id;		
		buffer.memory = memory;
		buffer.size = _size;
		buffer.doc = _doc;		
		buffer.lock = true;

		LOGGER_ERROR(m_serviceProvider)("CacheManager::lockBuffer new %d memory %d to %d"
			, buffer.id
			, buffer.size
			, _size
			);


		m_buffers.push_back( buffer );

		*_memory = buffer.memory;

		return new_id;
	}
	//////////////////////////////////////////////////////////////////////////
	void CacheManager::unlockBuffer( size_t _bufferId )
	{
		m_memoryMutex->lock();

		for( TVectorCacheBufferMemory::iterator
			it = m_buffers.begin(),
			it_end = m_buffers.end();
		it != it_end;
		++it )
		{
			CacheBufferMemory & buffer = *it;

			if( buffer.id != _bufferId )
			{
				continue;
			}

			buffer.lock = false;

			break;
		}

		m_memoryMutex->unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryCacheInputPtr CacheManager::createMemoryCacheInput()
	{
		MemoryCacheInput * memoryCache = m_factoryPoolMemoryCacheInput.createObjectT();

		return memoryCache;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryProxyInputPtr CacheManager::createMemoryProxyInput()
	{
		MemoryProxyInput * memoryProxy = m_factoryPoolMemoryProxyInput.createObjectT();

		return memoryProxy;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryInputPtr CacheManager::createMemoryInput()
	{
		MemoryInput * memory = m_factoryPoolMemoryInput.createObjectT();

		return memory;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t CacheManager::getArchiveData( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, unsigned char ** _data, size_t & _size )
	{
		magic_number_type magic_number;
		_stream->read( &magic_number, sizeof(magic_number) );

		if( magic_number != _magic )
		{
			LOGGER_ERROR(m_serviceProvider)("ArchiveService::getData: invalid magic number %u need %u"
				, magic_number
				, _magic
				);

			return 0;
		}

		magic_version_type magic_version;
		_stream->read( &magic_version, sizeof(magic_version) );

		if( magic_version != _version )
		{
			LOGGER_ERROR(m_serviceProvider)("ArchiveService::getData: invalid magic version %u need %u"
				, magic_version
				, _version
				);

			return 0;
		}

		uint32_t binary_size;
		_stream->read( &binary_size, sizeof(binary_size) );

		uint32_t compress_size;
		_stream->read( &compress_size, sizeof(compress_size) );

		CacheMemoryBuffer compress_buffer(m_serviceProvider, compress_size, "ArchiveService::getData compress_memory");
		void * compress_memory = compress_buffer.getMemory();

		if( compress_memory == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ArchiveService::getData: invalid get memory %d (compress)"
				, compress_size
				);

			return 0;
		}

		size_t read_data = _stream->read( compress_memory, compress_size );

		if( read_data != compress_size )
		{
			LOGGER_ERROR(m_serviceProvider)("ArchiveService::getData: invalid read data %d need %d"
				, read_data
				, compress_size
				);

			return 0;
		}

		void * binary_memory = nullptr;
		size_t binary_buffer = this->lockBuffer( binary_size, &binary_memory, "ArchiveService::getData binary_memory" );

		if( binary_buffer == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("ArchiveService::getData: invalid get memory %d (binary)"
				, binary_size
				);

			return 0;
		}

		size_t uncompressSize = 0;
		if( _archivator->decompress( binary_memory, binary_size, compress_memory, compress_size, uncompressSize ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ArchiveService::getData: invalid decompress"
				);

			this->unlockBuffer( binary_buffer );

			return 0;
		}

		*_data = static_cast<unsigned char *>(binary_memory);
		_size = binary_size;

		return binary_buffer;
	}
}