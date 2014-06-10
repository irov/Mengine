#	include "FileGroupZip.h"

#	include "Interface/ServiceInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Core/InputStreamBuffer.h"

#	include "Logger/Logger.h"

#	include "Core/CacheMemoryBuffer.h"
#	include "Core/FilePath.h"
#	include "Core/String.h"
#   include "Core/File.h"

#	include <string.h>
#   include <stdio.h>

#	include "../dependencies/zlib/zlib.h"

#	define ZIP_LOCAL_FILE_HEADER_SIGNATURE	0x04034b50
#	define MAX_FILENAME 1024

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
	bool FileGroupZip::initialize( ServiceProviderInterface * _serviceProvider, const FilePath & _path )
	{
        m_serviceProvider = _serviceProvider;

        m_path = _path;

		if( this->loadHeader_() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("FileSystemZip::initialize can't load header %s"
				, m_path.c_str()
				);

			return false;
		}
		
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
			->openInputFile( ConstString::none(), m_path );

		if( zipFile == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileSystemZip::loadHeader_ can't open input stream for path %s"
				, m_path.c_str()
				);

			return false;
		}

		InputStreamBuffer8196 zipFileBuffer(zipFile);

		size_t zip_size = zipFileBuffer.size();
		zipFileBuffer.seek( zip_size - 22 );

		size_t header_position = zipFileBuffer.tell();

		unsigned char endof_central_dir[22];

		if( zipFileBuffer.read( endof_central_dir, 22 ) != 22 )
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

		char fileNameBuffer[MAX_FILENAME];

		while( true, true )
		{
			zipFileBuffer.seek( header_offset );

			uint32_t signature;
			zipFileBuffer.read( &signature, sizeof(signature) );
			//int signature = s_read_int( zipFile );

			if( signature != 0x02014B50 )
			{
				break;
			}

			ZipCentralDirectoryFileHeader header;
						
			zipFileBuffer.read( &header.versionMade, sizeof(header.versionMade) );
			zipFileBuffer.read( &header.versionNeeded, sizeof(header.versionNeeded) );
			zipFileBuffer.read( &header.generalPurposeFlag, sizeof(header.generalPurposeFlag) );
			zipFileBuffer.read( &header.compressionMethod, sizeof(header.compressionMethod) );
			zipFileBuffer.read( &header.lastModTime, sizeof(header.lastModTime) );
			zipFileBuffer.read( &header.lastModDate, sizeof(header.lastModDate) );
			zipFileBuffer.read( &header.crc32, sizeof(header.crc32) );
			zipFileBuffer.read( &header.compressedSize, sizeof(header.compressedSize) );
			zipFileBuffer.read( &header.uncompressedSize, sizeof(header.uncompressedSize) );
			zipFileBuffer.read( &header.fileNameLen, sizeof(header.fileNameLen) );
			zipFileBuffer.read( &header.extraFieldLen, sizeof(header.extraFieldLen) );
			zipFileBuffer.read( &header.commentLen, sizeof(header.commentLen) );
			zipFileBuffer.read( &header.diskNumber, sizeof(header.diskNumber) );
			zipFileBuffer.read( &header.internalAttributes, sizeof(header.internalAttributes) );
			zipFileBuffer.read( &header.externalAttributes, sizeof(header.externalAttributes) );
			zipFileBuffer.read( &header.relativeOffset, sizeof(header.relativeOffset) );
			
			uint32_t fileOffset = header.relativeOffset + 30 + header.fileNameLen + header.extraFieldLen;

			zipFileBuffer.read( &fileNameBuffer, header.fileNameLen );

			uint32_t header_size = 46 + header.fileNameLen + header.extraFieldLen + header.commentLen;
			header_offset += header_size;
						
			if( header.compressedSize == 0 ) // folder
			{
				continue;
			}

			FilePath fileName = Helper::stringizeStringSize(m_serviceProvider, fileNameBuffer, header.fileNameLen);
			
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
			
			m_files.insert( fi );

			zipFileBuffer.skip( header.compressedSize );
		}

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void FileGroupZip::finalize()
    {
        m_zipFileGroup = nullptr;
    }
	//////////////////////////////////////////////////////////////////////////
	const FilePath & FileGroupZip::getPath() const
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

		zs.zalloc = (alloc_func)0;
		zs.zfree = (free_func)0;

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
	InputStreamInterfacePtr FileGroupZip::createInputFile( const FilePath & _fileName )
	{
		FileInfo * info = m_files.find( _fileName );

		if( info == nullptr )
		{
			return nullptr;
		}
			
		if( info->compr_method == Z_NO_COMPRESSION )
		{
			InputStreamInterfacePtr stream = m_zipFileGroup->createInputFile( _fileName );

			return stream;
		}
		
		MemoryInputPtr memory = m_factoryMemoryInput.createObjectT();
		
		return memory;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupZip::openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size )
	{
		if( _stream == nullptr )
		{
			return false;
		}

		FileInfo * info = m_files.find( _fileName );

		if( info == nullptr )
		{
			return false;
		}

		size_t file_offset = info->seek_pos + _offset;
		size_t file_size = _size == 0 ? info->file_size : _size ;
	
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
				
		if( info->compr_method == Z_NO_COMPRESSION )
		{
			if( m_zipFileGroup->openInputFile( m_path, _stream, file_offset, file_size ) == false )
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
		
		InputStreamInterfacePtr stream = m_zipFileGroup->createInputFile( m_path );

		if( stream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileGroupZip::createInputFile: pak %s file %s invalid create stream"
				, m_path.c_str()
				, _fileName.c_str()
				);

			return nullptr;				 
		}

		if( m_zipFileGroup->openInputFile( m_path, stream, file_offset, file_size ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("FileGroupZip::createInputFile: pak %s file %s invalid open range %d:%d"
				, m_path.c_str()
				, _fileName.c_str()
				, file_offset
				, file_size
				);

			return false;
		}

		MemoryInput * memory = stdex::intrusive_get<MemoryInput>(_stream);
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

		stream->read( compress_memory, info->file_size );
		stream = nullptr;

		if( s_inflate_memory( buffer, info->unz_size, compress_memory, info->file_size ) == false )
		{
			return false;
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
	MappedFileInterfacePtr FileGroupZip::createMappedFile()
	{
		LOGGER_ERROR(m_serviceProvider)("FileGroupZip::createMappedFile unsupport method"
			);

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupZip::openMappedFile( const FilePath & _fileName, const MappedFileInterfacePtr & _stream )
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
