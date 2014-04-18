#	include "FileGroupZip.h"

#	include "Interface/ServiceInterface.h"
#	include "Interface/ArchiveInterface.h"
#	include "Interface/StringizeInterface.h"

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
        , m_zipMappedFile(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FileGroupZip::~FileGroupZip()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupZip::initialize( ServiceProviderInterface * _serviceProvider, const FilePath & _folder, const FilePath & _path, bool _create )
	{
        (void)_create;

        m_serviceProvider = _serviceProvider;

        m_folder = _folder;
        m_path = _path;

		if( this->loadHeader_() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("FileSystemZip::initialize can't load header %s"
				, m_path.c_str()
				);

			return false;
		}

		m_zipMappedFile = FILE_SERVICE(m_serviceProvider)
			->createMappedFile( ConstString::none(), m_path );

		if( m_zipMappedFile == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileSystemZip::initialize can't create mapped input stream for path %s"
				, m_path.c_str()
				);

			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupZip::loadHeader_()
	{
		InputStreamInterfacePtr zipFile = FILE_SERVICE(m_serviceProvider)
			->openInputFile( ConstString::none(), m_path );

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

		char fileNameBuffer[MAX_FILENAME];

		while( true, true )
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

			FilePath fileName = Helper::stringizeStringSize(m_serviceProvider, fileNameBuffer, header.fileNameLen);

			FileInfo fi;
			fi.seek_pos = fileOffset;
			fi.file_size = header.compressedSize;
			fi.unz_size = header.uncompressedSize;
			fi.compr_method = header.compressionMethod;            			

			m_files.insert( fileName, fi );

			Utils::skip( zipFile, header.compressedSize );
		}

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void FileGroupZip::finalize()
    {
        m_zipMappedFile = nullptr;
    }
	//////////////////////////////////////////////////////////////////////////
	const FilePath & FileGroupZip::getPath() const
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
		TMapFileInfo::const_iterator it_found = m_files.find( _fileName );

		if( it_found == m_files.end() )
		{
			return nullptr;
		}

		const FileInfo & fi = m_files.get_value( it_found );

		InputStreamInterfacePtr stream = m_zipMappedFile->createFileStream();

		if( fi.compr_method == 0 )
		{
			return stream;
		}
		
		void * compress_memory_mapped = nullptr;
		if( m_zipMappedFile->openFileStream( stream, fi.seek_pos, fi.file_size, &compress_memory_mapped ) == false )
		{
			return nullptr;
		}

		MemoryInputPtr memory = m_factoryMemoryInput.createObjectT();
		void * buffer = memory->newMemory( fi.unz_size );

		if( buffer == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("FileGroupZip::createInputFile: pak %s:%s file %s failed new memory %d"
				, m_folder.c_str()
				, m_path.c_str()
				, _fileName.c_str()
				, fi.unz_size
				);

			return nullptr;
		}

		if( compress_memory_mapped == nullptr )
		{
			CacheMemoryBuffer compress_buffer(m_serviceProvider, fi.file_size, "FileGroupZip_createInputFile");
			void * compress_memory = compress_buffer.getMemory();

			if( compress_memory == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("FileGroupZip::createInputFile: pak %s:%s file %s failed cache memory %d"
					, m_folder.c_str()
					, m_path.c_str()
					, _fileName.c_str()
					, fi.file_size
					);

				return nullptr;
			}

			stream->read( compress_memory, fi.file_size );
			stream = nullptr;
		
			if( s_inflate_memory( buffer, fi.unz_size, compress_memory, fi.file_size ) == false )
			{
				return nullptr;
			}
		}
		else
		{
			if( s_inflate_memory( buffer, fi.unz_size, compress_memory_mapped, fi.file_size ) == false )
			{
				return nullptr;
			}
		}

		return memory;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupZip::openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _stream )
	{
		if( _stream == nullptr )
		{
			return false;
		}

		TMapFileInfo::const_iterator it_found = m_files.find( _fileName );

		if( it_found == m_files.end() )
		{
			return false;
		}

		const FileInfo & fi = m_files.get_value( it_found );

		if( fi.compr_method == 0 )
		{			
			bool successful = m_zipMappedFile->openFileStream( _stream, fi.seek_pos, fi.file_size, nullptr );

			return successful;
		}
		
		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr FileGroupZip::createOutputFile()
    {
		LOGGER_ERROR(m_serviceProvider)("FileGroupZip::createOutputFile %s unsupport method"
			);

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::openOutputFile( const FilePath& _filename, const OutputStreamInterfacePtr & _file )
    {
        (void)_filename;
        (void)_file;

        LOGGER_ERROR(m_serviceProvider)("FileGroupZip::openOutputFile %s unsupport method"
            , _filename.c_str()
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
            , m_folder.c_str()
            , m_path.c_str()
            , _path.c_str()
            );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::createDirectory( const FilePath& _path )
    {
        (void)_path;

        LOGGER_ERROR(m_serviceProvider)("FileGroupZip::createDirectory %s unsupport method"
            , _path.c_str()
            );
        
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::removeDirectory( const FilePath& _path )
    {
        (void)_path;

        LOGGER_ERROR(m_serviceProvider)("FileGroupZip::removeDirectory %s unsupport method"
            , _path.c_str()
            );

        return false;
    }     
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::removeFile( const FilePath& _filename )
    {
        (void)_filename;

        LOGGER_ERROR(m_serviceProvider)("FileGroupZip::removeFile %s unsupport method"
            , _filename.c_str()
            );

        return false;
    }
}	// namespace Menge
