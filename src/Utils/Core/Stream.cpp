#	include "Stream.h"

#	include "Core/MemoryCacheBuffer.h"

#	include "Core/CRC32.h"

#	include "Logger/Logger.h"

namespace Menge
{
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		bool loadStreamMagicHeader( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, magic_number_type _magic, magic_version_type _version )
		{
			magic_number_type magic_number;
			_stream->read( &magic_number, sizeof(magic_number) );

			if( magic_number != _magic )
			{
				LOGGER_ERROR(_serviceProvider)("loadStreamMagicHeader: invalid magic number %u need %u"
					, magic_number
					, _magic
					);

				return false;
			}

			magic_version_type magic_version;
			_stream->read( &magic_version, sizeof(magic_version) );

			if( magic_version != _version )
			{
				LOGGER_ERROR(_serviceProvider)("loadStreamMagicHeader: invalid magic version %u need %u"
					, magic_version
					, _version
					);

				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool writeStreamMagicHeader( ServiceProviderInterface * _serviceProvider, const OutputStreamInterfacePtr & _stream, magic_number_type _magic, magic_version_type _version )
		{
			if( _stream->write( &_magic, sizeof(_magic) ) == false )
			{
				LOGGER_ERROR(_serviceProvider)("writeStreamMagicHeader: invalid write 'magic header'"
					);

				return false;
			}

			if( _stream->write( &_version, sizeof(_version) ) == false )
			{
				LOGGER_ERROR(_serviceProvider)("writeStreamMagicHeader: invalid write 'magic version'"
					);

				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool loadStreamArchiveBuffer( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, CacheBufferID & _bufferId, unsigned char ** _data, size_t & _size )
		{
			uint32_t crc32;
			_stream->read( &crc32, sizeof(crc32) );

			uint32_t load_binary_size;
			_stream->read( &load_binary_size, sizeof(load_binary_size) );

			uint32_t load_compress_size;
			_stream->read( &load_compress_size, sizeof(load_compress_size) );

			size_t binary_size = (size_t)load_binary_size;
			size_t compress_size = (size_t)load_compress_size;

			MemoryCacheBufferInterfacePtr compress_buffer = Helper::createMemoryBuffer( _serviceProvider, compress_size, "loadStreamArchiveBuffer" );

			if( compress_buffer == nullptr )
			{
				LOGGER_ERROR( _serviceProvider )("loadStreamArchiveBuffer: invalid get memory %d (compress)"
					, compress_size
					);

				return false;
			}

			void * compress_memory = compress_buffer->getMemory();

			size_t read_data = _stream->read( compress_memory, compress_size );

			if( read_data != (size_t)compress_size )
			{
				LOGGER_ERROR(_serviceProvider)("loadStreamArchiveBuffer: invalid read data %d need %d"
					, read_data
					, compress_size
					);

				return false;
			}

			if( crc32 != 0 )
			{
				uint32_t check_crc32 = Helper::make_crc32( compress_memory, compress_size );

				if( check_crc32 != crc32 )
				{
					LOGGER_ERROR(_serviceProvider)("loadStreamArchiveBuffer: invalid crc32 %d need %d"
						, check_crc32
						, crc32
						);

					return false;
				}
			}

			void * binaryMemory = nullptr;
			CacheBufferID binaryBufferId = MEMORY_SERVICE(_serviceProvider)
				->lockBuffer( binary_size, &binaryMemory, "loadStreamArchiveBuffer binary_memory" );

			if( binaryBufferId == INVALID_CACHE_BUFFER_ID )
			{
				LOGGER_ERROR(_serviceProvider)("loadStreamArchiveBuffer: invalid get memory %d (binary)"
					, binary_size
					);

				return false;
			}

			size_t uncompressSize = 0;
			if( _archivator->decompress( binaryMemory, binary_size, compress_memory, compress_size, uncompressSize ) == false )
			{
				LOGGER_ERROR(_serviceProvider)("loadStreamArchiveBuffer: invalid decompress"
					);

				MEMORY_SERVICE(_serviceProvider)
					->unlockBuffer( binaryBufferId );

				return false;
			}

			if( uncompressSize != binary_size )
			{
				LOGGER_ERROR(_serviceProvider)("loadStreamArchiveBuffer: invalid decompress size %d need %d"
					, uncompressSize
					, binary_size
					);

				MEMORY_SERVICE(_serviceProvider)
					->unlockBuffer( binaryBufferId );

				return false;
			}

			_bufferId = binaryBufferId;
			*_data = static_cast<unsigned char *>(binaryMemory);
			_size = binary_size;

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool loadStreamArchiveInplace( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, void * _data, size_t _size )
		{
			uint32_t crc32;
			_stream->read( &crc32, sizeof(crc32) );

			uint32_t load_binary_size;
			_stream->read( &load_binary_size, sizeof(load_binary_size) );

			uint32_t load_compress_size;
			_stream->read( &load_compress_size, sizeof(load_compress_size) );

			size_t binary_size = (size_t)load_binary_size;
			size_t compress_size = (size_t)load_compress_size;

			if( binary_size != _size )
			{
				LOGGER_ERROR(_serviceProvider)("loadStreamArchiveInplace: invalid buffer size %d need %d"
					, _size
					, binary_size
					);

				return false;
			}

			MemoryCacheBufferInterfacePtr compress_buffer = Helper::createMemoryBuffer( _serviceProvider, compress_size, "ArchiveService::getData compress_memory" );

			if( compress_buffer == nullptr )
			{
				LOGGER_ERROR( _serviceProvider )("loadStreamArchiveInplace: invalid get memory %d (compress)"
					, compress_size
					);

				return false;
			}

			void * compress_memory = compress_buffer->getMemory();

			size_t read_data = _stream->read( compress_memory, compress_size );

			if( read_data != (size_t)compress_size )
			{
				LOGGER_ERROR(_serviceProvider)("loadStreamArchiveInplace: invalid read data %d need %d"
					, read_data
					, compress_size
					);

				return false;
			}

			if( crc32 != 0 )
			{
				uint32_t check_crc32 = Helper::make_crc32( compress_memory, compress_size );

				if( check_crc32 != crc32 )
				{
					LOGGER_ERROR(_serviceProvider)("loadStreamArchiveInplace: invalid crc32 %d need %d"
						, check_crc32
						, crc32
						);

					return false;
				}
			}

			size_t uncompressSize = 0;
			if( _archivator->decompress( _data, _size, compress_memory, compress_size, uncompressSize ) == false )
			{
				LOGGER_ERROR(_serviceProvider)("loadStreamArchiveInplace: invalid decompress"
					);

				return false;
			}

			if( uncompressSize != _size )
			{
				LOGGER_ERROR(_serviceProvider)("loadStreamArchiveInplace: invalid decompress size %d need %d"
					, uncompressSize
					, _size
					);

				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool loadStreamArchiveBufferSize( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, size_t & _size )
		{
			size_t pos = _stream->tell();

			uint32_t crc32;
			if( _stream->read( &crc32, sizeof(crc32) ) != sizeof(crc32) )
			{
				LOGGER_ERROR(_serviceProvider)("loadStreamArchiveBufferSize: invalid read crc32"
					);

				return false;
			}

			uint32_t load_binary_size;
			if( _stream->read( &load_binary_size, sizeof(load_binary_size) ) != sizeof(load_binary_size) )
			{
				LOGGER_ERROR(_serviceProvider)("loadStreamArchiveBufferSize: invalid read binary size"
					);


				return false;
			}

			if( _stream->seek( pos ) == false )
			{
				LOGGER_ERROR(_serviceProvider)("loadStreamArchiveBufferSize: invalid rewind"
					);


				return false;
			}

			_size = (size_t)load_binary_size;

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool writeStreamArchiveBuffer( ServiceProviderInterface * _serviceProvider, const OutputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, bool _crc32, const void * _data, size_t _size )
		{
			MemoryInputInterfacePtr compress_memory = ARCHIVE_SERVICE( _serviceProvider )
				->compressBuffer( _archivator, _data, _size );

			if( compress_memory == nullptr )
			{
				LOGGER_ERROR(_serviceProvider)("writeStreamArchiveBuffer: invalid compress"
					);

				return false;
			}

			size_t compressSize;
			const void * compressBuffer = compress_memory->getMemory( compressSize );

			if( compressBuffer == nullptr )
			{
				LOGGER_ERROR(_serviceProvider)("writeStreamArchiveBuffer: invalid get memory"
					);

				return false;
			}

			uint32_t value_crc32 = _crc32 == true ? Helper::make_crc32( compressBuffer, compressSize ) : 0;

			if( _stream->write( &value_crc32, sizeof(value_crc32) ) == false )
			{
				LOGGER_ERROR(_serviceProvider)("writeStreamArchiveBuffer: invalid write 'crc32'"
					);

				return false;
			}

			uint32_t write_uncompressSize = (uint32_t)_size;
			if( _stream->write( &write_uncompressSize, sizeof(write_uncompressSize) ) == false )
			{
				LOGGER_ERROR(_serviceProvider)("writeStreamArchiveBuffer: invalid write 'uncompress size'"
					);

				return false;
			}

			uint32_t write_compressSize = (uint32_t)compressSize;
			if( _stream->write( &write_compressSize, sizeof(write_compressSize) ) == false )
			{
				LOGGER_ERROR(_serviceProvider)("writeStreamArchiveBuffer: invalid write 'compress size'"
					);

				return false;
			}

			if( _stream->write( compressBuffer, compressSize ) == false )
			{
				LOGGER_ERROR(_serviceProvider)("writeStreamArchiveBuffer: invalid write 'buffer %d'"
					, compressSize
					);

				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool loadStreamArchiveData( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, CacheBufferID & _bufferId, unsigned char ** _data, size_t & _size )
		{
			if( Helper::loadStreamMagicHeader( _serviceProvider, _stream, _magic, _version ) == false )
			{
				return false;
			}

			if( Helper::loadStreamArchiveBuffer( _serviceProvider, _stream, _archivator, _bufferId, _data, _size ) == false )
			{
				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool writeStreamArchiveData( ServiceProviderInterface * _serviceProvider, const OutputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, bool _crc32, const void * _data, size_t _size )
		{
			if( Helper::writeStreamMagicHeader( _serviceProvider, _stream, _magic, _version ) == false )
			{
				return false;
			}

			if( Helper::writeStreamArchiveBuffer( _serviceProvider, _stream, _archivator, _crc32, _data, _size ) == false )
			{
				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		MemoryInterfacePtr loadStreamArchiveMemory( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator )
		{
			uint32_t crc32;
			_stream->read( &crc32, sizeof( crc32 ) );

			uint32_t load_binary_size;
			_stream->read( &load_binary_size, sizeof( load_binary_size ) );

			uint32_t load_compress_size;
			_stream->read( &load_compress_size, sizeof( load_compress_size ) );

			size_t binary_size = (size_t)load_binary_size;
			size_t compress_size = (size_t)load_compress_size;

			MemoryCacheBufferInterfacePtr compress_buffer = Helper::createMemoryBuffer( _serviceProvider, compress_size, "loadStreamArchiveMemory compress" );

			if( compress_buffer == nullptr )
			{
				LOGGER_ERROR( _serviceProvider )("loadStreamArchiveBuffer: invalid get memory %d (compress)"
					, compress_size
					);

				return nullptr;
			}

			void * compress_memory = compress_buffer->getMemory();

			size_t read_data = _stream->read( compress_memory, compress_size );

			if( read_data != (size_t)compress_size )
			{
				LOGGER_ERROR( _serviceProvider )("loadStreamArchiveBuffer: invalid read data %d need %d"
					, read_data
					, compress_size
					);

				return nullptr;
			}

			if( crc32 != 0 )
			{
				uint32_t check_crc32 = Helper::make_crc32( compress_memory, compress_size );

				if( check_crc32 != crc32 )
				{
					LOGGER_ERROR( _serviceProvider )("loadStreamArchiveBuffer: invalid crc32 %d need %d"
						, check_crc32
						, crc32
						);

					return nullptr;
				}
			}

			MemoryInterfacePtr binary_buffer = MEMORY_SERVICE( _serviceProvider )
				->createMemory();

			if( binary_buffer == nullptr )
			{
				LOGGER_ERROR( _serviceProvider )("loadStreamArchiveBuffer: invalid get memory %d (binary)"
					, binary_size
					);

				return nullptr;
			}

			void * binary_memory = binary_buffer->newMemory( binary_size );

			size_t uncompressSize = 0;
			if( _archivator->decompress( binary_memory, binary_size, compress_memory, compress_size, uncompressSize ) == false )
			{
				LOGGER_ERROR( _serviceProvider )("loadStreamArchiveBuffer: invalid decompress"
					);

				return nullptr;
			}

			if( uncompressSize != binary_size )
			{
				LOGGER_ERROR( _serviceProvider )("loadStreamArchiveBuffer: invalid decompress size %d need %d"
					, uncompressSize
					, binary_size
					);

				return nullptr;
			}

			return binary_buffer;
		}
		//////////////////////////////////////////////////////////////////////////
		MemoryInterfacePtr loadStreamArchiveMemory( ServiceProviderInterface * _serviceProvider, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version )
		{
			if( Helper::loadStreamMagicHeader( _serviceProvider, _stream, _magic, _version ) == false )
			{
				return nullptr;
			}

			MemoryInterfacePtr memory = Helper::loadStreamArchiveMemory( _serviceProvider, _stream, _archivator );

			return memory;
		}
	}
}