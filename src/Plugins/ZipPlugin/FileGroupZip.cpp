#include "FileGroupZip.h"

#include "Interface/ServiceInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/FilePath.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/String.h"
#include "Kernel/MemoryHelper.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

#include "zlib.h"

#include <string.h>
#include <stdio.h>

#define ZIP_LOCAL_FILE_HEADER_SIGNATURE	0x04034b50
#define ZIP_MAX_FILENAME 1024

namespace Mengine
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
    static uint32_t s_get_uint32( uint8_t * _buff )
    {
        uint32_t x;
        x = (uint32_t)_buff[0];
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
    bool FileGroupZip::initialize( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & _path )
    {
        m_name = _name;
        m_fileGroup = _fileGroup;
        m_path = _path;

        if( this->loadHeader_() == false )
        {
            LOGGER_ERROR( "can't load header '%s'"
                , m_path.c_str()
            );

            return false;
        }

        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, false );

        m_mutex = mutex;

        uint32_t FileGroupZipReserveFiles = CONFIG_VALUE( "Engine", "FileGroupZipReserveFiles", 16 * 1024 );

        m_files.reserve( FileGroupZipReserveFiles );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::loadHeader_()
    {
        InputStreamInterfacePtr zipFile = FILE_SERVICE()
            ->openInputFile( m_fileGroup, m_path, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( zipFile, false, "can't open input stream for path '%s'"
            , m_path.c_str()
        );

        size_t zip_size = zipFile->size();
        zipFile->seek( zip_size - 22 );

        uint32_t header_position = (uint32_t)zipFile->tell();

        uint8_t endof_central_dir[22];

        if( zipFile->read( endof_central_dir, 22 ) != 22 )
        {
            LOGGER_ERROR( "invalid zip format '%s'"
                , m_path.c_str()
            );

            return false;
        }

        uint32_t eocd = s_get_uint32( endof_central_dir );

        if( eocd != 0x06054B50 )
        {
            LOGGER_ERROR( "bad 'End of Central Dir' signature zip '%s'"
                , m_path.c_str()
            );

            return false;
        }

        uint32_t header_size = s_get_uint32( endof_central_dir + 12 );
        uint32_t header_offset = s_get_uint32( endof_central_dir + 16 );
        uint32_t arc_offset = header_position - header_offset - header_size;

        header_offset += arc_offset;

        Char fileNameBuffer[ZIP_MAX_FILENAME];

        for( ;;)
        {
            zipFile->seek( header_offset );

            uint32_t signature;
            zipFile->read( &signature, sizeof( signature ) );

            if( signature != 0x02014B50 )
            {
                break;
            }

            ZipCentralDirectoryFileHeader header;

            zipFile->read( &header.versionMade, sizeof( header.versionMade ) );
            zipFile->read( &header.versionNeeded, sizeof( header.versionNeeded ) );
            zipFile->read( &header.generalPurposeFlag, sizeof( header.generalPurposeFlag ) );
            zipFile->read( &header.compressionMethod, sizeof( header.compressionMethod ) );
            zipFile->read( &header.lastModTime, sizeof( header.lastModTime ) );
            zipFile->read( &header.lastModDate, sizeof( header.lastModDate ) );
            zipFile->read( &header.crc32, sizeof( header.crc32 ) );
            zipFile->read( &header.compressedSize, sizeof( header.compressedSize ) );
            zipFile->read( &header.uncompressedSize, sizeof( header.uncompressedSize ) );
            zipFile->read( &header.fileNameLen, sizeof( header.fileNameLen ) );
            zipFile->read( &header.extraFieldLen, sizeof( header.extraFieldLen ) );
            zipFile->read( &header.commentLen, sizeof( header.commentLen ) );
            zipFile->read( &header.diskNumber, sizeof( header.diskNumber ) );
            zipFile->read( &header.internalAttributes, sizeof( header.internalAttributes ) );
            zipFile->read( &header.externalAttributes, sizeof( header.externalAttributes ) );
            zipFile->read( &header.relativeOffset, sizeof( header.relativeOffset ) );

            uint32_t fileOffset = header.relativeOffset + 30 + header.fileNameLen + header.extraFieldLen;

            zipFile->read( &fileNameBuffer, header.fileNameLen );

            uint32_t file_header_size = 46 + header.fileNameLen + header.extraFieldLen + header.commentLen;
            header_offset += file_header_size;

            if( header.compressedSize == 0 ) // folder
            {
                continue;
            }

            FilePath fileName = Helper::stringizeFilePathSize( fileNameBuffer, header.fileNameLen );

            if( header.compressionMethod != Z_NO_COMPRESSION && header.compressionMethod != Z_DEFLATED )
            {
                LOGGER_ERROR( "pak '%s' file '%s' invalid compress method %d"
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

            m_files.emplace( fileName, fi );
            m_indexes.push_back( fileName );

            zipFile->skip( header.compressedSize );
        }

        m_zipFile = zipFile;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileGroupZip::finalize()
    {
        m_files.clear();
        m_indexes.clear();

        m_fileGroup = nullptr;
        m_zipFile = nullptr;

        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & FileGroupZip::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    const FileGroupInterfacePtr & FileGroupZip::getCategory() const
    {
        return m_fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::isPacked() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & FileGroupZip::getRelationPath() const
    {
        return m_relationPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & FileGroupZip::getFolderPath() const
    {
        return m_path;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::existFile( const FilePath & _fileName ) const
    {
        MapFileInfo::const_iterator it_found = m_files.find( _fileName );

        if( it_found == m_files.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::existDirectory( const FilePath & _folderName ) const
    {
        MENGINE_UNUSED( _folderName );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::createDirectory( const FilePath & _folderName ) const
    {
        MENGINE_UNUSED( _folderName );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    static int wildcards_mask( const Char * string, const Char * mask, int endofmask = 1 )
    {
        while( *string )
        {
            switch( *mask )
            {
            case '\0':
                {
                    return endofmask;
                }
            case '*':
                {
                    ++mask;

                    while( *string )
                    {
                        int match = wildcards_mask( string, mask, endofmask );

                        if( match >= 0 )
                        {
                            return match;
                        }

                        ++string;
                    }

                    return 1;
                }
            default:
                {
                    if( *string == *mask )
                    {
                        ++string;
                        ++mask;
                        break;
                    }
                    return -1;
                }break;
            }
        }

        if( *mask == '\0' )
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::findFiles( const FilePath & _filePath, const Char * _mask, const LambdaFilePath & _lambda ) const
    {
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _mask );
        MENGINE_UNUSED( _lambda );

        PathString mask;
        mask.append( _filePath.c_str(), _filePath.size() );
        mask.append( _mask );

        for( const FilePath & path : m_indexes )
        {
            const Char * path_str = path.c_str();
            const Char * mask_str = mask.c_str();

            if( wildcards_mask( path_str, mask_str ) == 0 )
            {
                _lambda( path );
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    static voidpf s_alloc_func( voidpf opaque, uInt items, uInt size )
    {
        MENGINE_UNUSED( opaque );

        void * memory = stdex_malloc( items * size, "FileGroupZip" );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////	
    static void s_free_func( voidpf opaque, voidpf address )
    {
        MENGINE_UNUSED( opaque );

        stdex_free( address, "FileGroupZip" );
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

        int32_t err_init = inflateInit2( &zs, -MAX_WBITS );

        if( err_init != Z_OK )
        {
            return false;
        }

        int32_t err_inflate = inflate( &zs, Z_FINISH );
        int32_t err_end = inflateEnd( &zs );

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
    InputStreamInterfacePtr FileGroupZip::createInputFile( const FilePath & _fileName, bool _streaming, const Char * _doc )
    {
        MapFileInfo::const_iterator it_found = m_files.find( _fileName );

        if( it_found == m_files.end() )
        {
            return nullptr;
        }

        if( _streaming == true )
        {
            InputStreamInterfacePtr stream = m_fileGroup->createInputFile( _fileName, true, _doc );

            return stream;
        }

        MemoryInputInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryInput( _doc );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::openInputFile( const FilePath & _fileName, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, false, "pak '%s' file '%s' stream is NULL"
            , m_path.c_str()
            , _fileName.c_str()
        );

        MapFileInfo::const_iterator it_found = m_files.find( _fileName );

        if( it_found == m_files.end() )
        {
            LOGGER_ERROR( "pak '%s' file '%s' not found"
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
            LOGGER_ERROR( "pak '%s' file '%s' invalid open range %d:%d (file size is low %d:%d)"
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
                LOGGER_ERROR( "pak '%s' file '%s' invalid open, not support compress + stream"
                    , m_path.c_str()
                    , _fileName.c_str()
                );

                return false;
            }

            if( m_fileGroup->openInputFile( m_path, _stream, file_offset, file_size, true ) == false )
            {
                LOGGER_ERROR( "pak '%s' file '%s' invalid open range %d:%d"
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
            void * buffer = memory->newBuffer( fi.file_size, MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, false, "pak '%s' file '%s' failed new memory %d"
                , m_path.c_str()
                , _fileName.c_str()
                , fi.unz_size
            );

            m_mutex->lock();
            m_zipFile->seek( file_offset );
            m_zipFile->read( buffer, fi.file_size );
            m_mutex->unlock();
        }
        else
        {
            void * buffer = memory->newBuffer( fi.unz_size, MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, false, "pak '%s' file '%s' failed new memory %d"
                , m_path.c_str()
                , _fileName.c_str()
                , fi.unz_size
            );

            MemoryInterfacePtr compress_buffer = Helper::createMemoryCacheBuffer( fi.file_size, MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( compress_buffer, false, "pak '%s' file '%s' failed cache memory %d"
                , m_path.c_str()
                , _fileName.c_str()
                , fi.file_size
            );

            void * compress_memory = compress_buffer->getBuffer();

            MENGINE_ASSERTION_MEMORY_PANIC( compress_memory, false );

            m_mutex->lock();
            m_zipFile->seek( file_offset );
            m_zipFile->read( compress_memory, fi.file_size );
            m_mutex->unlock();

            if( s_inflate_memory( buffer, fi.unz_size, compress_memory, fi.file_size ) == false )
            {
                LOGGER_ERROR( "pak '%s' file '%s' failed inflate"
                    , m_path.c_str()
                    , _fileName.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr FileGroupZip::createOutputFile( const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        LOGGER_ERROR( "unsupport method" );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::openOutputFile( const FilePath& _fileName, const OutputStreamInterfacePtr & _file )
    {
        MENGINE_UNUSED( _fileName );
        MENGINE_UNUSED( _file );

        LOGGER_ERROR( "'%s' unsupport method"
            , _fileName.c_str()
        );

        return false;
    }
}
