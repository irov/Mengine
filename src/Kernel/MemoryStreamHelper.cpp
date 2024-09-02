#include "MemoryStreamHelper.h"

#include "Interface/MemoryServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/FileGroupHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryStream( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc )
        {
            size_t stream_size = _stream->size();

            MemoryInterfacePtr memory = Helper::createMemoryStreamSize( _stream, stream_size, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory );

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryStreamSize( const InputStreamInterfacePtr & _stream, size_t _size, const DocumentInterfacePtr & _doc )
        {
            MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
                ->createMemoryBuffer( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory );

            void * memory_buffer = memory->newBuffer( _size );

            MENGINE_ASSERTION_MEMORY_PANIC( memory_buffer );

            size_t read_byte = _stream->read( memory_buffer, _size );

            if( read_byte != _size )
            {
                return nullptr;
            }

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryStreamExtraSize( const InputStreamInterfacePtr & _stream, size_t _extraSize, const DocumentInterfacePtr & _doc )
        {
            size_t stream_size = _stream->size();

            MemoryBufferInterfacePtr memory = Helper::createMemoryBuffer( stream_size + _extraSize, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory );

            void * memory_buffer = memory->getBuffer();

            MENGINE_ASSERTION_MEMORY_PANIC( memory_buffer );

            size_t read_byte = _stream->read( memory_buffer, stream_size );

            if( read_byte != stream_size )
            {
                return nullptr;
            }

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryStreamString( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc )
        {
            size_t stream_size = _stream->size();

            MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
                ->createMemoryBuffer( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory );

            void * memory_buffer = memory->newBuffer( stream_size + 1 );

            MENGINE_ASSERTION_MEMORY_PANIC( memory_buffer );

            size_t read_byte = _stream->read( memory_buffer, stream_size );

            if( read_byte != stream_size )
            {
                return nullptr;
            }

            static_cast<Char *>(memory_buffer)[stream_size] = '\0';

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _stream, bool _share, const DocumentInterfacePtr & _doc )
        {
            InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, _stream, _share, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid create input file stream '%s' stream [%u] (doc: %s)"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                , _stream
                , MENGINE_DOCUMENT_STR( _doc )
            );

            MemoryInterfacePtr memory = Helper::createMemoryStream( stream, _doc );

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryFileString( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _stream, bool _share, const DocumentInterfacePtr & _doc )
        {
            InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, _stream, _share, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid create input file stream '%s' stream [%u] (doc: %s)"
                , Helper::getFileGroupFullPath( _fileGroup, _filePath )
                , _stream
                , MENGINE_DOCUMENT_STR( _doc )
            );

            MemoryInterfacePtr memory = Helper::createMemoryStreamString( stream, _doc );

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryBuffer( size_t _size, const DocumentInterfacePtr & _doc )
        {
            MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
                ->createMemoryBuffer( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory );

            void * memory_buffer = memory->newBuffer( _size );
            MENGINE_UNUSED( memory_buffer );
            
            MENGINE_ASSERTION_MEMORY_PANIC( memory_buffer );

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryCacheBuffer( size_t _size, const DocumentInterfacePtr & _doc )
        {
            MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
                ->createMemoryCacheBuffer( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory );

            void * memory_buffer = memory->newBuffer( _size );
            MENGINE_UNUSED( memory_buffer );
            
            MENGINE_ASSERTION_MEMORY_PANIC( memory_buffer );

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryCacheStreamSize( const InputStreamInterfacePtr & _stream, size_t _size, const DocumentInterfacePtr & _doc )
        {
            MemoryInterfacePtr cache = Helper::createMemoryCacheBuffer( _size, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( cache );

            void * memory = cache->getBuffer();

            size_t read_byte = _stream->read( memory, _size );

            if( read_byte != _size )
            {
                return nullptr;
            }

            return cache;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryCacheStreamExtraSize( const InputStreamInterfacePtr & _stream, size_t _extraSize, const DocumentInterfacePtr & _doc )
        {
            size_t stream_size = _stream->size();

            MemoryBufferInterfacePtr memory = Helper::createMemoryCacheBuffer( stream_size + _extraSize, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory );

            void * memory_buffer = memory->getBuffer();

            MENGINE_ASSERTION_MEMORY_PANIC( memory_buffer );

            size_t read_byte = _stream->read( memory_buffer, stream_size );

            if( read_byte != stream_size )
            {
                return nullptr;
            }

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryCacheStream( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc )
        {
            size_t size = _stream->size();

            MemoryInterfacePtr memory = Helper::createMemoryCacheStreamSize( _stream, size, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid cache size [%zu]", size );

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryCacheFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _stream, bool _share, const DocumentInterfacePtr & _doc )
        {
            InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, _stream, _share, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( stream );

            MemoryInterfacePtr memory = Helper::createMemoryCacheStream( stream, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory );

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryCacheFileString( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _stream, bool _share, const DocumentInterfacePtr & _doc )
        {
            InputStreamInterfacePtr stream = Helper::openInputStreamFile( _fileGroup, _filePath, _stream, _share, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( stream );

            size_t size = stream->size();

            MemoryInterfacePtr memory = Helper::createMemoryCacheBuffer( size + 1, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory );

            uint8_t * memory_buffer = memory->getBuffer();

            MENGINE_ASSERTION_MEMORY_PANIC( memory_buffer );

            size_t read_byte = stream->read( memory_buffer, size );

            if( read_byte != size )
            {
                return nullptr;
            }

            memory_buffer[size] = 0;

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
