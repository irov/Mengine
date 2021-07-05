#include "FileGroupZip.h"

#include "Interface/ServiceInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

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
#include "Kernel/AllocatorHelper.h"
#include "Kernel/PathString.h"

#include "zlib.h"

//////////////////////////////////////////////////////////////////////////
#define ZIP_LOCAL_HEADER_SIGNATURE (0x04034b50)
#define ZIP_CENTRAL_HEADER_SIGNATURE (0x02014b50)
#define ZIP_END_HEADER_SIGNATURE (0x06054b50)
#define ZIP_END_HEADER_SIGNATURE_64 (0x06064b50)
#define ZIP_END_LOCAL_HEADER_SIGNATURE_64 (0x07064b50)
#define ZIP_MAX_FILEPATH 1024
#define ZIP_END_CENTRAL_DIR_SIZE 22
#define ZIP_LOCAL_HEADER_SIZE 30
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
#pragma pack( push , 1 )
    //////////////////////////////////////////////////////////////////////////
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
    struct ZipLocalFileHeader
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
    };
    //////////////////////////////////////////////////////////////////////////
    struct ZipEndOfCentralDirectoryFileHeader
    {
        uint16_t numberOfThisDisk;
        uint16_t diskWhereCentralDirectoryStarts;
        uint16_t numberOfCentralDirectoryRecordsOnThisDisk;
        uint16_t totalNumberOfCentralDirectoryRecords;
        uint32_t sizeOfCentralDirectoryBytes;
        uint32_t offsetOfStartOfCentralDirectoryRelativeToStartOfArchive;
        uint16_t commentLength;
    };
    //////////////////////////////////////////////////////////////////////////
#pragma pack( pop )
    //////////////////////////////////////////////////////////////////////////    
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static voidpf zip_alloc_func( voidpf _opaque, uInt _items, uInt _size )
        {
            MENGINE_UNUSED( _opaque );

            uInt total = _items * _size;

            void * p = Helper::allocateMemory( total, "zip" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void zip_free_func( voidpf _opaque, voidpf _address )
        {
            MENGINE_UNUSED( _opaque );

            Helper::deallocateMemory( _address, "zip" );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool zip_inflate_memory( void * const _buffer, size_t _capacity, z_const void * _src, size_t _size )
        {
            z_stream zs;
            zs.next_in = static_cast<z_const Bytef *>(_src);
            zs.avail_in = (uInt)_size;

            zs.next_out = static_cast<Bytef *>(_buffer);
            zs.avail_out = (uInt)_capacity;

            zs.zalloc = &zip_alloc_func;
            zs.zfree = &zip_free_func;

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
    }
    //////////////////////////////////////////////////////////////////////////
    FileGroupZip::FileGroupZip()
        : m_mappedThreshold( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FileGroupZip::~FileGroupZip()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::_initialize()
    {
        if( m_baseFileGroup->isAvailableMappedFile() == true )
        {
            m_mappedThreshold = CONFIG_VALUE( "Engine", "ZipMappedThreshold", 262144U );

            FileGroupInterface * mappedFileGroup;
            FileMappedInterfacePtr mappedFile = m_baseFileGroup->createMappedFile( m_folderPath, &mappedFileGroup, MENGINE_DOCUMENT_FACTORABLE );

            if( mappedFileGroup->openMappedFile( m_folderPath, mappedFile, false ) == false )
            {
                return false;
            }

            m_mappedFile = mappedFile;
        }

        if( this->loadHeader_() == false )
        {
            LOGGER_ERROR( "can't load header '%s'"
                , m_folderPath.c_str()
            );

            return false;
        }

        ThreadMutexInterfacePtr mutex = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

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
        m_mappedFile = nullptr;

        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::loadHeader_()
    {
        InputStreamInterfacePtr zipFile = Helper::openInputStreamFile( m_baseFileGroup, m_folderPath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( zipFile, "can't open input stream for path '%s'"
            , m_folderPath.c_str()
        );

        if( zipFile->rseek( ZIP_END_CENTRAL_DIR_SIZE ) == false )
        {
            return false;
        }

        uint32_t eocd_signature;
        zipFile->read( &eocd_signature, sizeof( eocd_signature ) );

        MENGINE_ASSERTION_FATAL( eocd_signature == ZIP_END_HEADER_SIGNATURE, "bad 'End of Central Dir' signature zip '%s'"
            , m_folderPath.c_str()
        );

        ZipEndOfCentralDirectoryFileHeader eocd;
        zipFile->read( &eocd, sizeof( eocd ) );

        MENGINE_ASSERTION_FATAL( eocd.commentLength == 0 );
        MENGINE_ASSERTION_FATAL( eocd.offsetOfStartOfCentralDirectoryRelativeToStartOfArchive != 0xffffffffU );

        size_t header_offset = (size_t)eocd.offsetOfStartOfCentralDirectoryRelativeToStartOfArchive;

        if( zipFile->seek( header_offset ) == false )
        {
            return false;
        }

        Char fileNameBuffer[ZIP_MAX_FILEPATH] = {'\0'};

        for( ;;)
        {
            uint32_t signature;
            zipFile->read( &signature, sizeof( signature ) );

            if( signature == ZIP_END_HEADER_SIGNATURE )
            {
                break;
            }
            else if( signature == ZIP_END_HEADER_SIGNATURE_64 )
            {
                break;
            }

            MENGINE_ASSERTION_FATAL( signature == ZIP_CENTRAL_HEADER_SIGNATURE, "unknown zlib header signature [%#010x]"
                , signature
            );

            ZipCentralDirectoryFileHeader header;
            zipFile->read( &header, sizeof( header ) );

            zipFile->read( &fileNameBuffer, header.fileNameLen );

            uint64_t uncompressedSize = header.uncompressedSize;
            uint64_t compressedSize = header.compressedSize;
            uint64_t relativeOffset = header.relativeOffset;

            if( header.extraFieldLen != 0 )
            {
                uint16_t headerID;
                uint16_t dataSize;

                zipFile->read( &headerID, sizeof( headerID ) );
                zipFile->read( &dataSize, sizeof( dataSize ) );

                if( header.uncompressedSize == 0xffffffffU )
                {
                    uint64_t dataValue;
                    zipFile->read( &dataValue, sizeof( dataValue ) );

                    uncompressedSize = dataValue;
                }

                if( header.compressedSize == 0xffffffffU )
                {
                    uint64_t dataValue;
                    zipFile->read( &dataValue, sizeof( dataValue ) );

                    compressedSize = dataValue;
                }

                if( header.relativeOffset == 0xffffffffU )
                {
                    uint64_t dataValue;
                    zipFile->read( &dataValue, sizeof( dataValue ) );

                    relativeOffset = dataValue;
                }
            }

            zipFile->skip( header.commentLen );

            FilePath filePath = Helper::stringizeFilePathSize( fileNameBuffer, header.fileNameLen );

            MENGINE_ASSERTION_FATAL( header.compressionMethod == Z_NO_COMPRESSION || header.compressionMethod == Z_DEFLATED, "zip '%s' file '%s' invalid compress method [%u]"
                , m_folderPath.c_str()
                , filePath.c_str()
                , header.compressionMethod
            );

            FileInfo fi;
            fi.seek_pos = (size_t)relativeOffset + ZIP_LOCAL_HEADER_SIZE + header.fileNameLen + header.commentLen;
            fi.file_size = (size_t)compressedSize;
            fi.unz_size = (size_t)uncompressedSize;
            fi.compr_method = header.compressionMethod;

            m_files.emplace( filePath, fi );
            m_indexes.push_back( filePath );
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
    bool FileGroupZip::getFullPath( const FilePath & _filePath, Char * const _fullPath ) const
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
    bool FileGroupZip::removeFile( const FilePath & _filePath ) const
    {
        MENGINE_UNUSED( _filePath );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
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
    bool FileGroupZip::moveFile( const FilePath & _oldFilePath, const FilePath & _newFilePath ) const
    {
        MENGINE_UNUSED( _oldFilePath );
        MENGINE_UNUSED( _newFilePath );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static int32_t wildcards_mask( const Char * string, const Char * mask, int endofmask = 1 )
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
                            int32_t match = Detail::wildcards_mask( string, mask, endofmask );

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

            if( Detail::wildcards_mask( path_str, mask_str ) == 0 )
            {
                _lambda( path );
            }
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr FileGroupZip::createInputFile( const FilePath & _filePath, bool _streaming, FileGroupInterface ** const _fileGroup, const DocumentPtr & _doc )
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

        if( _fileGroup != nullptr )
        {
            *_fileGroup = this;
        }

        if( _streaming == true )
        {
            if( m_mappedFile != nullptr )
            {
                InputStreamInterfacePtr stream = m_mappedFile->createInputStream( _doc );

                MENGINE_ASSERTION_MEMORY_PANIC( stream );

                return stream;
            }
            else
            {
                InputStreamInterfacePtr stream = m_baseFileGroup->createInputFile( _filePath, true, nullptr, _doc );

                MENGINE_ASSERTION_MEMORY_PANIC( stream );

                return stream;
            }
        }

        MapFileInfo::const_iterator it_found = m_files.find( _filePath );

        const FileInfo & fi = it_found->second;

        if( fi.file_size < m_mappedThreshold || fi.compr_method != Z_NO_COMPRESSION || m_mappedFile == nullptr )
        {
            MemoryInputInterfacePtr memory = MEMORY_SERVICE()
                ->createMemoryInput( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory );

            return memory;
        }

        InputStreamInterfacePtr stream = m_mappedFile->createInputStream( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream );

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "zip '%s' file '%s' stream is nullptr"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        MapFileInfo::const_iterator it_found = m_files.find( _filePath );

        MENGINE_ASSERTION_FATAL( it_found != m_files.end(), "zip '%s' file '%s' not found"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        const FileInfo & fi = it_found->second;

        size_t file_offset = fi.seek_pos + _offset;
        size_t file_size = _size == ~0U ? fi.file_size : _size;

        MENGINE_ASSERTION_FATAL( _offset + file_size <= fi.file_size, "zip '%s' file '%s' invalid open range %zu:%zu (file size is low %zu:%zu)"
            , m_folderPath.c_str()
            , _filePath.c_str()
            , _offset
            , _size
            , fi.seek_pos
            , fi.file_size
        );

        if( _streaming == true )
        {
            MENGINE_ASSERTION_FATAL( fi.compr_method == Z_NO_COMPRESSION, "zip '%s' file '%s' invalid open, not support compress + stream"
                , m_folderPath.c_str()
                , _filePath.c_str()
            );

            if( m_mappedFile != nullptr )
            {
                if( m_mappedFile->openInputStream( _stream, file_offset, fi.file_size ) == false )
                {
                    return false;
                }
            }
            else
            {
                if( m_baseFileGroup->openInputFile( m_folderPath, _stream, file_offset, file_size, true, _share ) == false )
                {
                    LOGGER_ERROR( "zip '%s' file '%s' invalid open range %zu:%zu"
                        , m_folderPath.c_str()
                        , _filePath.c_str()
                        , fi.seek_pos
                        , fi.file_size
                    );

                    return false;
                }
            }

            return true;
        }

        if( fi.compr_method == Z_NO_COMPRESSION )
        {
            if( fi.file_size < m_mappedThreshold || m_mappedFile == nullptr )
            {
                MemoryInputInterface * memory = stdex::intrusive_get<MemoryInputInterface *>( _stream );

                void * buffer = memory->newBuffer( fi.file_size );

                MENGINE_ASSERTION_MEMORY_PANIC( buffer, "zip '%s' file '%s' failed new memory %zu"
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
                if( m_mappedFile->openInputStream( _stream, file_offset, fi.file_size ) == false )
                {
                    return false;
                }
            }
        }
        else
        {
            MemoryInputInterface * memory = stdex::intrusive_get<MemoryInputInterface *>( _stream );

            void * buffer = memory->newBuffer( fi.unz_size );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, "zip '%s' file '%s' failed new memory %zu"
                , m_folderPath.c_str()
                , _filePath.c_str()
                , fi.unz_size
            );

            MemoryInterfacePtr compress_buffer = Helper::createMemoryCacheBuffer( fi.file_size, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( compress_buffer, "zip '%s' file '%s' failed cache memory %zu"
                , m_folderPath.c_str()
                , _filePath.c_str()
                , fi.file_size
            );

            void * compress_memory = compress_buffer->getBuffer();

            MENGINE_ASSERTION_MEMORY_PANIC( compress_memory );

            m_mutex->lock();
            m_zipFile->seek( file_offset );
            m_zipFile->read( compress_memory, fi.file_size );
            m_mutex->unlock();

            if( Detail::zip_inflate_memory( buffer, fi.unz_size, compress_memory, fi.file_size ) == false )
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
    bool FileGroupZip::closeInputFile( const InputStreamInterfacePtr & _stream )
    {
        MENGINE_UNUSED( _stream );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr FileGroupZip::createOutputFile( const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream )
    {
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _stream );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::closeOutputFile( const OutputStreamInterfacePtr & _stream )
    {
        MENGINE_UNUSED( _stream );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::isAvailableMappedFile() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    MappedInterfacePtr FileGroupZip::createMappedFile( const FilePath & _filePath, FileGroupInterface ** const _fileGroup, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _fileGroup );
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::openMappedFile( const FilePath & _filePath, const MappedInterfacePtr & _stream, bool _shared )
    {
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _stream );
        MENGINE_UNUSED( _shared );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::closeMappedFile( const MappedInterfacePtr & _stream )
    {
        MENGINE_UNUSED( _stream );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
