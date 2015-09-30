#	include "ArchiveService.h"

#	include "Interface/CacheInterface.h"

#	include "Core/MemoryCacheBuffer.h"

#	include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ArchiveService, Menge::ArchiveServiceInterface, Menge::ArchiveService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ArchiveService::ArchiveService()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ArchiveService::~ArchiveService()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void ArchiveService::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * ArchiveService::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	void ArchiveService::registerArchivator( const ConstString & _type, const ArchivatorInterfacePtr & _archivator )
	{
		m_archivators.insert( _type, _archivator );
	}
	//////////////////////////////////////////////////////////////////////////
	void ArchiveService::unregisterArchivator( const ConstString & _type )
	{
		TMapArchivators::iterator it_found = m_archivators.find( _type );

		if( it_found == m_archivators.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("ArchiveService::unregisterArchivator: not registry archivator '%s'"
				, _type.c_str() 
				);

			return;
		}

		m_archivators.erase( it_found );
	}
	//////////////////////////////////////////////////////////////////////////
	ArchivatorInterfacePtr ArchiveService::getArchivator( const ConstString & _type ) const
	{
		TMapArchivators::const_iterator it_found = m_archivators.find( _type );

		if( it_found == m_archivators.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("ArchiveService::getArchivator: not registry archivator '%s'"
				, _type.c_str() 
				);

			return nullptr;
		}

		const ArchivatorInterfacePtr & archivator = m_archivators.get_value(it_found);

		return archivator;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ArchiveService::decompressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream, size_t _size, void * _memory, size_t _capacity, size_t & _uncompress )
	{
		MemoryCacheBufferInterfacePtr compress_buffer = Helper::createMemoryStreamSize( m_serviceProvider, _stream, _size, "ArchiveService::decompressStream" );

		if( compress_buffer == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("ArchiveService::decomress: invalid compress buffer %d"
				, _size
				);

			return false;
		}

		const void * compress_memory = compress_buffer->getMemory();

		size_t uncompressSize = 0;
		if( _archivator->decompress( _memory, _capacity, compress_memory, _size, uncompressSize ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ArchiveService::decomress: invalid decompress"
				);

			return false;
		}

		_uncompress = uncompressSize;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryInputInterfacePtr ArchiveService::compressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream )
	{
		MemoryCacheBufferInterfacePtr uncompress_buffer = Helper::createMemoryStream( m_serviceProvider, _stream, "ArchiveService::compressStream" );

		if( uncompress_buffer == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("ArchiveService::compressStream: invalid cache buffer"
				);

			return nullptr;
		}

		const void * uncompress_memory = uncompress_buffer->getMemory();
		size_t uncompress_size = uncompress_buffer->getSize();
				
		MemoryInputInterfacePtr compress_memory = this->compressBuffer( _archivator, uncompress_memory, uncompress_size );

		return compress_memory;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryInputInterfacePtr ArchiveService::compressBuffer( const ArchivatorInterfacePtr & _archivator, const void * _buffer, size_t _size )
	{
		size_t compressSize2 = _archivator->compressBound( _size );

		MemoryInputInterfacePtr memory = CACHE_SERVICE( m_serviceProvider )
			->createMemoryInput();

		void * buffer = memory->newMemory( compressSize2 );

		if( buffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ArchiveService::compress: invalid new memory"
				);

			return nullptr;
		}

		size_t compressSize;
		if( _archivator->compress( buffer, compressSize2, _buffer, _size, compressSize ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ArchiveService::compress: invalid compress"
				);

			return nullptr;
		}

		if( memory->newMemory( compressSize ) == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ArchiveService::compress: invalid new memory"
				);

			return nullptr;
		}

		return memory;
	}
}
