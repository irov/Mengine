#include "ArchiveService.h"

#include "Interface/MemoryInterface.h"

#include "Kernel/MemoryHelper.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ArchiveService, Mengine::ArchiveService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	ArchiveService::ArchiveService()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ArchiveService::~ArchiveService()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ArchiveService::registerArchivator( const ConstString & _type, const ArchivatorInterfacePtr & _archivator )
	{
		m_archivators.insert( std::make_pair(_type, _archivator) );
	}
	//////////////////////////////////////////////////////////////////////////
	void ArchiveService::unregisterArchivator( const ConstString & _type )
	{
		MapArchivators::iterator it_found = m_archivators.find( _type );

		if( it_found == m_archivators.end() )
		{
			LOGGER_ERROR("ArchiveService::unregisterArchivator: not registry archivator '%s'"
				, _type.c_str() 
				);

			return;
		}

		m_archivators.erase( it_found );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ArchiveService::hasArchivator( const ConstString & _type ) const
	{
		MapArchivators::const_iterator it_found = m_archivators.find( _type );

		if( it_found == m_archivators.end() )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const ArchivatorInterfacePtr & ArchiveService::getArchivator( const ConstString & _type ) const
	{
		MapArchivators::const_iterator it_found = m_archivators.find( _type );

		if( it_found == m_archivators.end() )
		{
			LOGGER_ERROR("ArchiveService::getArchivator: not registry archivator '%s'"
				, _type.c_str() 
				);

			return ArchivatorInterfacePtr::none();
		}

		const ArchivatorInterfacePtr & archivator = it_found->second;

		return archivator;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ArchiveService::decompressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream, size_t _size, void * _memory, size_t _capacity, size_t & _uncompress )
	{
		MemoryInterfacePtr compress_buffer = Helper::createMemoryCacheStreamSize( _stream, _size, __FILE__, __LINE__ );

		if( compress_buffer == nullptr )
		{
			LOGGER_ERROR("ArchiveService::decomress: invalid compress buffer %d"
				, _size
				);

			return false;
		}

		const void * compress_memory = compress_buffer->getBuffer();

		size_t uncompressSize = 0;
		if( _archivator->decompress( _memory, _capacity, compress_memory, _size, uncompressSize ) == false )
		{
			LOGGER_ERROR("ArchiveService::decomress: invalid decompress"
				);

			return false;
		}

		_uncompress = uncompressSize;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryInputInterfacePtr ArchiveService::compressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream, EArchivatorCompress _compress )
	{
		MemoryInterfacePtr uncompress_buffer = Helper::createMemoryCacheStream( _stream, __FILE__, __LINE__ );

		if( uncompress_buffer == nullptr )
		{
			LOGGER_ERROR("ArchiveService::compressStream: invalid cache buffer"
				);

			return nullptr;
		}

		const void * uncompress_memory = uncompress_buffer->getBuffer();
		size_t uncompress_size = uncompress_buffer->getSize();
				
		MemoryInputInterfacePtr compress_memory = this->compressBuffer( _archivator, uncompress_memory, uncompress_size, _compress );

		return compress_memory;
	}
	//////////////////////////////////////////////////////////////////////////
	MemoryInputInterfacePtr ArchiveService::compressBuffer( const ArchivatorInterfacePtr & _archivator, const void * _buffer, size_t _size, EArchivatorCompress _compress )
	{
		size_t compressSize2 = _archivator->compressBound( _size );

		MemoryInputInterfacePtr memory = MEMORY_SERVICE()
			->createMemoryInput();

		void * buffer = memory->newBuffer( compressSize2 );

		if( buffer == nullptr )
		{
			LOGGER_ERROR("ArchiveService::compress: invalid new memory"
				);

			return nullptr;
		}

		size_t compressSize;
		if( _archivator->compress( buffer, compressSize2, _buffer, _size, compressSize, _compress ) == false )
		{
			LOGGER_ERROR("ArchiveService::compress: invalid compress"
				);

			return nullptr;
		}
        
        void * new_memory = memory->newBuffer( compressSize );

		if( new_memory == nullptr )
		{
			LOGGER_ERROR("ArchiveService::compress: invalid new memory"
				);

			return nullptr;
		}

		return memory;
	}
}
