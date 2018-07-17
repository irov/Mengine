#include "Stream.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/CRC32.h"

#include "Kernel/Logger.h"

namespace Mengine
{
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		bool loadStreamMagicHeader( const InputStreamInterfacePtr & _stream, magic_number_type _magic, magic_version_type _version )
		{
			magic_number_type magic_number;
			_stream->read( &magic_number, sizeof(magic_number) );

			if( magic_number != _magic )
			{
                LOGGER_ERROR( "loadStreamMagicHeader: invalid magic number %u need %u"
					, magic_number
					, _magic
					);

				return false;
			}

			magic_version_type magic_version;
			_stream->read( &magic_version, sizeof(magic_version) );

			if( magic_version != _version )
			{
                LOGGER_ERROR( "loadStreamMagicHeader: invalid magic version %u need %u"
					, magic_version
					, _version
					);

				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool writeStreamMagicHeader( const OutputStreamInterfacePtr & _stream, magic_number_type _magic, magic_version_type _version )
		{
			if( _stream->write( &_magic, sizeof(_magic) ) == false )
			{
                LOGGER_ERROR( "writeStreamMagicHeader: invalid write 'magic header'"
					);

				return false;
			}

			if( _stream->write( &_version, sizeof(_version) ) == false )
			{
                LOGGER_ERROR( "writeStreamMagicHeader: invalid write 'magic version'"
					);

				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool loadStreamArchiveBuffer( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, MemoryInterfacePtr & _binaryBuffer, const char * _file, uint32_t _line )
		{
			uint32_t crc32;
			_stream->read( &crc32, sizeof(crc32) );

			uint32_t load_binary_size;
			_stream->read( &load_binary_size, sizeof(load_binary_size) );

			uint32_t load_compress_size;
			_stream->read( &load_compress_size, sizeof(load_compress_size) );

            uint32_t binary_size = load_binary_size;
            uint32_t compress_size = load_compress_size;

            MemoryInterfacePtr compress_buffer = Helper::createMemoryCacheBuffer( compress_size, _file, _line );

			if( compress_buffer == nullptr )
			{
				LOGGER_ERROR( "loadStreamArchiveBuffer: invalid get memory %d (compress)"
					, compress_size
					);

				return false;
			}

			void * compress_memory = compress_buffer->getMemory();

			size_t read_data = _stream->read( compress_memory, compress_size );

			if( read_data != (size_t)compress_size )
			{
                LOGGER_ERROR( "loadStreamArchiveBuffer: invalid read data %d need %d"
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
                    LOGGER_ERROR( "loadStreamArchiveBuffer: invalid crc32 %d need %d"
						, check_crc32
						, crc32
						);

					return false;
				}
			}

            MemoryBufferInterfacePtr binaryBuffer = MEMORY_SERVICE()
				->createMemoryCacheBuffer();

			if( binaryBuffer == nullptr )
			{
				LOGGER_ERROR("loadStreamArchiveBuffer: invalid create memory cache"
					, binary_size
					);

				return false;
			}

			void * binaryMemory = binaryBuffer->newMemory( binary_size, __FILE__, __LINE__ );

			if( binaryMemory == nullptr )
			{
				LOGGER_ERROR( "loadStreamArchiveBuffer: invalid get memory %d (binary)"
					, binary_size
					);

				return false;
			}

			size_t uncompressSize = 0;
			if( _archivator->decompress( binaryMemory, binary_size, compress_memory, compress_size, uncompressSize ) == false )
			{
                LOGGER_ERROR( "loadStreamArchiveBuffer: invalid decompress"
					);

				return false;
			}

			if( uncompressSize != binary_size )
			{
				LOGGER_ERROR("loadStreamArchiveBuffer: invalid decompress size %d need %d"
					, uncompressSize
					, binary_size
					);

				return false;
			}

			_binaryBuffer = binaryBuffer;

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool loadStreamArchiveInplace( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, void * _data, size_t _size, const char * _file, uint32_t _line )
		{
			uint32_t crc32;
			_stream->read( &crc32, sizeof(crc32) );

			uint32_t load_binary_size;
			_stream->read( &load_binary_size, sizeof(load_binary_size) );

			uint32_t load_compress_size;
			_stream->read( &load_compress_size, sizeof(load_compress_size) );

            uint32_t binary_size = load_binary_size;
            uint32_t compress_size = load_compress_size;

			if( binary_size != _size )
			{
				LOGGER_ERROR("loadStreamArchiveInplace: invalid buffer size %u need %u"
					, _size
					, binary_size
					);

				return false;
			}

			MemoryInterfacePtr compress_buffer = Helper::createMemoryCacheStreamSize( _stream, compress_size, _file, _line );
			
			if( compress_buffer == nullptr )
			{
				LOGGER_ERROR( "loadStreamArchiveInplace: invalid get memory %u (compress)"
					, compress_size
					);

				return false;
			}

			void * compress_memory = compress_buffer->getMemory();

			if( crc32 != 0 )
			{
				uint32_t check_crc32 = Helper::make_crc32( compress_memory, compress_size );

				if( check_crc32 != crc32 )
				{
					LOGGER_ERROR("loadStreamArchiveInplace: invalid crc32 %u need %u"
						, check_crc32
						, crc32
						);

					return false;
				}
			}

			size_t uncompressSize = 0;
			if( _archivator->decompress( _data, _size, compress_memory, compress_size, uncompressSize ) == false )
			{
				LOGGER_ERROR("loadStreamArchiveInplace: invalid decompress"
					);

				return false;
			}

			if( uncompressSize != _size )
			{
				LOGGER_ERROR("loadStreamArchiveInplace: invalid decompress size %u need %u"
					, uncompressSize
					, _size
					);

				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool loadStreamArchiveBufferSize( const InputStreamInterfacePtr & _stream, size_t & _size )
		{
			size_t pos = _stream->tell();

			uint32_t crc32;
			if( _stream->read( &crc32, sizeof(crc32) ) != sizeof(crc32) )
			{
				LOGGER_ERROR("loadStreamArchiveBufferSize: invalid read crc32"
					);

				return false;
			}

			uint32_t load_binary_size;
			if( _stream->read( &load_binary_size, sizeof(load_binary_size) ) != sizeof(load_binary_size) )
			{
				LOGGER_ERROR("loadStreamArchiveBufferSize: invalid read binary size"
					);


				return false;
			}

			if( _stream->seek( pos ) == false )
			{
				LOGGER_ERROR("loadStreamArchiveBufferSize: invalid rewind"
					);


				return false;
			}

			_size = (size_t)load_binary_size;

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool writeStreamArchiveBuffer( const OutputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, bool _crc32, const void * _data, size_t _size, EArchivatorCompress _compress )
		{ 
			MemoryInputInterfacePtr compress_memory = ARCHIVE_SERVICE()
				->compressBuffer( _archivator, _data, _size, _compress );

			if( compress_memory == nullptr )
			{
				LOGGER_ERROR("writeStreamArchiveBuffer: invalid compress"
					);

				return false;
			}

			const void * compressBuffer = compress_memory->getMemory();
			size_t compressSize = compress_memory->getSize();

			if( compressBuffer == nullptr )
			{
				LOGGER_ERROR("writeStreamArchiveBuffer: invalid get memory"
					);

				return false;
			}

			uint32_t value_crc32 = _crc32 == true ? Helper::make_crc32( compressBuffer, compressSize ) : 0;

			if( _stream->write( &value_crc32, sizeof(value_crc32) ) == false )
			{
				LOGGER_ERROR("writeStreamArchiveBuffer: invalid write 'crc32'"
					);

				return false;
			}

			uint32_t write_uncompressSize = (uint32_t)_size;
			if( _stream->write( &write_uncompressSize, sizeof(write_uncompressSize) ) == false )
			{
				LOGGER_ERROR("writeStreamArchiveBuffer: invalid write 'uncompress size'"
					);

				return false;
			}

			uint32_t write_compressSize = (uint32_t)compressSize;
			if( _stream->write( &write_compressSize, sizeof(write_compressSize) ) == false )
			{
				LOGGER_ERROR("writeStreamArchiveBuffer: invalid write 'compress size'"
					);

				return false;
			}

			if( _stream->write( compressBuffer, compressSize ) == false )
			{
				LOGGER_ERROR("writeStreamArchiveBuffer: invalid write 'buffer %d'"
					, compressSize
					);

				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool loadStreamArchiveData( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, MemoryInterfacePtr & _binaryBuffer, const char * _file, uint32_t _line )
		{
			if( Helper::loadStreamMagicHeader( _stream, _magic, _version ) == false )
			{
				return false;
			}

            if( Helper::loadStreamArchiveBuffer( _stream, _archivator, _binaryBuffer, _file, _line ) == false )
			{
				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool writeStreamArchiveData( const OutputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, bool _crc32, const void * _data, size_t _size, EArchivatorCompress _compress )
		{
			if( Helper::writeStreamMagicHeader( _stream, _magic, _version ) == false )
			{
				return false;
			}

			if( Helper::writeStreamArchiveBuffer( _stream, _archivator, _crc32, _data, _size, _compress ) == false )
			{
				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		MemoryInterfacePtr loadStreamArchiveMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, const char * _file, uint32_t _line )
		{
			uint32_t crc32;
			_stream->read( &crc32, sizeof( crc32 ) );

			uint32_t load_binary_size;
			_stream->read( &load_binary_size, sizeof( load_binary_size ) );

			uint32_t load_compress_size;
			_stream->read( &load_compress_size, sizeof( load_compress_size ) );

			size_t binary_size = load_binary_size;
			size_t compress_size = load_compress_size;

			MemoryInterfacePtr compress_buffer = Helper::createMemoryCacheBuffer( compress_size, _file, _line );

			if( compress_buffer == nullptr )
			{
				LOGGER_ERROR( "loadStreamArchiveBuffer: invalid get memory %d (compress)"
					, compress_size
					);

				return nullptr;
			}

			void * compress_memory = compress_buffer->getMemory();

			size_t read_data = _stream->read( compress_memory, compress_size );

			if( read_data != (size_t)compress_size )
			{
                LOGGER_ERROR( "loadStreamArchiveBuffer: invalid read data %d need %d"
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
					LOGGER_ERROR( "loadStreamArchiveBuffer: invalid crc32 %d need %d"
						, check_crc32
						, crc32
						);

					return nullptr;
				}
			}

            MemoryBufferInterfacePtr binary_buffer = MEMORY_SERVICE()
				->createMemoryBuffer();

			if( binary_buffer == nullptr )
			{
				LOGGER_ERROR( "loadStreamArchiveBuffer: invalid get memory %d (binary)"
					, binary_size
					);

				return nullptr;
			}

			void * binary_memory = binary_buffer->newMemory( binary_size, _file, _line );

			size_t uncompressSize = 0;
			if( _archivator->decompress( binary_memory, binary_size, compress_memory, compress_size, uncompressSize ) == false )
			{
				LOGGER_ERROR( "loadStreamArchiveBuffer: invalid decompress"
					);

				return nullptr;
			}

			if( uncompressSize != binary_size )
			{
				LOGGER_ERROR( "loadStreamArchiveBuffer: invalid decompress size %d need %d"
					, uncompressSize
					, binary_size
					);

				return nullptr;
			}

			return binary_buffer;
		}
		//////////////////////////////////////////////////////////////////////////
		MemoryInterfacePtr loadStreamArchiveMagicMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, magic_number_type _magic, magic_version_type _version, const char * _file, uint32_t _line )
		{
			if( Helper::loadStreamMagicHeader( _stream, _magic, _version ) == false )
			{
				return nullptr;
			}

			MemoryInterfacePtr memory = Helper::loadStreamArchiveMemory( _stream, _archivator, _file, _line );

			return memory;
		}
	}
}