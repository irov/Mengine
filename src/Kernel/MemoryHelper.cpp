#include "MemoryHelper.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"

#include "Kernel/Document.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryStream( const InputStreamInterfacePtr & _stream, const Char * _doc, const Char * _file, uint32_t _line )
        {
            size_t stream_size = _stream->size();

            MemoryInterfacePtr memory = Helper::createMemoryStreamSize( _stream, stream_size, _doc, _file, _line );

            MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryStreamSize( const InputStreamInterfacePtr & _stream, size_t _size, const Char * _doc, const Char * _file, uint32_t _line )
        {
            MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
                ->createMemoryBuffer( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

            void * memory_buffer = memory->newBuffer( _size, _doc, _file, _line );

            MENGINE_ASSERTION_MEMORY_PANIC( memory_buffer, nullptr );

            size_t read_byte = _stream->read( memory_buffer, _size );

            if( read_byte != _size )
            {
                return nullptr;
            }

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryStreamExtraSize( const InputStreamInterfacePtr & _stream, size_t _extraSize, const Char * _doc, const Char * _file, uint32_t _line )
        {
            size_t stream_size = _stream->size();

            MemoryBufferInterfacePtr memory = Helper::createMemoryBuffer( stream_size + _extraSize, _doc, _file, _line );

            MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

            void * memory_buffer = memory->getBuffer();

            MENGINE_ASSERTION_MEMORY_PANIC( memory_buffer, nullptr );

            size_t read_byte = _stream->read( memory_buffer, stream_size );

            if( read_byte != stream_size )
            {
                return nullptr;
            }

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _stream, const Char * _doc, const Char * _file, uint32_t _line )
        {
            InputStreamInterfacePtr stream = FILE_SERVICE()
                ->openInputFile( _fileGroup, _filePath, _stream, MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, nullptr );

            MemoryInterfacePtr memory = Helper::createMemoryStream( stream, _doc, _file, _line );

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryBuffer( size_t _size, const Char * _doc, const Char * _file, uint32_t _line )
        {
            MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
                ->createMemoryBuffer( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

            void * memory_buffer = memory->newBuffer( _size, _doc, _file, _line );

            MENGINE_UNUSED( memory_buffer );
            MENGINE_ASSERTION_MEMORY_PANIC( memory_buffer, nullptr );

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryCacheBuffer( size_t _size, const Char * _doc, const Char * _file, uint32_t _line )
        {
            MemoryBufferInterfacePtr memory = MEMORY_SERVICE()
                ->createMemoryCacheBuffer( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

            void * memory_buffer = memory->newBuffer( _size, _doc, _file, _line );

            MENGINE_UNUSED( memory_buffer );
            MENGINE_ASSERTION_MEMORY_PANIC( memory_buffer, nullptr );

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryCacheStreamSize( const InputStreamInterfacePtr & _stream, size_t _size, const Char * _doc, const Char * _file, uint32_t _line )
        {
            MemoryInterfacePtr cache = Helper::createMemoryCacheBuffer( _size, _doc, _file, _line );

            if( cache == nullptr )
            {
                return nullptr;
            }

            void * memory = cache->getBuffer();

            size_t read_byte = _stream->read( memory, _size );

            if( read_byte != _size )
            {
                return nullptr;
            }

            return cache;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryCacheStreamExtraSize( const InputStreamInterfacePtr & _stream, size_t _extraSize, const Char * _doc, const Char * _file, uint32_t _line )
        {
            size_t stream_size = _stream->size();

            MemoryBufferInterfacePtr memory = Helper::createMemoryCacheBuffer( stream_size + _extraSize, _doc, _file, _line );

            MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

            void * memory_buffer = memory->getBuffer();

            MENGINE_ASSERTION_MEMORY_PANIC( memory_buffer, nullptr );

            size_t read_byte = _stream->read( memory_buffer, stream_size );

            if( read_byte != stream_size )
            {
                return nullptr;
            }

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryCacheStream( const InputStreamInterfacePtr & _stream, const Char * _doc, const Char * _file, uint32_t _line )
        {
            size_t size = _stream->size();

            MemoryInterfacePtr memory = Helper::createMemoryCacheStreamSize( _stream, size, _doc, _file, _line );

            MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryCacheFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _stream, const Char * _doc, const Char * _file, uint32_t _line )
        {
            InputStreamInterfacePtr stream = FILE_SERVICE()
                ->openInputFile( _fileGroup, _filePath, _stream, MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, nullptr );

            MemoryInterfacePtr memory = Helper::createMemoryCacheStream( stream, _doc, _file, _line );

            MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryCacheFileString( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, bool _stream, const Char * _doc, const Char * _file, uint32_t _line )
        {
            InputStreamInterfacePtr stream = FILE_SERVICE()
                ->openInputFile( _fileGroup, _filePath, _stream, MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( stream, nullptr );

            size_t size = stream->size();

            MemoryInterfacePtr memory = Helper::createMemoryCacheBuffer( size + 1, _doc, _file, _line );

            MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

            uint8_t * memory_buffer = memory->getBuffer();

            MENGINE_ASSERTION_MEMORY_PANIC( memory_buffer, nullptr );

            size_t read_byte = stream->read( memory_buffer, size );

            if( read_byte != size )
            {
                return nullptr;
            }

            memory_buffer[size] = 0;

            return memory;
        }
    }
}
