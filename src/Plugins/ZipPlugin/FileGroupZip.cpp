#include "FileGroupZip.h"

#include "Interface/ServiceInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ThreadSystemInterface.h"

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
#include "Kernel/StringFormat.h"
#include "Kernel/ConfigHelper.h"

#include "Config/StdIO.h"

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
    namespace Detail
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
    }
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

            zs.zalloc = &Detail::zip_alloc_func;
            zs.zfree = &Detail::zip_free_func;

            int32_t err_init = ::inflateInit2( &zs, -MAX_WBITS );

            if( err_init != Z_OK )
            {
                return false;
            }

            int32_t err_inflate = ::inflate( &zs, Z_FINISH );
            int32_t err_end = ::inflateEnd( &zs );

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
        uint32_t ZipPlugin_ReserveFiles = CONFIG_VALUE( "ZipPlugin", "ReserveFiles", 16 * 1024 );

        m_files.reserve( ZipPlugin_ReserveFiles );
        m_indexes.reserve( ZipPlugin_ReserveFiles );

        uint32_t ZipPlugin_MappedThreshold = CONFIG_VALUE( "ZipPlugin", "MappedThreshold", 262144U );

        m_mappedThreshold = ZipPlugin_MappedThreshold;

        if( this->loadHeaders_() == false )
        {
            LOGGER_ERROR( "can't load header '%s'"
                , m_folderPath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileGroupZip::_finalize()
    {
        m_files.clear();
        m_zips.clear();
        m_indexes.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::loadHeaders_()
    {
        if( Helper::getStringFormatExpectedArgs( m_folderPath.c_str(), m_folderPath.size() ) == 0 )
        {
            if( this->loadHeader_( m_folderPath ) == false )
            {
                return false;
            }
        }
        else
        {
            uint32_t enumerator = 0;
            for( ;; ++enumerator )
            {
                Char folderPathSplit[MENGINE_MAX_PATH] = {'\0'};
                int32_t size_sprintf = MENGINE_SNPRINTF( folderPathSplit, MENGINE_MAX_PATH, m_folderPath.c_str(), enumerator );

                if( size_sprintf < 0 )
                {
                    return false;
                }

                FilePath folderPathSplit_fp = Helper::stringizeFilePath( folderPathSplit );

                if( m_baseFileGroup->existFile( folderPathSplit_fp, false ) == false )
                {
                    break;
                }

                if( this->loadHeader_( folderPathSplit_fp ) == false )
                {
                    return false;
                }
            }

            if( enumerator == 0 )
            {
                LOGGER_ERROR( "format path '%s' can't find any zip"
                    , m_folderPath.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::loadMappedFile( const FilePath & _folderPath, FileMappedInterfacePtr * const _mappedFile ) const
    {
        if( m_baseFileGroup->isAvailableMappedFile() == false )
        {
            *_mappedFile = nullptr;

            return true;
        }

        FileGroupInterface * mappedFileGroup;
        FileMappedInterfacePtr mappedFile = m_baseFileGroup->createMappedFile( _folderPath, &mappedFileGroup, MENGINE_DOCUMENT_FACTORABLE );

        if( mappedFileGroup->openMappedFile( _folderPath, mappedFile, false ) == false )
        {
            return false;
        }

        *_mappedFile = mappedFile;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::loadHeader_( const FilePath & _folderPath )
    {
        FileMappedInterfacePtr mappedFile;
        if( this->loadMappedFile( _folderPath, &mappedFile ) == false )
        {
            return false;
        }

        ThreadMutexInterfacePtr mutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        ThreadMutexInterfacePtr proxyMutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( proxyMutex );

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( m_baseFileGroup, _folderPath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "can't open input stream for path '%s'"
            , _folderPath.c_str()
        );

        InputStreamInterfacePtr proxyStream = Helper::openInputStreamFile( m_baseFileGroup, _folderPath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( proxyStream, "can't open input mutex stream for path '%s'"
            , _folderPath.c_str()
        );

        if( stream->rseek( ZIP_END_CENTRAL_DIR_SIZE ) == false )
        {
            return false;
        }

        ZipInfo zinfo;
        zinfo.folderPath = _folderPath;
        zinfo.mappedFile = mappedFile;
        zinfo.stream = stream;
        zinfo.proxyStream = proxyStream;
        zinfo.mutex = mutex;
        zinfo.proxyMutex = proxyMutex;

        const ZipInfo & pzinfo = m_zips.emplace_back( zinfo );

        uint32_t eocd_signature;
        stream->read( &eocd_signature, sizeof( eocd_signature ) );

        MENGINE_ASSERTION_FATAL( eocd_signature == ZIP_END_HEADER_SIGNATURE, "bad 'End of Central Dir' signature zip '%s'"
            , _folderPath.c_str()
        );

        Detail::ZipEndOfCentralDirectoryFileHeader eocd;
        stream->read( &eocd, sizeof( eocd ) );

        MENGINE_ASSERTION_FATAL( eocd.commentLength == 0 );
        MENGINE_ASSERTION_FATAL( eocd.offsetOfStartOfCentralDirectoryRelativeToStartOfArchive != 0xffffffffU );

        size_t header_offset = (size_t)eocd.offsetOfStartOfCentralDirectoryRelativeToStartOfArchive;

        if( stream->seek( header_offset ) == false )
        {
            return false;
        }

        Char fileNameBuffer[ZIP_MAX_FILEPATH] = {'\0'};

        for( ;;)
        {
            uint32_t signature;
            stream->read( &signature, sizeof( signature ) );

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

            Detail::ZipCentralDirectoryFileHeader header;
            stream->read( &header, sizeof( header ) );

            stream->read( &fileNameBuffer, header.fileNameLen );

            uint64_t uncompressedSize = header.uncompressedSize;
            uint64_t compressedSize = header.compressedSize;
            uint64_t relativeOffset = header.relativeOffset;

            if( header.extraFieldLen != 0 )
            {
                uint16_t headerID;
                uint16_t dataSize;

                stream->read( &headerID, sizeof( headerID ) );
                stream->read( &dataSize, sizeof( dataSize ) );

                if( header.uncompressedSize == 0xffffffffU )
                {
                    uint64_t dataValue;
                    stream->read( &dataValue, sizeof( dataValue ) );

                    uncompressedSize = dataValue;
                }

                if( header.compressedSize == 0xffffffffU )
                {
                    uint64_t dataValue;
                    stream->read( &dataValue, sizeof( dataValue ) );

                    compressedSize = dataValue;
                }

                if( header.relativeOffset == 0xffffffffU )
                {
                    uint64_t dataValue;
                    stream->read( &dataValue, sizeof( dataValue ) );

                    relativeOffset = dataValue;
                }
            }

            stream->skip( header.commentLen );

            FilePath filePath = Helper::stringizeFilePathSize( fileNameBuffer, header.fileNameLen );

            MENGINE_ASSERTION_FATAL( header.compressionMethod == Z_NO_COMPRESSION || header.compressionMethod == Z_DEFLATED, "zip '%s' file '%s' invalid compress method [%u]"
                , _folderPath.c_str()
                , filePath.c_str()
                , header.compressionMethod
            );

            FileInfo fi;
            fi.seek_pos = (size_t)relativeOffset + ZIP_LOCAL_HEADER_SIZE + header.fileNameLen + header.commentLen;
            fi.file_size = (size_t)compressedSize;
            fi.unz_size = (size_t)uncompressedSize;
            fi.compr_method = header.compressionMethod;
            fi.zip = &pzinfo;

            m_files.emplace( filePath, fi );
            m_indexes.push_back( filePath );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::isPacked() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FileGroupZip::getFullPath( const FilePath & _filePath, Char * const _fullPath ) const
    {
        MENGINE_SNPRINTF( _fullPath, MENGINE_MAX_PATH, "%s/%s"
            , this->getFolderPath().c_str()
            , _filePath.c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::existFile( const FilePath & _filePath, bool _recursive ) const
    {
        UnorderedMapFileInfo::const_iterator it_found = m_files.find( _filePath );

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

        if( _fileGroup != nullptr )
        {
            *_fileGroup = this;
        }

        UnorderedMapFileInfo::const_iterator it_found = m_files.find( _filePath );

        MENGINE_ASSERTION_FATAL( it_found != m_files.end(), "'%s' invalid found file '%s'"
            , this->getName().c_str()
            , _filePath.c_str()
        );

        const FileInfo & fi = it_found->second;

        if( _streaming == true )
        {
            if( fi.zip->mappedFile != nullptr )
            {
                InputStreamInterfacePtr stream = fi.zip->mappedFile->createInputStream( _doc );

                MENGINE_ASSERTION_MEMORY_PANIC( stream );

                return stream;
            }
            else
            {
                InputStreamInterfacePtr stream = m_baseFileGroup->createInputMutexFile( _filePath, fi.zip->proxyStream, fi.zip->proxyMutex, nullptr, _doc );

                MENGINE_ASSERTION_MEMORY_PANIC( stream );

                return stream;
            }
        }

        if( fi.file_size < m_mappedThreshold || fi.compr_method != Z_NO_COMPRESSION || fi.zip->mappedFile == nullptr )
        {
            MemoryInputInterfacePtr memory = MEMORY_SERVICE()
                ->createMemoryInput( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory );

            return memory;
        }

        InputStreamInterfacePtr stream = fi.zip->mappedFile->createInputStream( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream );

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        MENGINE_UNUSED( _share );

        MENGINE_ASSERTION_MEMORY_PANIC( _stream, "zip '%s' file '%s' stream is nullptr"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        UnorderedMapFileInfo::const_iterator it_found = m_files.find( _filePath );

        MENGINE_ASSERTION_FATAL( it_found != m_files.end(), "zip '%s' file '%s' not found"
            , m_folderPath.c_str()
            , _filePath.c_str()
        );

        const FileInfo & fi = it_found->second;

        size_t file_offset = fi.seek_pos + _offset;
        size_t file_size = _size == MENGINE_UNKNOWN_SIZE ? fi.file_size : _size;

        MENGINE_ASSERTION_FATAL( _offset + file_size <= fi.file_size, "zip '%s' file '%s' invalid open range %zu:%zu (file size is low %zu:%zu)"
            , fi.zip->folderPath.c_str()
            , _filePath.c_str()
            , _offset
            , _size
            , fi.seek_pos
            , fi.file_size
        );

        if( _streaming == true )
        {
            MENGINE_ASSERTION_FATAL( fi.compr_method == Z_NO_COMPRESSION, "zip '%s' file '%s' invalid open, not support compress + stream"
                , fi.zip->folderPath.c_str()
                , _filePath.c_str()
            );

            if( fi.zip->mappedFile != nullptr )
            {
                if( fi.zip->mappedFile->openInputStream( _stream, file_offset, fi.file_size ) == false )
                {
                    return false;
                }
            }
            else
            {
                if( m_baseFileGroup->openInputMutexFile( fi.zip->folderPath, _stream, file_offset, file_size ) == false )
                {
                    LOGGER_ERROR( "zip '%s' file '%s' invalid open range %zu:%zu"
                        , fi.zip->folderPath.c_str()
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
            if( fi.file_size < m_mappedThreshold || fi.zip->mappedFile == nullptr )
            {
                MemoryInputInterface * memory = _stream.getT<MemoryInputInterface *>();

                void * buffer = memory->newBuffer( fi.file_size );

                MENGINE_ASSERTION_MEMORY_PANIC( buffer, "zip '%s' file '%s' failed new memory %zu"
                    , fi.zip->folderPath.c_str()
                    , _filePath.c_str()
                    , fi.unz_size
                );

                fi.zip->mutex->lock();
                fi.zip->stream->seek( file_offset );
                fi.zip->stream->read( buffer, fi.file_size );
                fi.zip->mutex->unlock();
            }
            else
            {
                if( fi.zip->mappedFile->openInputStream( _stream, file_offset, fi.file_size ) == false )
                {
                    return false;
                }
            }
        }
        else
        {
            MemoryInputInterface * memory = _stream.getT<MemoryInputInterface *>();

            void * buffer = memory->newBuffer( fi.unz_size );

            MENGINE_ASSERTION_MEMORY_PANIC( buffer, "zip '%s' file '%s' failed new memory %zu"
                , fi.zip->folderPath.c_str()
                , _filePath.c_str()
                , fi.unz_size
            );

            MemoryInterfacePtr compress_buffer = Helper::createMemoryCacheBuffer( fi.file_size, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( compress_buffer, "zip '%s' file '%s' failed cache memory %zu"
                , fi.zip->folderPath.c_str()
                , _filePath.c_str()
                , fi.file_size
            );

            void * compress_memory = compress_buffer->getBuffer();

            MENGINE_ASSERTION_MEMORY_PANIC( compress_memory );

            fi.zip->mutex->lock();
            fi.zip->stream->seek( file_offset );
            fi.zip->stream->read( compress_memory, fi.file_size );
            fi.zip->mutex->unlock();

            if( Detail::zip_inflate_memory( buffer, fi.unz_size, compress_memory, fi.file_size ) == false )
            {
                LOGGER_ERROR( "zip '%s' file '%s' failed inflate"
                    , fi.zip->folderPath.c_str()
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
    InputStreamInterfacePtr FileGroupZip::createInputMutexFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, const ThreadMutexInterfacePtr & _mutex, FileGroupInterface ** const _fileGroup, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _stream );
        MENGINE_UNUSED( _mutex );
        MENGINE_UNUSED( _fileGroup );
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::openInputMutexFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size )
    {
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _stream );
        MENGINE_UNUSED( _offset );
        MENGINE_UNUSED( _size );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::closeInputMutexFile( const InputStreamInterfacePtr & _stream )
    {
        MENGINE_UNUSED( _stream );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr FileGroupZip::createOutputFile( const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FileGroupZip::openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream, bool _withTemp )
    {
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _stream );
        MENGINE_UNUSED( _withTemp );

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
