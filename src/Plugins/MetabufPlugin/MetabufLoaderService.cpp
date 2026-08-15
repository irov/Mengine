#include "MetabufLoaderService.h"

#include "Interface/ArchiveServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Metacode/Metacache.h"
#include "Metacode/Metacode.h"

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
#include "Metacode/Metaprotocol.h"
#endif

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Data.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( MetabufLoaderService, Mengine::MetabufLoaderService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderService::MetabufLoaderService()
#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        : m_xmlMetaconvert( nullptr )
        , m_jsonMetaconvert( nullptr )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetabufLoaderService::~MetabufLoaderService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderService::_initializeService()
    {
#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        const Metacode::MetaprotocolGenerator generator;
        m_xmlMetaconvert = Metabuf::createXmlMetaconvert( &generator );

        if( m_xmlMetaconvert == nullptr )
        {
            return false;
        }

        m_jsonMetaconvert = Metabuf::createJsonMetaconvert( &generator );

        if( m_jsonMetaconvert == nullptr )
        {
            Metabuf::destroyMetaconvert( m_xmlMetaconvert );
            m_xmlMetaconvert = nullptr;

            return false;
        }
#endif

        LOGGER_MESSAGE( "Metacode version: %u protocol: %u"
            , Metacode::get_metacode_version()
            , Metacode::get_metacode_protocol_version()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetabufLoaderService::_finalizeService()
    {
        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "MetabufLoader" ) );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        Metabuf::destroyMetaconvert( m_xmlMetaconvert );
        m_xmlMetaconvert = nullptr;

        Metabuf::destroyMetaconvert( m_jsonMetaconvert );
        m_jsonMetaconvert = nullptr;
#endif

        MENGINE_ASSERTION_ALLOCATOR( "metabuf" );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderService::load( const ContentInterfacePtr & _content, Metabuf::Metaparse * _metadata, bool * const _exist, const DocumentInterfacePtr & _doc ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _metadata, "invalid Metabuf metadata" );

        const FilePath & filePath = _content->getFilePath();

        if( filePath.empty() == true )
        {
            LOGGER_ERROR( "invalid Metabuf content '%s' path is empty"
                , Helper::getContentFullPath( _content ).c_str()
            );

            return false;
        }

        if( _content->exist( true ) == false )
        {
            if( _exist != nullptr )
            {
                *_exist = false;
            }

            return false;
        }

        if( _exist != nullptr )
        {
            *_exist = true;
        }

        InputStreamInterfacePtr stream = _content->openInputStreamFile( false, false, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "invalid open Metabuf content '%s'"
            , Helper::getContentFullPath( _content ).c_str()
        );

        const uint32_t metaVersion = _metadata->getMetaVersion();

#if defined(MENGINE_MASTER_RELEASE_ENABLE)
        bool successful = this->importBin_( stream, _metadata, metaVersion, _doc );
#else
        bool successful = false;

        const ConstString extension = Helper::getFilePathExt( filePath );

        if( extension == STRINGIZE_STRING_LOCAL( "bin" ) )
        {
            successful = this->importBin_( stream, _metadata, metaVersion, _doc );
        }
        else if( extension == STRINGIZE_STRING_LOCAL( "xml" ) || extension == STRINGIZE_STRING_LOCAL( "json" ) )
        {
            MemoryInterfacePtr source = Helper::createMemoryCacheStream( stream, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( source, "invalid read Metabuf source '%s'"
                , Helper::getContentFullPath( _content ).c_str()
            );

            const void * sourceBuffer = source->getBuffer();
            const size_t sourceSize = source->getSize();

            Data raw;
            std::string error;
            const char * metaName = _metadata->getMetaName();
            const char * nodeName = _metadata->getNodeName();
            const Metabuf::MetaconvertInterface * metaconvert;

            if( extension == STRINGIZE_STRING_LOCAL( "json" ) )
            {
                metaconvert = m_jsonMetaconvert;
            }
            else
            {
                metaconvert = m_xmlMetaconvert;
            }

            const Metabuf::MetaInterface * meta = metaconvert->getMeta( metaName );

            if( meta == nullptr )
            {
                LOGGER_ERROR( "invalid Metabuf meta '%s' for source '%s'"
                    , metaName
                    , Helper::getContentFullPath( _content ).c_str()
                );

                _content->closeInputStreamFile( stream );

                return false;
            }

            const Metabuf::NodeInterface * node = meta->getNode( nodeName );

            if( node == nullptr )
            {
                LOGGER_ERROR( "invalid Metabuf node '%s' for meta '%s' source '%s'"
                    , nodeName
                    , metaName
                    , Helper::getContentFullPath( _content ).c_str()
                );

                _content->closeInputStreamFile( stream );

                return false;
            }

            if( metaconvert->convert( sourceBuffer, sourceSize, meta, node, raw, error ) == false )
            {
                LOGGER_ERROR( "invalid convert Metabuf source '%s': %s"
                    , Helper::getContentFullPath( _content ).c_str()
                    , error.c_str()
                );

                _content->closeInputStreamFile( stream );

                return false;
            }

            const void * rawBuffer = raw.data();
            const size_t rawSize = raw.size();

            successful = this->parseRaw_( rawBuffer, rawSize, _metadata );
        }
        else
        {
            LOGGER_ERROR( "unsupported Metabuf extension '%s' for '%s'"
                , extension.c_str()
                , Helper::getContentFullPath( _content ).c_str()
            );

            _content->closeInputStreamFile( stream );

            return false;
        }
#endif
        _content->closeInputStreamFile( stream );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderService::importBin_( const InputStreamInterfacePtr & _stream, Metabuf::Metaparse * _metadata, uint32_t _metaVersion, const DocumentInterfacePtr & _doc ) const
    {
        const size_t size = _stream->size();

        if( size < Metacode::header_size + sizeof( uint32_t ) * 2 )
        {
            LOGGER_ERROR( "invalid Metabuf BIN size [%zu]", size );

            return false;
        }

        uint8_t header_buff[Metacode::header_size];

        if( _stream->read( header_buff, Metacode::header_size ) != Metacode::header_size )
        {
            return false;
        }

        size_t header_read = 0;
        uint32_t readVersion;
        uint32_t needVersion;
        uint32_t readProtocol;
        uint32_t needProtocol;
        uint32_t readMetaVersion;

        Metabuf::HeaderError result = Metacode::readHeader( header_buff, Metacode::header_size, header_read, readVersion, needVersion, readProtocol, needProtocol, _metaVersion, readMetaVersion );

        if( result != Metabuf::HEADER_SUCCESSFUL )
        {
            LOGGER_ERROR( "error '%s' invalid version read [%u] need [%u] or protocol [%u] need [%u] meta [%u:%u]"
                , Metacode::getHeaderErrorMessage( result )
                , readVersion
                , needVersion
                , readProtocol
                , needProtocol
                , readMetaVersion
                , _metaVersion
            );

            return false;
        }

        uint32_t bin_size;
        uint32_t compress_size;

        if( _stream->read( &bin_size, sizeof( bin_size ) ) != sizeof( bin_size ) )
        {
            return false;
        }

        if( _stream->read( &compress_size, sizeof( compress_size ) ) != sizeof( compress_size ) )
        {
            return false;
        }

        if( compress_size > size - Metacode::header_size - sizeof( uint32_t ) * 2 )
        {
            LOGGER_ERROR( "invalid Metabuf compressed size [%u]", compress_size );

            return false;
        }

        MemoryInterfacePtr binary_buffer = Helper::createMemoryCacheBuffer( bin_size, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( binary_buffer, "invalid create Metabuf memory cache buffer" );

        void * binaryMemory = binary_buffer->getBuffer();

        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator, "invalid get lz4 archivator" );

        size_t uncompress_size;

        if( ARCHIVE_SERVICE()
            ->decompressStream( archivator, _stream, compress_size, binaryMemory, bin_size, &uncompress_size ) == false )
        {
            LOGGER_ERROR( "invalid uncompress Metabuf BIN" );

            return false;
        }

        if( uncompress_size != bin_size )
        {
            LOGGER_ERROR( "invalid uncompress Metabuf BIN [%zu:%u]", uncompress_size, bin_size );

            return false;
        }

        if( this->parseRaw_( binaryMemory, bin_size, _metadata ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetabufLoaderService::parseRaw_( const void * _buffer, size_t _size, Metabuf::Metaparse * _metadata ) const
    {
        size_t read_size = 0;
        const uint32_t internalStringsCount = Metacode::getInternalStringsCount();

        uint32_t stringCount;
        if( Metacode::readStrings( _buffer, _size, read_size, stringCount ) == false )
        {
            return false;
        }

        Metacache metacache;
        metacache.strings.resize( internalStringsCount + stringCount );

        uint32_t index = 0;

        for( ConstString & cstr : metacache.strings )
        {
            uint32_t stringSize;
            const char * stringBuffer;

            if( index < internalStringsCount )
            {
                stringBuffer = Metacode::getInternalString( index, stringSize );
            }
            else
            {
                stringBuffer = Metacode::readString( _buffer, _size, read_size, stringSize );
            }

            if( stringBuffer == nullptr )
            {
                return false;
            }

            STRINGIZE_SERVICE()
                ->stringize( stringBuffer, stringSize, MENGINE_STRINGIZE_UNKNOWN_HASH, &cstr );

            ++index;
        }

        if( read_size != _size && _metadata->parse( static_cast<const uint8_t *>(_buffer), _size, read_size, &metacache ) == false )
        {
            LOGGER_ERROR( "invalid Metabuf parse" );

            return false;
        }

        if( read_size != _size )
        {
            LOGGER_ERROR( "invalid Metabuf parse read [%zu] archive [%zu]", read_size, _size );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
