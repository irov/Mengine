#include "LoaderService.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/FilePath.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "metabuf/Metadata.hpp"
#include "Metacode/Metacode.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#ifndef MENGINE_MASTER_RELEASE
#   include "Plugins/XmlToBinPlugin/XmlToBinInterface.h"
#endif

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
        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator, false );

        m_archivator = archivator;

        m_protocolPath = CONFIG_VALUE( "Engine", "ProtocolPath", STRINGIZE_FILEPATH_LOCAL( "protocol.xml" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoaderService::_finalizeService()
    {
        m_archivator = nullptr;

        m_loaders.clear();

        m_metacache.strings.clear();
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
    bool LoaderService::load( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, Metabuf::Metaparse * _metadata, uint32_t _metaVersion, bool & _exist ) const
    {
        LOGGER_INFO( "pak '%s:%s'"
            , _fileGroup->getName().c_str()
            , _filePath.c_str()
        );

        if( _filePath.empty() == true )
        {
            LOGGER_ERROR( "invalid open bin '%s' path is empty"
                , _fileGroup->getName().c_str()
            );

            return false;
        }

        InputStreamInterfacePtr file_bin;
        if( this->openBin_( _fileGroup, _filePath, file_bin, _exist ) == false )
        {
            LOGGER_ERROR( "invalid open bin '%s':'%s'"
                , _fileGroup->getName().c_str()
                , _filePath.c_str()
            );

            return false;
        }


        MENGINE_ASSERTION_MEMORY_PANIC( file_bin, true );

        bool reimport = false;
        bool done = this->importBin_( file_bin, _metadata, _metaVersion, &reimport );

#ifndef MENGINE_MASTER_RELEASE
        if( reimport == true )
        {
            file_bin = nullptr;

            PathString cache_path_xml;
            cache_path_xml += _filePath;
            cache_path_xml.replace_last( "xml" );

            ConstString c_cache_path_xml = Helper::stringizeStringSize( cache_path_xml.c_str(), cache_path_xml.size() );

            if( this->makeBin_( _fileGroup, FilePath( c_cache_path_xml ), _filePath ) == false )
            {
                LOGGER_ERROR( "invlid rebild bin %s from xml %s"
                    , _filePath.c_str()
                    , c_cache_path_xml.c_str()
                );

                return false;
            }

            file_bin = FILE_SERVICE()
                ->openInputFile( _fileGroup, _filePath, false, MENGINE_DOCUMENT_FUNCTION );

            done = this->importBin_( file_bin, _metadata, _metaVersion, nullptr );
        }
#endif

        return done;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderService::validation( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, uint32_t _metaVersion ) const
    {
        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( _fileGroup, _filePath, false, MENGINE_DOCUMENT_FUNCTION );

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
    bool LoaderService::importBin_( const InputStreamInterfacePtr & _stream, Metabuf::Metaparse * _metadata, uint32_t _metaVersion, bool * _reimport ) const
    {
        size_t size = _stream->size();

        if( size == 0 )
        {
            LOGGER_ERROR( "invalid size (empty)"
            );

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
                LOGGER_ERROR( "error '%s' invalid version read %d need %d or protocol %d need %d (Update you protocol file)"
                    , Metacode::getHeaderErrorMessage( result )
                    , readVersion
                    , needVersion
                    , readProtocol
                    , needProtocol
                );
            }
            else
            {
                *_reimport = true;
            }

            return false;
        }

        uint32_t bin_size;
        _stream->read( &bin_size, sizeof( bin_size ) );

        uint32_t compress_size;
        _stream->read( &compress_size, sizeof( compress_size ) );

        MemoryInterfacePtr binary_buffer = Helper::createMemoryCacheBuffer( bin_size, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( binary_buffer, false );

        uint8_t * binary_memory = binary_buffer->getBuffer();

        size_t uncompress_size = 0;
        if( ARCHIVE_SERVICE()
            ->decompressStream( m_archivator, _stream, compress_size, binary_memory, bin_size, uncompress_size ) == false )
        {
            if( _reimport == nullptr )
            {
                LOGGER_ERROR( "invalid uncompress"
                );
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

            MENGINE_ASSERTION_MEMORY_PANIC( stringBuffer, false, "invlid read string (error)" );

            STRINGIZE_SERVICE()
                ->stringize( stringBuffer, stringSize, stringHash, cstr );

            ++index;
        }

        if( _metadata->parse( binary_memory, bin_size, read_size, (void *)&m_metacache ) == false )
        {
            LOGGER_ERROR( "invlid parse (error)"
            );

            return false;
        }

        m_metacache.strings.clear();

        if( read_size != bin_size )
        {
            LOGGER_ERROR( "invlid parse (read != archive)"
            );

            return false;
        }

        return true;
    }
#ifndef MENGINE_MASTER_RELEASE
    //////////////////////////////////////////////////////////////////////////
    bool LoaderService::openBin_( const FileGroupInterfacePtr & _pak, const FilePath & _path, InputStreamInterfacePtr & _file, bool & _exist ) const
    {
        PathString cache_path_xml;
        cache_path_xml += _path;
        cache_path_xml.replace_last( "xml" );

        FilePath c_cache_path_xml = Helper::stringizeFilePath( cache_path_xml );

        if( _pak->existFile( c_cache_path_xml ) == false )
        {
            if( _pak->existFile( _path ) == false )
            {
                _exist = false;

                return false;
            }

            InputStreamInterfacePtr file_bin = FILE_SERVICE()
                ->openInputFile( _pak, _path, false, MENGINE_DOCUMENT_FUNCTION );

            MENGINE_ASSERTION_MEMORY_PANIC( file_bin, false );

            _file = file_bin;

            return true;
        }

        _exist = true;

        if( _pak->existFile( _path ) == false )
        {
            if( this->makeBin_( _pak, c_cache_path_xml, _path ) == false )
            {
                _file = nullptr;

                return false;
            }
        }

        InputStreamInterfacePtr file_bin = FILE_SERVICE()
            ->openInputFile( _pak, _path, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( file_bin, false );

        InputStreamInterfacePtr file_xml = FILE_SERVICE()
            ->openInputFile( _pak, c_cache_path_xml, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( file_xml, false );

        uint64_t time_xml;
        file_xml->time( time_xml );

        uint64_t time_bin;
        file_bin->time( time_bin );

        file_xml = nullptr;

        if( time_xml > time_bin )
        {
            //Rebild bin file from xml
            file_bin = nullptr;

            if( this->makeBin_( _pak, c_cache_path_xml, _path ) == false )
            {
                _file = nullptr;

                return false;
            }

            file_bin = FILE_SERVICE()
                ->openInputFile( _pak, _path, false, MENGINE_DOCUMENT_FUNCTION );
        }

        _file = file_bin;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderService::makeBin_( const FileGroupInterfacePtr & _pak, const FilePath & _pathXml, const FilePath & _pathBin ) const
    {
        XmlDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoderT<XmlDecoderInterfacePtr>( STRINGIZE_STRING_LOCAL( "xml2bin" ), MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( decoder, false, "invalid create decoder xml2bin for %s:%s"
            , _pak->getName().c_str()
            , _pathXml.c_str()
        );

        if( decoder->prepareData( nullptr ) == false )
        {
            LOGGER_ERROR( "invalid initialize decoder xml2bin for %s:%s"
                , _pak->getName().c_str()
                , _pathXml.c_str()
            );

            return false;
        }

        XmlCodecOptions options;
        options.pathProtocol = m_protocolPath;

        const FilePath & folderPath = _pak->getFolderPath();

        options.pathXml = Helper::concatenationFilePath( folderPath, _pathXml );
        options.pathBin = Helper::concatenationFilePath( folderPath, _pathBin );

        options.useProtocolVersion = Metacode::get_metacode_protocol_version();
        options.useProtocolCrc32 = Metacode::get_metacode_protocol_crc32();

        if( decoder->setOptions( &options ) == false )
        {
            return false;
        }

        if( decoder->decode( 0, 0 ) == 0 )
        {
            return false;
        }

        return true;
    }
#else
    //////////////////////////////////////////////////////////////////////////
    bool LoaderService::openBin_( const FileGroupInterfacePtr & _pak, const FilePath & _path, InputStreamInterfacePtr & _file, bool & _exist ) const
    {
        if( _pak->existFile( _path ) == false )
        {
            _exist = false;

            return false;
        }

        InputStreamInterfacePtr file_bin = FILE_SERVICE()
            ->openInputFile( _pak, _path, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( file_bin, false );

        _file = file_bin;

        return true;
    }
#endif
}
