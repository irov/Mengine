#include "FileGroupZip.h"

#include "Interface/ServiceInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/AllocatorServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FilePath.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

#include "zlib.h"

#define ZIP_LOCAL_FILE_HEADER_SIGNATURE 0x04034b50
#define ZIP_MAX_FILEPATH 1024

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
    bool FileGroupZip::_initialize()
    {
        if( this->loadHeader_() == false )
        {
            LOGGER_ERROR( "can't load header '%s'"
                , m_folderPath.c_str()
            );

            return false;
        }

        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, false );

        m_mutex = mutex;

        uint32_t FileGroupZipReserveFiles = CONFIG_VALUE( "Engine", "FileGroupZipReserveFiles", 16 * 1024 );

        m_files.reserve( FileGroupZipReserveFiles );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileGroupZip::_finalize()
    {
        m_files.clear();
        m_indexes.clear();

        m_zipFile = nullptr;

        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::loadHeader_()
    {
        InputStreamInterfacePtr zipFile = Helper::openInputStreamFile( m_baseFileGroup, m_folderPath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( zipFile, false, "can't open input stream for path '%s'"
            , m_folderPath.c_str()
        );

        size_t zip_size = zipFile->size();
        zipFile->seek( zip_size - 22 );

        uint32_t header_position = (uint32_t)zipFile->tell();

        uint8_t endof_central_dir[22];

        if( zipFile->read( endof_central_dir, 22 ) != 22 )
        {
            LOGGER_ERROR( "invalid zip format '%s'"
                , m_folderPath.c_str()
            );

            return false;
        }

        MENGINE_ASSERTION_RETURN( s_get_uint32( endof_central_dir ) == 0x06054B50, false, "bad 'End of Central Dir' signature zip '%s'"
            , m_folderPath.c_str()
        );

        uint32_t header_size = s_get_uint32( endof_central_dir + 12 );
        uint32_t header_offset = s_get_uint32( endof_central_dir + 16 );
        uint32_t arc_offset = header_position - header_offset - header_size;

        header_offset += arc_offset;

        Char fileNameBuffer[ZIP_MAX_FILEPATH];

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

            FilePath filePath = Helper::stringizeFilePathSize( fileNameBuffer, header.fileNameLen );

            if( header.compressionMethod != Z_NO_COMPRESSION && header.compressionMethod != Z_DEFLATED )
            {
                LOGGER_ERROR( "zip '%s' file '%s' invalid compress method %d"
                    , m_folderPath.c_str()
                    , filePath.c_str()
                    , header.compressionMethod
                );

                return false;
            }

            FileInfo fi;

            fi.seek_pos = fileOffset;
            fi.file_size = header.compressedSize;
            fi.unz_size = header.uncompressedSize;
            fi.compr_method = header.compressionMethod;

            m_files.emplace( filePath, fi );
            m_indexes.push_back( filePath );

            zipFile->skip( header.compressedSize );
        }

        m_zipFile = zipFile;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::isPacked() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::getFullPath( const FilePath & _filePath, Char * _fullPath ) const
    {
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _fullPath );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::existFile( const FilePath & _filePath, bool _recursive ) const
    {
        MapFileInfo::const_iterator it_found = m_files.find( _filePath );

        bool result = (it_found != m_files.end());

        if( _recursive == true && result == false && m_parentFileGroup != nullptr )
        {
            result = m_parentFileGroup->existFile( _filePath, true );
        }

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::existDirectory( const FilePath & _folderName, bool _recursive ) const
    {
        MENGINE_UNUSED( _folderName );
        MENGINE_UNUSED( _recursive );

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
    static voidpf s_alloc_func( voidpf _opaque, uInt _items, uInt _size )
    {
        MENGINE_UNUSED( _opaque );

        uInt total = _items * _size;

        void * p = ALLOCATOR_SERVICE()
            ->malloc( total, "FileGroupZip" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_free_func( voidpf _opaque, voidpf _address )
    {
        MENGINE_UNUSED( _opaque );

        ALLOCATOR_SERVICE()
            ->free( _address, "FileGroupZip" );
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
    InputStreamInterfacePtr FileGroupZip::createInputFile( const FilePath & _filePath, bool _streaming, FileGroupInterface ** _fileGroup, const DocumentPtr & _doc )
    {
        if( m_parentFileGroup != nullptr )
        {
            if( this->existFile( _filePath, false ) == false )
            {
                InputStreamInterfacePtr stream = m_parentFileGroup->createInputFile( _filePath, _streaming, _fileGroup, _doc );

                return stream;
            }
        }

        MENGINE_ASSERTION_FATAL( m_files.find( _filePath ) != m_files.end(), "'%s' invalid found file '%s'"
            , this->getName().c_str()
            , _filePath.c_str() 
        );

        if( _streaming == true )
        {
            InputStreamInterfacePtr stream = m_baseFileGroup->createInputFile( _filePath, true, nullptr, _doc );

            if( _fileGroup != nullptr )
            {
                *_fileGroup = this;
            }

            return stream;
        }

        MemoryInputInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryInput( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

        if( _fileGroup != nullptr )
        {
            *_fileGroup = this;
        }

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, false, "zip '%s' file '%s' stream is NULL"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        MapFileInfo::const_iterator it_found = m_files.find( _filePath );

        MENGINE_ASSERTION_FATAL_RETURN( it_found != m_files.end(), false, "zip '%s' file '%s' not found"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        const FileInfo & fi = it_found->second;

        size_t file_offset = fi.seek_pos + _offset;
        size_t file_size = _size == 0 ? fi.file_size : _size;

        MENGINE_ASSERTION_FATAL_RETURN( _offset + file_size <= fi.file_size, false, "zip '%s' file '%s' invalid open range %d:%d (file size is low %d:%d)"
            , m_folderPath.c_str()
            , _filePath.c_str()
            , _offset
            , _size
            , fi.seek_pos
            , fi.file_size
        );

        if( _streaming == true )
        {
            MENGINE_ASSERTION_FATAL_RETURN( fi.compr_method == Z_NO_COMPRESSION, false, "zip '%s' file '%s' invalid open, not support compress + stream"
                , m_folderPath.c_str()
                , _filePath.c_str()
            );

            if( m_baseFileGroup->openInputFile( m_folderPath, _stream, file_offset, file_size, true, _share ) == false )
            {
                LOGGER_ERROR( "zip '%s' file '%s' invalid open range %d:%d"
                    , m_folderPath.c_str()
                    , _filePath.c_str()
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
            void * buffer = memory->newBuffer( fi.file_size );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, false, "zip '%s' file '%s' failed new memory %d"
                , m_folderPath.c_str()
                , _filePath.c_str()
                , fi.unz_size
            );

            m_mutex->lock();
            m_zipFile->seek( file_offset );
            m_zipFile->read( buffer, fi.file_size );
            m_mutex->unlock();
        }
        else
        {
            void * buffer = memory->newBuffer( fi.unz_size );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, false, "zip '%s' file '%s' failed new memory %d"
                , m_folderPath.c_str()
                , _filePath.c_str()
                , fi.unz_size
            );

            MemoryInterfacePtr compress_buffer = Helper::createMemoryCacheBuffer( fi.file_size, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( compress_buffer, false, "zip '%s' file '%s' failed cache memory %d"
                , m_folderPath.c_str()
                , _filePath.c_str()
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
                LOGGER_ERROR( "zip '%s' file '%s' failed inflate"
                    , m_folderPath.c_str()
                    , _filePath.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr FileGroupZip::createOutputFile( const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        LOGGER_ERROR( "unsupport method" );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream )
    {
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _stream );

        LOGGER_ERROR( "'%s' unsupport method"
            , _filePath.c_str()
        );

        return false;
    }
}
