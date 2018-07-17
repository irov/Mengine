#include "MemoryHelper.h"

#include "Interface/FileSystemInterface.h"

namespace Mengine
{
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		MemoryInterfacePtr createMemoryStream( const InputStreamInterfacePtr & _stream, const Char * _file, uint32_t _line )
		{
			size_t stream_size = _stream->size();

			MemoryInterfacePtr memoryBuffer = Helper::createMemoryStreamSize( _stream, stream_size, _file, _line );

			if( memoryBuffer == nullptr )
			{
				return nullptr;
			}

			return memoryBuffer;
		}
		//////////////////////////////////////////////////////////////////////////
		MemoryInterfacePtr createMemoryStreamSize( const InputStreamInterfacePtr & _stream, size_t _size, const Char * _file, uint32_t _line )
		{
			MemoryBufferInterfacePtr memoryBuffer = MEMORY_SERVICE()
				->createMemoryBuffer();

			if( memoryBuffer == nullptr )
			{
				return nullptr;
			}

			void * memory = memoryBuffer->newMemory( _size, _file, _line );

			if( memory == nullptr )
			{
				return nullptr;
			}

			size_t read_byte = _stream->read( memory, _size );

			if( read_byte != _size )
			{
				return nullptr;
			}

			return memoryBuffer;
		}
		//////////////////////////////////////////////////////////////////////////
		MemoryInterfacePtr createMemoryFile( const FileGroupInterfacePtr & _category, const FilePath & _filePath, bool _stream, const Char * _file, uint32_t _line )
		{
			InputStreamInterfacePtr fileStream = FILE_SERVICE()
				->openInputFile( _category, _filePath, _stream );

			if( fileStream == nullptr )
			{
				return nullptr;
			}

            MemoryInterfacePtr memory = Helper::createMemoryStream( fileStream, _file, _line );

			return memory;
		}
		//////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryCacheBuffer( size_t _size, const Char * _file, uint32_t _line )
		{
            MemoryBufferInterfacePtr cache = MEMORY_SERVICE()
				->createMemoryCacheBuffer();

			if( cache == nullptr )
			{
				return nullptr;
			}

			void * memory = cache->newMemory( _size, _file, _line );

			if( memory == nullptr )
			{
				return nullptr;
			}

			return cache;
		}
		//////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryCacheStreamSize( const InputStreamInterfacePtr & _stream, size_t _size, const Char * _file, uint32_t _line )
		{
            MemoryInterfacePtr cache = Helper::createMemoryCacheBuffer( _size, _file, _line );

			if( cache == nullptr )
			{
				return nullptr;
			}

			void * memory = cache->getMemory();

			size_t read_byte = _stream->read( memory, _size );

			if( read_byte != _size )
			{
				return nullptr;
			}

			return cache;
		}
		//////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryCacheStream( const InputStreamInterfacePtr & _stream, const Char * _file, uint32_t _line )
		{
			size_t size = _stream->size();

            MemoryInterfacePtr cache = Helper::createMemoryCacheStreamSize( _stream, size, _file, _line );

			if( cache == nullptr )
			{
				return nullptr;
			}

			return cache;
		}
		//////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryCacheFile( const FileGroupInterfacePtr & _category, const FilePath & _filePath, bool _stream, const Char * _file, uint32_t _line )
		{
			InputStreamInterfacePtr stream = FILE_SERVICE()
				->openInputFile( _category, _filePath, _stream );

			if( stream == nullptr )
			{
				return nullptr;
			}

            MemoryInterfacePtr cache = Helper::createMemoryCacheStream( stream, _file, _line );

			if( cache == nullptr )
			{
				return nullptr;
			}

			return cache;
		}
		//////////////////////////////////////////////////////////////////////////
        MemoryInterfacePtr createMemoryCacheFileString( const FileGroupInterfacePtr & _category, const FilePath & _filePath, bool _stream, const Char * _file, uint32_t _line )
		{
			InputStreamInterfacePtr stream = FILE_SERVICE()
				->openInputFile( _category, _filePath, _stream );

			if( stream == nullptr )
			{
				return nullptr;
			}

			size_t size = stream->size();

            MemoryInterfacePtr cache = Helper::createMemoryCacheBuffer( size + 1, _file, _line );

			if( cache == nullptr )
			{
				return nullptr;
			}

            uint8_t * memory = cache->getMemory();

			size_t read_byte = stream->read( memory, size );

			if( read_byte != size )
			{
				return nullptr;
			}

			memory[size] = 0;

			return cache;
		}
	}
}	
