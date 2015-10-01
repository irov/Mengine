#	include "MemoryHelper.h"

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		MemoryCacheBufferInterfacePtr createMemoryBuffer( ServiceProviderInterface * _serviceProvider, size_t _size, const char * _doc )
		{
			MemoryCacheBufferInterfacePtr cache = MEMORY_SERVICE( _serviceProvider )
				->createMemoryCacheBuffer();

			if( cache == nullptr )
			{
				return nullptr;
			}

			if( cache->cacheMemory( _size, _doc ) == false )
			{
				return nullptr;
			}

			return cache;
		}
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferInterfacePtr createMemoryStreamSize( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, size_t _size, const char * _doc )
		{
			MemoryCacheBufferInterfacePtr cache = Helper::createMemoryBuffer( _serviceProvider, _size, _doc );

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
		MemoryCacheBufferInterfacePtr createMemoryStream( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const char * _doc )
		{
			size_t size = _stream->size();

			MemoryCacheBufferInterfacePtr cache = Helper::createMemoryStreamSize( _serviceProvider, _stream, size, _doc );

			if( cache == nullptr )
			{
				return nullptr;
			}

			return cache;
		}
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferInterfacePtr createMemoryFile( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _filePath, bool _stream, const char * _doc )
		{
			InputStreamInterfacePtr stream = FILE_SERVICE( _serviceProvider )
				->openInputFile( _category, _filePath, _stream );

			if( stream == nullptr )
			{
				return nullptr;
			}

			MemoryCacheBufferInterfacePtr cache = Helper::createMemoryStream( _serviceProvider, stream, _doc );

			if( cache == nullptr )
			{
				return nullptr;
			}

			return cache;
		}
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheBufferInterfacePtr createMemoryFileString( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _filePath, bool _stream, const char * _doc )
		{
			InputStreamInterfacePtr stream = FILE_SERVICE( _serviceProvider )
				->openInputFile( _category, _filePath, _stream );

			if( stream == nullptr )
			{
				return nullptr;
			}

			size_t size = stream->size();

			MemoryCacheBufferInterfacePtr cache = Helper::createMemoryBuffer( _serviceProvider, size + 1, _doc );

			if( cache == nullptr )
			{
				return nullptr;
			}

			unsigned char * memory = cache->getMemoryT<unsigned char *>();

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
