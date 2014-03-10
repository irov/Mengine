#	include "FileGroupZip.h"

#	include "Interface/ServiceInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#	include "Core/FilePath.h"
#	include "Core/String.h"
#   include "Core/File.h"

#	include <string.h>
#   include <stdio.h>

#	define ZIP_LOCAL_FILE_HEADER_SIGNATURE	0x04034b50
#	define MAX_FILENAME 1024

namespace Menge
{
	struct ZipCentralDirectoryFileHeader
	{
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
	};
    //////////////////////////////////////////////////////////////////////////
    static int s_get_int( unsigned char * _buff )
    {
        int x;
        x =  (int)_buff[0];
        x |= (int)_buff[1] << 8;
        x |= (int)_buff[2] << 16;
        x |= (int)_buff[3] << 24;

        return x;
    }
    ////////////////////////////////////////////////////////////////////////////
    //static short get_short( unsigned char * _buff )
    //{
    //    short x;
    //    x =  (short)_buff[0];
    //    x |= (short)_buff[1] << 8;

    //    return x;
    //}
    //////////////////////////////////////////////////////////////////////////
    static int s_read_int( const InputStreamInterfacePtr & _stream )
    {
        unsigned char buff[4];

        _stream->read( buff, 4 );

        int value = s_get_int( buff );

        return value;
    }
    ////////////////////////////////////////////////////////////////////////////
    //static int read_short( FileInputStreamInterface * _fileStream )
    //{
    //    unsigned char buff[2];

    //    _fileStream->read( buff, 2 );

    //    int value = get_int( buff );

    //    return value;
    //}
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

		//uint32 signature = 0;

		size_t zip_size = zipFile->size();
		zipFile->seek( zip_size - 22 );

		int header_position = zipFile->tell();

		unsigned char endof_central_dir[22];

		if( zipFile->read( endof_central_dir, 22 ) != 22 )
		{
			LOGGER_ERROR(m_serviceProvider)("FileSystemZip::loadHeader_ invalid zip format %s"
				, m_path.c_str()
				);

			return false;
		}

		if( s_get_int( endof_central_dir ) != 0x06054B50 )
		{
			LOGGER_ERROR(m_serviceProvider)("FileSystemZip::loadHeader_ bad 'End of Central Dir' signature zip %s"
				, m_path.c_str()
				);

			return false;
		}

		int header_size = s_get_int(endof_central_dir + 12);
		int header_offset = s_get_int(endof_central_dir + 16);
		int arc_offset = header_position - header_offset - header_size;

		header_offset += arc_offset;

		char fileNameBuffer[MAX_FILENAME];

		while( true, true )
		{
			zipFile->seek( header_offset );

			int signature = s_read_int( zipFile );

			if( signature != 0x02014B50 )
			{
				break;
			}

			ZipCentralDirectoryFileHeader header;

			zipFile->seek( header_offset + 10 );

			zipFile->read( &header.compressionMethod, sizeof( header.compressionMethod ) );
			zipFile->read( &header.lastModTime, sizeof( header.lastModTime ) );
			zipFile->read( &header.lastModDate, sizeof( header.lastModDate ) );
			zipFile->read( &header.crc32, sizeof( header.crc32 ) );
			zipFile->read( &header.compressedSize, sizeof( header.compressedSize ) );
			zipFile->read( &header.uncompressedSize, sizeof( header.uncompressedSize ) );
			zipFile->read( &header.fileNameLen, sizeof( header.fileNameLen ) );
			zipFile->read( &header.extraFieldLen, sizeof( header.extraFieldLen ) );
			zipFile->read( &header.commentLen, sizeof( header.commentLen ) );

			zipFile->seek( header_offset + 42 );

			uint32_t localFileHeaderOffset; 
			zipFile->read( &localFileHeaderOffset, sizeof(localFileHeaderOffset) );

			uint32_t fileOffset = localFileHeaderOffset + 30 + header.fileNameLen + header.extraFieldLen;

			zipFile->seek( header_offset + 46 );
			zipFile->read( &fileNameBuffer, header.fileNameLen );

			uint32_t header_size = 46 + header.fileNameLen + header.extraFieldLen + header.commentLen;
			header_offset += header_size;

			if( header.compressedSize == 0 ) // if folder
			{
				//m_folders.insert( fileName );

				continue;
			}

			FilePath fileName = Helper::stringizeStringSize(m_serviceProvider, fileNameBuffer, header.fileNameLen);

			if( header.compressionMethod != 0 )
			{
				LOGGER_ERROR(m_serviceProvider)("FileSystemZip::loadHeader_ %s compressed %d file '%s'"
					, m_path.c_str()
					, header.compressionMethod
					, fileName.c_str() 
					);

				continue;
			}

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
	class FileGroupZip_FinderValueFiles
	{
	public:
		FileGroupZip_FinderValueFiles( const char * _filename, size_t _hash )
			: m_filename(_filename)
			, m_hash(_hash)
		{
		}

	protected:
		void operator = ( const FileGroupZip_FinderValueFiles & )
		{
		}

	public:
		const char * m_filename;
		size_t m_hash;
	};
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
	InputStreamInterfacePtr FileGroupZip::createInputFile()
	{
		InputStreamInterfacePtr stream = m_zipMappedFile->createFileStream();

		return stream;
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

		bool successful = m_zipMappedFile->openFileStream( _stream, fi.seek_pos, fi.file_size );

		return successful;
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
