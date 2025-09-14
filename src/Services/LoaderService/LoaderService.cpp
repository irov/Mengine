#include "LoaderService.h"

#include "Interface/ConverterServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/FilePath.h"
#include "Kernel/PathHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/AllocatorHelper.h"
#include "Kernel/PathString.h"
#include "Kernel/Logger.h"
#include "Kernel/FileGroupHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/ParamsHelper.h"

#include "metabuf/Metadata.hpp"
#include "Metacode/Metacode.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( LoaderService, Mengine::LoaderService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderService::LoaderService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderService::~LoaderService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderService::_initializeService()
    {
        m_protocolPath = CONFIG_VALUE_FILEPATH( "Engine", "ProtocolPath", STRINGIZE_FILEPATH_LOCAL( "protocol.xml" ) );

        LOGGER_MESSAGE( "Metacode version: %u protocol: %u"
            , Metacode::get_metacode_version()
            , Metacode::get_metacode_protocol_version()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoaderService::_finalizeService()
    {
        m_loaders.clear();

        m_metacache.strings.clear();

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "MetabufLoader" ) );

        MENGINE_ASSERTION_ALLOCATOR( "metabuf" );
    }
    //////////////////////////////////////////////////////////////////////////
    void LoaderService::setProtocolPath( const FilePath & _protocolPath )
    {
        m_protocolPath = _protocolPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & LoaderService::getProtocolPath() const
    {
        return m_protocolPath;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderService::load( const ContentInterfacePtr & _content, Metabuf::Metaparse * _metadata, uint32_t _metaVersion, bool * const _exist, const DocumentInterfacePtr & _doc ) const
    {
        LOGGER_INFO( "loader", "load bin '%s'"
            , Helper::getContentFullPath( _content ).c_str()
        );

        const FileGroupInterfacePtr & fileGroup = _content->getFileGroup();
        const FilePath & filePath = _content->getFilePath();

        if( filePath.empty() == true )
        {
            LOGGER_ERROR( "invalid open bin '%s' path is empty"
                , Helper::getContentFullPath( _content ).c_str()
            );

            return false;
        }

        InputStreamInterfacePtr file_bin;
        if( this->openBin_( _content, &file_bin, _exist, _doc ) == false )
        {
            LOGGER_ERROR( "invalid open bin '%s'"
                , Helper::getContentFullPath( _content ).c_str()
            );

            return false;
        }

        MENGINE_ASSERTION_MEMORY_PANIC( file_bin, "invalid open bin '%s'"
            , Helper::getContentFullPath( _content ).c_str()
        );

        bool reimport = false;
        bool done = this->importBin_( file_bin, _metadata, _metaVersion, &reimport, _doc );

        _content->closeInputStreamFile( file_bin );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        if( reimport == true )
        {
            file_bin = nullptr;

            PathString cache_path_xml;
            cache_path_xml += filePath;
            cache_path_xml.replace_last( "xml" );

            FilePath c_cache_path_xml = Helper::stringizeFilePath( cache_path_xml );

            ContentInterfacePtr content_xml = Helper::makeFileContent( fileGroup, c_cache_path_xml, _doc );

            if( this->makeBin_( content_xml, filePath ) == false )
            {
                LOGGER_ERROR( "invalid rebuild bin '%s' from xml '%s'"
                    , Helper::getContentFullPath( _content ).c_str()
                    , Helper::getContentFullPath( content_xml ).c_str()
                );

                return false;
            }

            ContentInterfacePtr reimport_content_bin = Helper::makeFileContent( fileGroup, filePath, _doc );

            InputStreamInterfacePtr reimport_stream_bin = reimport_content_bin->openInputStreamFile( false, false, _doc );

            done = this->importBin_( reimport_stream_bin, _metadata, _metaVersion, nullptr, _doc );

            reimport_content_bin->closeInputStreamFile( reimport_stream_bin );
        }
#endif

        return done;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderService::validation( const ContentInterfacePtr & _content, uint32_t _metaVersion ) const
    {
        InputStreamInterfacePtr stream = _content->openInputStreamFile( false, false, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            return false;
        }

        size_t size = stream->size();

        if( size == 0 )
        {
            return false;
        }

        uint8_t header_buff[Metacode::header_size];
        stream->read( header_buff, Metacode::header_size );

        _content->closeInputStreamFile( stream );

        size_t header_read = 0;
        uint32_t readVersion;
        uint32_t needVersion;
        uint32_t readProtocol;
        uint32_t needProtocol;
        uint32_t metaMetaVersion = _metaVersion;
        uint32_t needMetaVersion;

        Metabuf::HeaderError result = Metacode::readHeader( header_buff, Metacode::header_size, header_read, readVersion, needVersion, readProtocol, needProtocol, metaMetaVersion, needMetaVersion );

        if( result != Metabuf::HEADER_SUCCESSFUL )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderService::importBin_( const InputStreamInterfacePtr & _stream, Metabuf::Metaparse * _metadata, uint32_t _metaVersion, bool * const _reimport, const DocumentInterfacePtr & _doc ) const
    {
        size_t size = _stream->size();

        if( size == 0 )
        {
            LOGGER_ERROR( "invalid size (empty)" );

            return false;
        }

        uint8_t header_buff[Metacode::header_size];
        _stream->read( header_buff, Metacode::header_size );

        size_t header_read = 0;
        uint32_t readVersion;
        uint32_t needVersion;
        uint32_t readProtocol;
        uint32_t needProtocol;
        uint32_t metaMetaVersion = _metaVersion;
        uint32_t needMetaVersion;

        Metabuf::HeaderError result = Metacode::readHeader( header_buff, Metacode::header_size, header_read, readVersion, needVersion, readProtocol, needProtocol, metaMetaVersion, needMetaVersion );

        if( result != Metabuf::HEADER_SUCCESSFUL )
        {
            if( _reimport == nullptr )
            {
                LOGGER_ERROR( "error '%s' invalid version read [%u] need [%u] or protocol [%u] need [%u] (Update you protocol file)"
                    , Metacode::getHeaderErrorMessage( result )
                    , readVersion
                    , needVersion
                    , readProtocol
                    , needProtocol
                );
            }
            else
            {
                LOGGER_INFO( "loader", "error '%s' invalid version read [%u] need [%u] or protocol [%u] need [%u] (Update you protocol file)"
                    , Metacode::getHeaderErrorMessage( result )
                    , readVersion
                    , needVersion
                    , readProtocol
                    , needProtocol
                );

                *_reimport = true;
            }

            return false;
        }

        uint32_t bin_size;
        _stream->read( &bin_size, sizeof( bin_size ) );

        uint32_t compress_size;
        _stream->read( &compress_size, sizeof( compress_size ) );

        MemoryInterfacePtr binary_buffer = Helper::createMemoryCacheBuffer( bin_size, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( binary_buffer, "invalid create memory cache buffer" );

        uint8_t * binary_memory = binary_buffer->getBuffer();

        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator, "invalid get archivator" );

        size_t uncompress_size;
        if( ARCHIVE_SERVICE()
            ->decompressStream( archivator, _stream, compress_size, binary_memory, bin_size, &uncompress_size ) == false )
        {
            if( _reimport == nullptr )
            {
                LOGGER_ERROR( "invalid uncompress" );
            }
            else
            {
                *_reimport = true;
            }

            return false;
        }

        size_t read_size = 0;

        uint32_t internalStringsCount = Metacode::getInternalStringsCount();

        uint32_t stringCount;
        if( Metacode::readStrings( binary_memory, bin_size, read_size, stringCount ) == false )
        {
            return false;
        }

        m_metacache.strings.resize( internalStringsCount + stringCount );

        uint32_t index = 0;

        for( ConstString & cstr : m_metacache.strings )
        {
            uint32_t stringSize;
            int64_t stringHash;
            const char * stringBuffer = nullptr;

            if( index < internalStringsCount )
            {
                stringBuffer = Metacode::getInternalString( index, stringSize, stringHash );
            }
            else
            {
                stringBuffer = Metacode::readString( binary_memory, bin_size, read_size, stringSize, stringHash );
            }

            MENGINE_ASSERTION_MEMORY_PANIC( stringBuffer, "invalid read string (error)" );

            STRINGIZE_SERVICE()
                ->stringize( stringBuffer, stringSize, stringHash, &cstr );

            ++index;
        }

        if( read_size != bin_size )
        {
            if( _metadata->parse( binary_memory, bin_size, read_size, (void *)&m_metacache ) == false )
            {
                LOGGER_ERROR( "invalid parse (error)" );

                return false;
            }
        }

        m_metacache.strings.clear();

        if( read_size != bin_size )
        {
            LOGGER_ERROR( "invalid parse (read != archive)" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_MASTER_RELEASE_DISABLE)
    //////////////////////////////////////////////////////////////////////////
    bool LoaderService::openBin_( const ContentInterfacePtr & _content, InputStreamInterfacePtr * const _stream, bool * const _exist, const DocumentInterfacePtr & _doc ) const
    {
        const FileGroupInterfacePtr & fileGroup = _content->getFileGroup();
        const FilePath & filePath = _content->getFilePath();

        PathString cache_path_xml;
        cache_path_xml += filePath;
        cache_path_xml.replace_last( "xml" );

        FilePath c_cache_path_xml = Helper::stringizeFilePath( cache_path_xml );

        if( fileGroup->existFile( c_cache_path_xml, true ) == false )
        {
            if( _content->exist( true ) == false )
            {
                *_exist = false;

                return false;
            }

            InputStreamInterfacePtr file_bin = _content->openInputStreamFile( false, false, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( file_bin, "invalid open bin '%s'"
                , Helper::getContentFullPath( _content ).c_str()
            );

            *_stream = file_bin;

            return true;
        }

        *_exist = true;

        if( _content->exist( true ) == false )
        {
            ContentInterfacePtr content_xml = Helper::makeFileContent( fileGroup, c_cache_path_xml, _doc );

            if( this->makeBin_( content_xml, filePath ) == false )
            {
                return false;
            }
        }

        InputStreamInterfacePtr file_bin = Helper::openInputStreamFile( fileGroup, filePath, false, false, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( file_bin, "invalid open bin '%s'"
            , Helper::getContentFullPath( _content ).c_str()
        );

        InputStreamInterfacePtr file_xml = Helper::openInputStreamFile( fileGroup, c_cache_path_xml, false, false, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( file_xml, "invalid open xml '%s'"
            , Helper::getContentFullPath( _content ).c_str()
        );

        uint64_t time_xml;
        file_xml->time( &time_xml );

        uint64_t time_bin;
        file_bin->time( &time_bin );

        file_xml = nullptr;

        if( time_xml > time_bin )
        {
            //Rebuild bin file from xml
            file_bin = nullptr;

            ContentInterfacePtr content_xml = Helper::makeFileContent( fileGroup, c_cache_path_xml, _doc );

            if( this->makeBin_( content_xml, filePath ) == false )
            {
                *_stream = nullptr;

                return false;
            }

            file_bin = Helper::openInputStreamFile( fileGroup, filePath, false, false, _doc );
        }

        *_stream = file_bin;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderService::makeBin_( const ContentInterfacePtr & _content, const FilePath & _pathBin ) const
    {
        ConverterInterfacePtr converter = CONVERTER_SERVICE()
            ->createConverter( STRINGIZE_STRING_LOCAL( "xml2bin" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( converter, "invalid create converter xml2bin for '%s'"
            , Helper::getContentFullPath( _content ).c_str()
        );

        ConverterOptions options;

        const FileGroupInterfacePtr & fileGroup = _content->getFileGroup();

        options.inputContent = _content;
        options.outputContent = Helper::makeFileContent( fileGroup, _pathBin, MENGINE_DOCUMENT_FUNCTION );

        FileGroupInterfacePtr fileGroupDev = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( fileGroupDev, "not found 'dev' file group" );

        Mengine::ContentInterfacePtr protocolContent = Mengine::Helper::makeFileContent( fileGroupDev, m_protocolPath, MENGINE_DOCUMENT_FUNCTION );

        uint32_t useProtocolVersion = Metacode::get_metacode_protocol_version();
        uint32_t useProtocolCrc32 = Metacode::get_metacode_protocol_crc32();

        Helper::setParam( options.params, STRINGIZE_STRING_LOCAL( "protocolContent" ), protocolContent );
        Helper::setParam( options.params, STRINGIZE_STRING_LOCAL( "useProtocolVersion" ), (ParamInteger)useProtocolVersion );
        Helper::setParam( options.params, STRINGIZE_STRING_LOCAL( "useProtocolCrc32" ), (ParamInteger)useProtocolCrc32 );

        converter->setOptions( options );

        if( converter->convert() == false )
        {
            LOGGER_ERROR( "invalid make bin for '%s'"
                , Helper::getContentFullPath( _content ).c_str()
            );

            return false;
        }

        return true;
    }
#else
    //////////////////////////////////////////////////////////////////////////
    bool LoaderService::openBin_( const ContentInterfacePtr & _content, InputStreamInterfacePtr * const _stream, bool * const _exist, const DocumentInterfacePtr & _doc ) const
    {
        if( _content->exist( true ) == false )
        {
            *_exist = false;

            return false;
        }

        InputStreamInterfacePtr file_bin = _content->openInputStreamFile( false, false, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( file_bin, "invalid open bin '%s'"
            , Helper::getContentFullPath( _content ).c_str()
        );

        *_stream = file_bin;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
#endif
}
