#include "ArchiveService.h"

#include "Interface/MemoryServiceInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/WriteHelper.h"

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
    MemoryInputInterfacePtr ArchiveService::compressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream, EArchivatorCompress _compress )
    {
        MemoryInterfacePtr uncompress_buffer = Helper::createMemoryCacheStream( _stream, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( uncompress_buffer, "invalid cache buffer" );

        const void * uncompress_memory = uncompress_buffer->getBuffer();
        size_t uncompress_size = uncompress_buffer->getSize();

        MemoryInputInterfacePtr compress_memory = this->compressBuffer( _archivator, uncompress_memory, uncompress_size, _compress );

        return compress_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ArchiveService::decompressStream( const ArchivatorInterfacePtr & _archivator, const InputStreamInterfacePtr & _stream, size_t _size, void * const _memory, size_t _capacity, size_t * const _decompressSize )
    {
        MemoryInterfacePtr compress_buffer = Helper::createMemoryCacheStreamSize( _stream, _size, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( compress_buffer, "invalid compress buffer %zu"
            , _size
        );

        const void * compress_memory = compress_buffer->getBuffer();

        size_t uncompressSize = 0;
        if( _archivator->decompress( _memory, _capacity, compress_memory, _size, &uncompressSize ) == false )
        {
            return false;
        }

        *_decompressSize = uncompressSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInputInterfacePtr ArchiveService::compressBuffer( const ArchivatorInterfacePtr & _archivator, const void * _buffer, size_t _size, EArchivatorCompress _compress )
    {
        size_t compressSize2 = _archivator->compressBound( _size );

        MemoryInputInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryInput( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory" );

        void * memory_buffer = memory->newBuffer( compressSize2 );

        MENGINE_ASSERTION_MEMORY_PANIC( memory_buffer, "invalid new memory size '%zu'"
            , compressSize2
        );

        size_t compressSize;
        if( _archivator->compress( memory_buffer, compressSize2, _buffer, _size, &compressSize, _compress ) == false )
        {
            return nullptr;
        }

        void * new_memory = memory->newBuffer( compressSize );
        MENGINE_UNUSED( new_memory );

        MENGINE_ASSERTION_MEMORY_PANIC( new_memory, "invalid new memory '%zu'"
            , compressSize
        );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
}
