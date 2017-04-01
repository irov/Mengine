#	include "FileGroupZip.h"

#	include "Interface/ServiceInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/MemoryInterface.h"

#	include "Logger/Logger.h"

#	include "Core/FilePath.h"
#	include "Core/String.h"
#	include "Core/MemoryHelper.h"

#	include <string.h>
#   include <stdio.h>

#	include "../dependencies/zlib/zlib.h"

#	define ZIP_LOCAL_FILE_HEADER_SIGNATURE	0x04034b50
#	define ZIP_MAX_FILENAME 1024

namespace Menge
{
	struct ZipCentralDirectoryFileHeader
	{
		uint16_t versionMade;
		uint16_t versionNeeded;
		uint16_t generalPurposeFlag;
		uint16_t compressionMethod;
		uint16_t lastModTime;
		uint16_t lastModDate;
		uint32_t crc32;
		uint32_t compressedSize;
		uint32_t uncompressedSize;
		uint16_t fileNameLen;
		uint16_t extraFieldLen;
        uint16_t commentLen;
		uint16_t diskNumber;
		uint16_t internalAttributes;
		uint32_t externalAttributes;
		uint32_t relativeOffset;
	};
    //////////////////////////////////////////////////////////////////////////
    static uint32_t s_get_uint32( unsigned char * _buff )
    {
        uint32_t x;
        x =  (uint32_t)_buff[0];
        x |= (uint32_t)_buff[1] << 8;
        x |= (uint32_t)_buff[2] << 16;
        x |= (uint32_t)_buff[3] << 24;

        return x;
    }
	//////////////////////////////////////////////////////////////////////////
	FileGroupZip::FileGroupZip()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FileGroupZip::~FileGroupZip()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupZip::initialize( const ConstString & _path )
	{
        m_path = _path;

		if( this->loadHeader_() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("FileSystemZip::initialize can't load header %s"
				, m_path.c_str()
				);

			return false;
		}

		m_mutex = THREAD_SERVICE(m_serviceProvider)
			->createMutex( __FILE__, __LINE__ );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupZip::loadHeader_()
	{
		FileGroupInterfacePtr zipFileGroup;
		if( FILE_SERVICE(m_serviceProvider)
			->hasFileGroup( ConstString::none(), &zipFileGroup ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("FileSystemZip::loadHeader_ can't open file group for path %s"
				, m_path.c_str()
				);

			return false;
		}

		m_zipFileGroup = zipFileGroup;
		
		InputStreamInterfacePtr zipFile = FILE_SERVICE(m_serviceProvider)
			->openInputFile( ConstString::none(), m_path, false );

		if( zipFile == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileSystemZip::loadHeader_ can't open input stream for path %s"
				, m_path.c_str()
				);

			return false;
		}

		size_t zip_size = zipFile->size();
		zipFile->seek( zip_size - 22 );

		uint32_t header_position = (uint32_t)zipFile->tell();

		unsigned char endof_central_dir[22];

		if( zipFile->read( endof_central_dir, 22 ) != 22 )
		{
			LOGGER_ERROR(m_serviceProvider)("FileSystemZip::loadHeader_ invalid zip format %s"
				, m_path.c_str()
				);

			return false;
		}

		uint32_t eocd = s_get_uint32( endof_central_dir );

		if( eocd != 0x06054B50 )
		{
			LOGGER_ERROR(m_serviceProvider)("FileSystemZip::loadHeader_ bad 'End of Central Dir' signature zip %s"
				, m_path.c_str()
				);

			return false;
		}

		uint32_t header_size = s_get_uint32(endof_central_dir + 12);
		uint32_t header_offset = s_get_uint32(endof_central_dir + 16);
		uint32_t arc_offset = header_position - header_offset - header_size;

		header_offset += arc_offset;

		char fileNameBuffer[ZIP_MAX_FILENAME];

		for(;;)
		{
			zipFile->seek( header_offset );

			uint32_t signature;
			zipFile->read( &signature, sizeof(signature) );
			//int signature = s_read_int( zipFile );

			if( signature != 0x02014B50 )
			{
				break;
			}

			ZipCentralDirectoryFileHeader header;
						
			zipFile->read( &header.versionMade, sizeof(header.versionMade) );
			zipFile->read( &header.versionNeeded, sizeof(header.versionNeeded) );
			zipFile->read( &header.generalPurposeFlag, sizeof(header.generalPurposeFlag) );
			zipFile->read( &header.compressionMethod, sizeof(header.compressionMethod) );
			zipFile->read( &header.lastModTime, sizeof(header.lastModTime) );
			zipFile->read( &header.lastModDate, sizeof(header.lastModDate) );
			zipFile->read( &header.crc32, sizeof(header.crc32) );
			zipFile->read( &header.compressedSize, sizeof(header.compressedSize) );
			zipFile->read( &header.uncompressedSize, sizeof(header.uncompressedSize) );
			zipFile->read( &header.fileNameLen, sizeof(header.fileNameLen) );
			zipFile->read( &header.extraFieldLen, sizeof(header.extraFieldLen) );
			zipFile->read( &header.commentLen, sizeof(header.commentLen) );
			zipFile->read( &header.diskNumber, sizeof(header.diskNumber) );
			zipFile->read( &header.internalAttributes, sizeof(header.internalAttributes) );
			zipFile->read( &header.externalAttributes, sizeof(header.externalAttributes) );
			zipFile->read( &header.relativeOffset, sizeof(header.relativeOffset) );
			
			uint32_t fileOffset = header.relativeOffset + 30 + header.fileNameLen + header.extraFieldLen;

			zipFile->read( &fileNameBuffer, header.fileNameLen );

			uint32_t file_header_size = 46 + header.fileNameLen + header.extraFieldLen + header.commentLen;
			header_offset += file_header_size;
						
			if( header.compressedSize == 0 ) // folder
			{
				continue;
			}

			ConstString fileName = Helper::stringizeStringSize(m_serviceProvider, fileNameBuffer, header.fileNameLen);
			
			if( header.compressionMethod != Z_NO_COMPRESSION && header.compressionMethod != Z_DEFLATED )
			{
				LOGGER_ERROR(m_serviceProvider)("FileGroupZip::createInputFile: pak %s file %s invalid compress method %d"
					, m_path.c_str()
					, fileName.c_str()
					, header.compressionMethod
					);

				return false;
			}

			FileInfo fi;

			fi.seek_pos = fileOffset;
			fi.file_size = header.compressedSize;
			fi.unz_size = header.uncompressedSize;
			fi.compr_method = header.compressionMethod;
			
			m_files.insert( std::make_pair(fileName, fi) );
			
			zipFile->skip( header.compressedSize );
		}

		m_zipFile = zipFile;

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void FileGroupZip::finalize()
    {
        m_zipFileGroup = nullptr;
		m_zipFile = nullptr;
    }
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupZip::isPacked() const
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & FileGroupZip::getPath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupZip::existFile( const FilePath & _fileName ) const
	{
		TMapFileInfo::const_iterator it_found = m_files.find( _fileName );

		if( it_found == m_files.end() )
		{
			return false;
		}		

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static voidpf s_alloc_func(voidpf opaque, uInt items, uInt size)
	{
		(void)opaque;

		void * memory = stdex_malloc( items * size );

		return memory;
	}
	//////////////////////////////////////////////////////////////////////////	
	static void s_free_func( voidpf opaque, voidpf address )
	{
		(void)opaque;

		stdex_free( address );
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_inflate_memory( void * _buffer, size_t _capacity, const void * _src, size_t _size )
	{
		z_stream zs;
		zs.next_in = (z_const Bytef *)_src;
		zs.avail_in = (uInt)_size;
		/* Check for source > 64K on 16-bit machine: */
		//if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;

		zs.next_out = (Bytef *)_buffer;
		zs.avail_out = (uInt)_capacity;
		//if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

		zs.zalloc = &s_alloc_func;
		zs.zfree = &s_free_func;

		int err_init = inflateInit2( &zs, -MAX_WBITS );

		if( err_init != Z_OK )
		{
			return false;
		}

		int err_inflate = inflate(&zs, Z_FINISH);		
		int err_end = inflateEnd( &zs );

		if( err_inflate != Z_STREAM_END )
		{
			return false;
		}

		if( err_end != Z_OK )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterfacePtr FileGroupZip::createInputFile( const FilePath & _fileName, bool _streaming )
	{
		TMapFileInfo::const_iterator it_found = m_files.find( _fileName );

		if( it_found == m_files.end() )
		{
			return nullptr;
		}
			
		if( _streaming == true )
		{
			InputStreamInterfacePtr stream = m_zipFileGroup->createInputFile( _fileName, true );

			return stream;
		}
		
		MemoryInputInterfacePtr memory = MEMORY_SERVICE( m_serviceProvider )
			->createMemoryInput();
				
		return memory;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupZip::openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming )
	{
		if( _stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileGroupZip::openInputFile: pak %s file %s stream is NULL"
				, m_path.c_str()
				, _fileName.c_str()
				);

			return false;
		}

		TMapFileInfo::const_iterator it_found = m_files.find( _fileName );

		if( it_found == m_files.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("FileGroupZip::openInputFile: pak %s file %s not found"
				, m_path.c_str()
				, _fileName.c_str()
				);
			
			return false;
		}

		const FileInfo & fi = it_found->second;

		size_t file_offset = fi.seek_pos + _offset;
		size_t file_size = _size == 0 ? fi.file_size : _size;
	
		if( _offset + file_size > fi.file_size )
		{
			LOGGER_ERROR(m_serviceProvider)("FileGroupZip::openInputFile: pak %s file %s invalid open range %d:%d (file size is low %d:%d)"
				, m_path.c_str()
				, _fileName.c_str()
				, _offset
				, _size
				, fi.seek_pos
				, fi.file_size
				);

			return false;
		}
				
		if( _streaming == true )
		{
			if( fi.compr_method != Z_NO_COMPRESSION )
			{
				LOGGER_ERROR(m_serviceProvider)("FileGroupZip::openInputFile: pak %s file %s invalid open, not support compress + stream"
					, m_path.c_str()
					, _fileName.c_str()
					);

				return false;
			}

			if( m_zipFileGroup->openInputFile( m_path, _stream, file_offset, file_size, true ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("FileGroupZip::openInputFile: pak %s file %s invalid open range %d:%d"
					, m_path.c_str()
					, _fileName.c_str()
					, fi.seek_pos
					, fi.file_size
					);

				return false;
			}

			return true;
		}
		
		MemoryInputInterface * memory = stdex::intrusive_get<MemoryInputInterface *>( _stream );
		
		if( fi.compr_method == Z_NO_COMPRESSION )
		{
			void * buffer = memory->newMemory( fi.file_size );

			if( buffer == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("FileGroupZip::createInputFile: pak %s file %s failed new memory %d"
					, m_path.c_str()
					, _fileName.c_str()
					, fi.unz_size
					);

				return false;
			}

			m_mutex->lock();
			m_zipFile->seek( file_offset );
			m_zipFile->read( buffer, fi.file_size );
			m_mutex->unlock();
		}
		else
		{
			void * buffer = memory->newMemory( fi.unz_size );

			if( buffer == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("FileGroupZip::createInputFile: pak %s file %s failed new memory %d"
					, m_path.c_str()
					, _fileName.c_str()
					, fi.unz_size
					);

				return false;
			}

			MemoryInterfacePtr compress_buffer = Helper::createMemoryCacheBuffer( m_serviceProvider, fi.file_size, __FILE__, __LINE__ );
			
			if( compress_buffer == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("FileGroupZip::createInputFile: pak %s file %s failed cache memory %d"
					, m_path.c_str()
					, _fileName.c_str()
					, fi.file_size
					);

				return false;
			}

			void * compress_memory = compress_buffer->getMemory();

			m_mutex->lock();
			m_zipFile->seek( file_offset );
			m_zipFile->read( compress_memory, fi.file_size );
			m_mutex->unlock();

			if( s_inflate_memory( buffer, fi.unz_size, compress_memory, fi.file_size ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("FileGroupZip::createInputFile: pak %s file %s failed inflate"
					, m_path.c_str()
					, _fileName.c_str()
					);

				return false;
			}
		}
		
		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr FileGroupZip::createOutputFile()
    {
		LOGGER_ERROR(m_serviceProvider)("FileGroupZip::createOutputFile unsupport method"
			);

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::openOutputFile( const FilePath& _fileName, const OutputStreamInterfacePtr & _file )
    {
        (void)_fileName;
        (void)_file;

        LOGGER_ERROR(m_serviceProvider)("FileGroupZip::openOutputFile %s unsupport method"
            , _fileName.c_str()
            );

        return false;
    }}	// namespace Menge
