#include "ArchiveService.h"

#include "Interface/MemoryServiceInterface.h"

#include "Kernel/MemoryHelper.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

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
    bool ArchiveService::decompressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream, size_t _size, void * _memory, size_t _capacity, size_t & _uncompress )
    {
        MemoryInterfacePtr compress_buffer = Helper::createMemoryCacheStreamSize( _stream, _size, MENGINE_DOCUMENT_FUNCTION );

        if( compress_buffer == nullptr )
        {
            LOGGER_ERROR( "invalid compress buffer %d"
                , _size
            );

            return false;
        }

        const void * compress_memory = compress_buffer->getBuffer();

        size_t uncompressSize = 0;
        if( _archivator->decompress( _memory, _capacity, compress_memory, _size, uncompressSize ) == false )
        {
            LOGGER_ERROR( "invalid decompress"
            );

            return false;
        }

        _uncompress = uncompressSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInputInterfacePtr ArchiveService::compressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream, EArchivatorCompress _compress )
    {
        MemoryInterfacePtr uncompress_buffer = Helper::createMemoryCacheStream( _stream, MENGINE_DOCUMENT_FUNCTION );

        if( uncompress_buffer == nullptr )
        {
            LOGGER_ERROR( "invalid cache buffer"
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
            ->createMemoryInput( MENGINE_DOCUMENT_FUNCTION );

        void * buffer = memory->newBuffer( compressSize2, MENGINE_DOCUMENT_FUNCTION );

        if( buffer == nullptr )
        {
            LOGGER_ERROR( "invalid new memory size '%d'"
                , compressSize2
            );

            return nullptr;
        }

        size_t compressSize;
        if( _archivator->compress( buffer, compressSize2, _buffer, _size, compressSize, _compress ) == false )
        {
            LOGGER_ERROR( "invalid compress"
            );

            return nullptr;
        }

        void * new_memory = memory->newBuffer( compressSize, MENGINE_DOCUMENT_FUNCTION );

        if( new_memory == nullptr )
        {
            LOGGER_ERROR( "invalid new memory '%d'"
                , compressSize
            );

            return nullptr;
        }

        return memory;
    }
}
