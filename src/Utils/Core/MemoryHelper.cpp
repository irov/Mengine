#	include "MemoryHelper.h"

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		MemoryInterfacePtr createMemoryStream( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream )
		{
			size_t stream_size = _stream->size();

			MemoryInterfacePtr memoryBuffer = Helper::createMemoryStreamSize( _serviceProvider, _stream, stream_size );

			if( memoryBuffer == nullptr )
			{
				return nullptr;
			}

			return memoryBuffer;
		}
		//////////////////////////////////////////////////////////////////////////
		MemoryInterfacePtr createMemoryStreamSize( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, size_t _size )
		{
			MemoryInterfacePtr memoryBuffer = MEMORY_SERVICE( _serviceProvider )
				->createMemory();

			if( memoryBuffer == nullptr )
			{
				return nullptr;
			}

			void * memory = memoryBuffer->newMemory( _size );

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
		MemoryInterfacePtr createMemoryFile( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _filePath, bool _stream )
		{
			InputStreamInterfacePtr fileStream = FILE_SERVICE( _serviceProvider )
				->openInputFile( _category, _filePath, _stream );

			if( fileStream == nullptr )
			{
				return nullptr;
			}

			MemoryInterfacePtr memory = Helper::createMemoryStream( _serviceProvider, fileStream );

			return memory;
		}
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferInterfacePtr createMemoryCacheBuffer( ServiceProviderInterface * _serviceProvider, size_t _size, const char * _doc )
		{
			MemoryCacheBufferInterfacePtr cache = MEMORY_SERVICE( _serviceProvider )
				->createMemoryCacheBuffer();

			if( cache == nullptr )
			{
				return nullptr;
			}

			void * memory = cache->cacheMemory( _size, _doc );

			if( memory == nullptr )
			{
				return nullptr;
			}

			return cache;
		}
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferInterfacePtr createMemoryCacheStreamSize( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, size_t _size, const char * _doc )
		{
			MemoryCacheBufferInterfacePtr cache = Helper::createMemoryCacheBuffer( _serviceProvider, _size, _doc );

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
		MemoryCacheBufferInterfacePtr createMemoryCacheStream( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const char * _doc )
		{
			size_t size = _stream->size();

			MemoryCacheBufferInterfacePtr cache = Helper::createMemoryCacheStreamSize( _serviceProvider, _stream, size, _doc );

			if( cache == nullptr )
			{
				return nullptr;
			}

			return cache;
		}
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferInterfacePtr createMemoryCacheFile( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _filePath, bool _stream, const char * _doc )
		{
			InputStreamInterfacePtr stream = FILE_SERVICE( _serviceProvider )
				->openInputFile( _category, _filePath, _stream );

			if( stream == nullptr )
			{
				return nullptr;
			}

			MemoryCacheBufferInterfacePtr cache = Helper::createMemoryCacheStream( _serviceProvider, stream, _doc );

			if( cache == nullptr )
			{
				return nullptr;
			}

			return cache;
		}
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferInterfacePtr createMemoryCacheFileString( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _filePath, bool _stream, const char * _doc )
		{
			InputStreamInterfacePtr stream = FILE_SERVICE( _serviceProvider )
				->openInputFile( _category, _filePath, _stream );

			if( stream == nullptr )
			{
				return nullptr;
			}

			size_t size = stream->size();

			MemoryCacheBufferInterfacePtr cache = Helper::createMemoryCacheBuffer( _serviceProvider, size + 1, _doc );

			if( cache == nullptr )
			{
				return nullptr;
			}

			unsigned char * memory = cache->getMemory();

			size_t read_byte = stream->read( memory, size );

			if( read_byte != size )
			{
				return nullptr;
			}

			memory[size] = 0;

			return cache;
		}
	}
}	// namespace Menge
