#	include "CacheMemoryStream.h"

#	include "Interface/CacheInterface.h"
#	include "Interface/FileSystemInterface.h"

namespace Menge
{
	namespace Helper
	{
		MemoryCacheInputPtr createMemoryStream( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, size_t _size, const char * _doc )
		{
			MemoryCacheInputPtr cache = CACHE_SERVICE( _serviceProvider )
				->createMemoryCacheInput();

			if( cache == nullptr )
			{
				return nullptr;
			}

			if( _size == UNKNOWN_SIZE )
			{
				_size = _stream->size();
			}

			void * memory = cache->cacheMemory( _size, _doc );

			if( memory == nullptr )
			{
				return nullptr;
			}

			size_t read_byte = _stream->read( memory, _size );

			if( read_byte != _size )
			{
				return nullptr;
			}

			return cache;
		}
		//////////////////////////////////////////////////////////////////////////
		MemoryCacheInputPtr createMemoryFile( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const FilePath & _filePath, bool _stream, const char * _doc )
		{
			InputStreamInterfacePtr stream = FILE_SERVICE( _serviceProvider )
				->openInputFile( _category, _filePath, _stream );

			if( stream == nullptr )
			{
				return nullptr;
			}
			
			MemoryCacheInputPtr cache = createMemoryStream( _serviceProvider, stream, UNKNOWN_SIZE, _doc );

			if( cache == nullptr )
			{
				return nullptr;
			}

			return cache;
		}
	}
}