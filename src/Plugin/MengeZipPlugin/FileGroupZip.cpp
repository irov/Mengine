#	include "FileGroupZip.h"

#	include "Interface/ServiceInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/CacheInterface.h"

#	include "Logger/Logger.h"

#	include "Core/CacheMemoryBuffer.h"
#	include "Core/FilePath.h"
#	include "Core/String.h"

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
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FileGroupZip::~FileGroupZip()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void FileGroupZip::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * FileGroupZip::getServiceProvider() const
	{
		return m_serviceProvider;
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
			->createMutex();
		
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

		size_t header_position = zipFile->tell();

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

			uint32_t header_size = 46 + header.fileNameLen + header.extraFieldLen + header.commentLen;
			header_offset += header_size;
						
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

			FileInfo * fi = m_files.create();

			fi->fileName = fileName;
			fi->seek_pos = fileOffset;
			fi->file_size = header.compressedSize;
			fi->unz_size = header.uncompressedSize;
			fi->compr_method = header.compressionMethod;
			
			m_files.insert( fi, nullptr );
			
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
	const ConstString & FileGroupZip::getPath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupZip::existFile( const FilePath & _fileName ) const
	{
		bool successful = m_files.exist( _fileName );

		return successful;
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
		FileInfo * info = m_files.find( _fileName );

		if( info == nullptr )
		{
			return nullptr;
		}
			
		if( _streaming == true )
		{
			InputStreamInterfacePtr stream = m_zipFileGroup->createInputFile( _fileName, true );

			return stream;
		}
		
		MemoryInputPtr memory = CACHE_SERVICE(m_serviceProvider)
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

		FileInfo * info = m_files.find( _fileName );

		if( info == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileGroupZip::openInputFile: pak %s file %s not found"
				, m_path.c_str()
				, _fileName.c_str()
				);
			
			return false;
		}

		size_t file_offset = info->seek_pos + _offset;
		size_t file_size = _size == 0 ? info->file_size : _size;
	
		if( _offset + file_size > info->file_size )
		{
			LOGGER_ERROR(m_serviceProvider)("FileGroupZip::openInputFile: pak %s file %s invalid open range %d:%d (file size is low %d:%d)"
				, m_path.c_str()
				, _fileName.c_str()
				, _offset
				, _size
				, info->seek_pos
				, info->file_size
				);

			return false;
		}
				
		if( _streaming == true )
		{
			if( info->compr_method != Z_NO_COMPRESSION )
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
					, info->seek_pos
					, info->file_size
					);

				return false;
			}

			return true;
		}
		
		MemoryInput * memory = stdex::intrusive_get<MemoryInput>(_stream);
		
		if( info->compr_method == Z_NO_COMPRESSION )
		{
			void * buffer = memory->newMemory( info->file_size );

			if( buffer == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("FileGroupZip::createInputFile: pak %s file %s failed new memory %d"
					, m_path.c_str()
					, _fileName.c_str()
					, info->unz_size
					);

				return false;
			}

			m_mutex->lock();
			m_zipFile->seek( file_offset );
			m_zipFile->read( buffer, info->file_size );
			m_mutex->unlock();
		}
		else
		{
			void * buffer = memory->newMemory( info->unz_size );

			if( buffer == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("FileGroupZip::createInputFile: pak %s file %s failed new memory %d"
					, m_path.c_str()
					, _fileName.c_str()
					, info->unz_size
					);

				return false;
			}

			CacheMemoryBuffer compress_buffer(m_serviceProvider, info->file_size, "FileGroupZip_createInputFile");
			void * compress_memory = compress_buffer.getMemory();

			if( compress_memory == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("FileGroupZip::createInputFile: pak %s file %s failed cache memory %d"
					, m_path.c_str()
					, _fileName.c_str()
					, info->file_size
					);

				return false;
			}

			m_mutex->lock();
			m_zipFile->seek( file_offset );
			m_zipFile->read( compress_memory, info->file_size );
			m_mutex->unlock();

			if( s_inflate_memory( buffer, info->unz_size, compress_memory, info->file_size ) == false )
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
	FileMappedInterfacePtr FileGroupZip::createMappedFile()
	{
		LOGGER_ERROR(m_serviceProvider)("FileGroupZip::createMappedFile unsupport method"
			);

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupZip::openMappedFile( const FilePath & _fileName, const FileMappedInterfacePtr & _stream )
	{
		(void)_fileName;
		(void)_stream;

		LOGGER_ERROR(m_serviceProvider)("FileGroupZip::openMappedFile %s unsupport method"
			, _fileName.c_str()
			);

		return false;
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
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::existDirectory( const FilePath & _path ) const
    {
        //bool result = m_folders.has( _path );

        //return result;

        (void)_path;

		LOGGER_ERROR(m_serviceProvider)("FileGroupZip::existDirectory '%s:%s' unsupport method (path %s)"
            , m_path.c_str()
            , _path.c_str()
            );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::createDirectory( const FilePath& _path )
    {
        (void)_path;

        LOGGER_ERROR(m_serviceProvider)("FileGroupZip::createDirectory '%s:%s' unsupport method"
			, m_path.c_str()
            , _path.c_str()
            );
        
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::removeDirectory( const FilePath& _path )
    {
        (void)_path;

        LOGGER_ERROR(m_serviceProvider)("FileGroupZip::removeDirectory '%s:%s' unsupport method"
			, m_path.c_str()
            , _path.c_str()
            );

        return false;
    }     
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::removeFile( const FilePath& _fileName )
    {
        (void)_fileName;

        LOGGER_ERROR(m_serviceProvider)("FileGroupZip::removeFile '%s:%s' unsupport method"
			, m_path.c_str()
            , _fileName.c_str()
            );

        return false;
    }
}	// namespace Menge
