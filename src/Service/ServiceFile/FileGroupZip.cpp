#	include "FileGroupZip.h"

#	include "Interface/ServiceInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#	include "Core/String.h"
#   include "Core/File.h"

#   include <stdio.h>

#	define ZIP_LOCAL_FILE_HEADER_SIGNATURE	0x04034b50
#	define MAX_FILENAME 1024

namespace Menge
{
	struct ZipCentralDirectoryFileHeader
	{
		uint16 versionNeeded;
		uint16 generalPurposeFlag;
		uint16 compressionMethod;
		uint16 lastModTime;
		uint16 lastModDate;
		uint32 crc32;
		uint32 compressedSize;
		uint32 uncompressedSize;
		uint16 fileNameLen;
		uint16 extraFieldLen;
        uint16 commentLen;
	};
    //////////////////////////////////////////////////////////////////////////
    static int get_int( unsigned char * _buff )
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
    static int read_int( FileInputStreamInterface * _fileStream )
    {
        unsigned char buff[4];

        _fileStream->read( buff, 4 );

        int value = get_int( buff );

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
        : m_serviceProvider(NULL)
        , m_zipMappedFile(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FileGroupZip::~FileGroupZip()
	{
		if( m_zipMappedFile != NULL )
		{
			m_zipMappedFile->destroy();
            m_zipMappedFile = NULL;
		}        
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupZip::initialize( ServiceProviderInterface * _serviceProvider, const FilePath & _path, const ConstString & _type, FileSystemInterface * _fileSystem, bool _create )
	{
        (void)_create;

        m_serviceProvider = _serviceProvider;

        m_path = _path;
        m_type = _type;
       
		m_zipMappedFile = _fileSystem->createMappedInputStream();

        if( m_zipMappedFile == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "FileSystemZip::initialize can't create mapped input stream for path %s"
                , m_path.c_str()
                );

            return false;
        }
        
        if( m_zipMappedFile->open( m_path ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "FileSystemZip::initialize can't open zip file data %s"
                , m_path.c_str()
                );

            m_zipMappedFile->destroy();
            m_zipMappedFile = NULL;

            return false;
        }
        
		//uint32 signature = 0;
        
        size_t zip_size = m_zipMappedFile->size();
        m_zipMappedFile->seek( zip_size - 22 );

        int header_position = m_zipMappedFile->tell();

        unsigned char endof_central_dir[22];

        if( m_zipMappedFile->read( endof_central_dir, 22 ) != 22 )
        {
            LOGGER_ERROR(m_serviceProvider)( "FileSystemZip::initialize invalid zip format %s"
                , _path.c_str()
                );

            return false;
        }

        if( get_int(endof_central_dir) != 0x06054B50 )
        {
            LOGGER_ERROR(m_serviceProvider)( "FileSystemZip::initialize bad 'End of Central Dir' signature zip %s"
                , _path.c_str()
                );

            return false;
        }

        int header_size = get_int(endof_central_dir + 12);
        int header_offset = get_int(endof_central_dir + 16);
        int arc_offset = header_position - header_offset - header_size;
        
        header_offset += arc_offset;
        
		char fileNameBuffer[MAX_FILENAME];

		while( true, true )
		{
            m_zipMappedFile->seek( header_offset );

			int signature = read_int( m_zipMappedFile );

			if( signature != 0x02014B50 )
			{
				break;
			}
            			
            ZipCentralDirectoryFileHeader header;

            m_zipMappedFile->seek( header_offset + 10 );

            m_zipMappedFile->read( &header.compressionMethod, sizeof( header.compressionMethod ) );
            m_zipMappedFile->read( &header.lastModTime, sizeof( header.lastModTime ) );
            m_zipMappedFile->read( &header.lastModDate, sizeof( header.lastModDate ) );
            m_zipMappedFile->read( &header.crc32, sizeof( header.crc32 ) );
            m_zipMappedFile->read( &header.compressedSize, sizeof( header.compressedSize ) );
            m_zipMappedFile->read( &header.uncompressedSize, sizeof( header.uncompressedSize ) );
            m_zipMappedFile->read( &header.fileNameLen, sizeof( header.fileNameLen ) );
            m_zipMappedFile->read( &header.extraFieldLen, sizeof( header.extraFieldLen ) );
            m_zipMappedFile->read( &header.commentLen, sizeof( header.commentLen ) );

            m_zipMappedFile->seek( header_offset + 42 );
            
            uint32 localFileHeaderOffset; 
            m_zipMappedFile->read( &localFileHeaderOffset, sizeof(localFileHeaderOffset) );

            uint32 fileOffset = localFileHeaderOffset + 30 + header.fileNameLen + header.extraFieldLen;

            m_zipMappedFile->seek( header_offset + 46 );
            m_zipMappedFile->read( &fileNameBuffer, header.fileNameLen );

            uint32 header_size = 46 + header.fileNameLen + header.extraFieldLen + header.commentLen;
            header_offset += header_size;

            if( header.compressedSize == 0 ) // if folder
            {
                continue;
            }
			
            FilePath fileName = Helper::stringizeStringSize(m_serviceProvider, fileNameBuffer, header.fileNameLen);
            						
			if( header.compressionMethod != 0 )
			{
				LOGGER_ERROR(m_serviceProvider)( "FileSystemZip::initialize compressed %d file '%s'"
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
            			
			m_files.insert( std::make_pair( fileName, fi ) );
			
			Utils::skip( m_zipMappedFile, header.compressedSize );
		}

        m_zipMappedFile->seek( 0 );

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    void FileGroupZip::finalize()
    {
        if( m_zipMappedFile )
        {
            m_zipMappedFile->destroy();
            m_zipMappedFile = NULL;
        }
    }
	//////////////////////////////////////////////////////////////////////////
	const FilePath & FileGroupZip::getPath() const
	{
		return m_path;
	}
    //////////////////////////////////////////////////////////////////////////
    const ConstString & FileGroupZip::getType() const
    {
        return m_type;
    }
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupZip::existFile( const FilePath& _filename )
	{
		TMapFileInfo::iterator it_find = m_files.find( _filename );
		if( it_find == m_files.end() )
		{
            LOGGER_INFO(m_serviceProvider)("FileSystemZip::existFile file %s not exist"
                , _filename.c_str()
                );

			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterface * FileGroupZip::createInputFile()
	{
		InputStreamInterface * stream = m_zipMappedFile->createInputMemory();

		return stream;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupZip::openInputFile( const FilePath& _filename, InputStreamInterface* _stream )
	{
		if( _stream == 0 )
		{
			return false;
		}

		TMapFileInfo::iterator it_find = m_files.find( _filename );
		if( it_find == m_files.end() )
		{
			return false;
		}

        const FileInfo & fi = it_find->second;

        m_zipMappedFile->openInputMemory( _stream, _filename, fi.seek_pos, fi.file_size );

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterface* FileGroupZip::createOutputFile()
    {
        return NULL;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::openOutputFile( const FilePath& _filename, OutputStreamInterface* _file )
    {
        (void)_filename;
        (void)_file;

        LOGGER_ERROR(m_serviceProvider)("FileGroupZip::openOutputFile %s unsupport method"
            , _filename.c_str()
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
