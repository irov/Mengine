#	include "ArchiveService.h"

#	include "Core/CacheMemoryBuffer.h"
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
			LOGGER_ERROR(m_serviceProvider)("ArchiveService::unregisterArchivator: not registry archivator '%s'"
				, _type.c_str() 
				);

			return nullptr;
		}

		const ArchivatorInterfacePtr & archivator = m_archivators.get_value(it_found);

		return archivator;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ArchiveService::decompress( const ConstString & _type, const InputStreamInterfacePtr & _stream, size_t _size, void * _memory, size_t _capacity, size_t & _uncompress )
	{
		ArchivatorInterfacePtr archivator = this->getArchivator( _type );

		if( archivator == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ArchiveService::decomress: invalid get archivator %s"
				, _type.c_str()
				);

			return false;
		}

		CacheMemoryBuffer compress_buffer(m_serviceProvider, _size, "ArchiveService::decomress");
		void * compress_memory = compress_buffer.getMemory();

		size_t read_memory = _stream->read( compress_memory, _size );

		if( read_memory != _size )
		{
			LOGGER_ERROR(m_serviceProvider)("ArchiveService::decomress: invalid compress buffer %s read %d need %d"
				, _type.c_str()
				, read_memory
				, _size
				);

			return false;
		}

		size_t uncompressSize = 0;
		if( archivator->decompress( _memory, _capacity, compress_memory, _size, uncompressSize ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("ArchiveService::decomress: invalid decompress"
				);

			return false;
		}

		_uncompress = uncompressSize;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryInputPtr ArchiveService::compress( const ConstString & _type, const void * _buffer, size_t _size )
	{
		ArchivatorInterfacePtr archivator = this->getArchivator( _type );

		if( archivator == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ArchiveService::compress: invalid get archivator %s"
				, _type.c_str()
				);

			return nullptr;
		}

		size_t compressSize2 = archivator->compressBound( _size );

		MemoryInputPtr memory = m_factoryMemoryInput.createObjectT();
		void * buffer = memory->newMemory( compressSize2 );

		size_t compressSize;
		if( archivator->compress( buffer, compressSize2, _buffer, _size, compressSize ) == false )
		{
			return nullptr;
		}

		memory->newMemory( compressSize );

		return memory;
	}
}
