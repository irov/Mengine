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
    static int read_int( const MappedFileInputStreamInterfacePtr & _stream )
    {
        unsigned char buff[4];

        _stream->read( buff, 4 );

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
        : m_serviceProvider(nullptr)
        , m_zipMappedFile(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	FileGroupZip::~FileGroupZip()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupZip::initialize( ServiceProviderInterface * _serviceProvider, const FilePath & _folder, const FilePath & _path, const ConstString & _type, bool _create )
	{
        (void)_create;

        m_serviceProvider = _serviceProvider;

        m_folder = _folder;
        m_path = _path;
        m_type = _type;
       
		MappedFileInputStreamInterfacePtr zipMappedFile = FILE_SYSTEM(m_serviceProvider)
            ->createMappedInputStream();

        if( zipMappedFile == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "FileSystemZip::initialize can't create mapped input stream for path %s"
                , m_path.c_str()
                );

            return false;
        }
        
        if( zipMappedFile->open( ConstString::none(), m_path ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "FileSystemZip::initialize can't open zip file data %s"
                , m_path.c_str()
                );

            return false;
        }
       
		//uint32 signature = 0;
        
        size_t zip_size = zipMappedFile->size();
        zipMappedFile->seek( zip_size - 22 );

        int header_position = zipMappedFile->tell();

        unsigned char endof_central_dir[22];

        if( zipMappedFile->read( endof_central_dir, 22 ) != 22 )
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
            zipMappedFile->seek( header_offset );

			int signature = read_int( zipMappedFile );

			if( signature != 0x02014B50 )
			{
				break;
			}
            			
            ZipCentralDirectoryFileHeader header;

            zipMappedFile->seek( header_offset + 10 );

            zipMappedFile->read( &header.compressionMethod, sizeof( header.compressionMethod ) );
            zipMappedFile->read( &header.lastModTime, sizeof( header.lastModTime ) );
            zipMappedFile->read( &header.lastModDate, sizeof( header.lastModDate ) );
            zipMappedFile->read( &header.crc32, sizeof( header.crc32 ) );
            zipMappedFile->read( &header.compressedSize, sizeof( header.compressedSize ) );
            zipMappedFile->read( &header.uncompressedSize, sizeof( header.uncompressedSize ) );
            zipMappedFile->read( &header.fileNameLen, sizeof( header.fileNameLen ) );
            zipMappedFile->read( &header.extraFieldLen, sizeof( header.extraFieldLen ) );
            zipMappedFile->read( &header.commentLen, sizeof( header.commentLen ) );

            zipMappedFile->seek( header_offset + 42 );
            
            uint32 localFileHeaderOffset; 
            zipMappedFile->read( &localFileHeaderOffset, sizeof(localFileHeaderOffset) );

            uint32 fileOffset = localFileHeaderOffset + 30 + header.fileNameLen + header.extraFieldLen;

            zipMappedFile->seek( header_offset + 46 );
            zipMappedFile->read( &fileNameBuffer, header.fileNameLen );

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
			
			Utils::skip( zipMappedFile.get(), header.compressedSize );
		}

        zipMappedFile->seek( 0 );

        m_zipMappedFile = zipMappedFile;

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
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterfacePtr FileGroupZip::createInputFile()
	{
		InputStreamInterfacePtr stream = m_zipMappedFile->createInputMemory();

		return stream;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileGroupZip::openInputFile( const FilePath& _filename, const InputStreamInterfacePtr & _stream )
	{
		if( _stream == nullptr )
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
    OutputStreamInterfacePtr FileGroupZip::createOutputFile()
    {
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
