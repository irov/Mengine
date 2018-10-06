#include "LoaderEngine.h"

#include "Interface/FileSystemInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/XmlCodecInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/ConfigInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/FilePath.h"

#include "metabuf/Metadata.hpp"
#include "Metacode.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( LoaderService, Mengine::LoaderEngine );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderEngine::LoaderEngine()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderEngine::~LoaderEngine()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderEngine::_initializeService()
    {
        const ArchivatorInterfacePtr & archivator = ARCHIVE_SERVICE()
            ->getArchivator( STRINGIZE_STRING_LOCAL( "lz4" ) );

        if( archivator == nullptr )
        {
            return false;
        }

        m_archivator = archivator;

        m_protocolPath = CONFIG_VALUE( "Engine", "ProtocolPath", STRINGIZE_FILEPATH_LOCAL( "protocol.xml" ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LoaderEngine::_finalizeService()
    {
        m_archivator = nullptr;

        m_protocolPath.clear();
        m_bufferConstString.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void LoaderEngine::setProtocolPath( const FilePath & _protocolPath )
    {
        m_protocolPath = _protocolPath;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderEngine::load( const FileGroupInterfacePtr & _pak, const FilePath & _path, Metabuf::Metadata * _metadata, bool & _exist ) const
    {
        LOGGER_INFO( "LoaderEngine::load pak '%s:%s'"
            , _pak->getName().c_str()
            , _path.c_str()
        );

        if( _path.empty() == true )
        {
            LOGGER_ERROR( "LoaderEngine::import invalid open bin '%s' path is empty"
                , _pak->getName().c_str()
            );

            return false;
        }

        InputStreamInterfacePtr file_bin;
        if( this->openBin_( _pak, _path, file_bin, _exist ) == false )
        {
            LOGGER_ERROR( "LoaderEngine::import invalid open bin '%s':'%s'"
                , _pak->getName().c_str()
                , _path.c_str()
            );

            return false;
        }


        if( file_bin == nullptr )
        {
            return true;
        }

        bool reimport = false;
        bool done = this->importBin_( file_bin, _metadata, &reimport );

#ifndef MENGINE_MASTER_RELEASE
        if( reimport == true )
        {
            file_bin = nullptr;

            PathString cache_path_xml;
            cache_path_xml += _path;
            cache_path_xml.replace_last( "xml" );

            ConstString c_cache_path_xml = Helper::stringizeStringSize( cache_path_xml.c_str(), cache_path_xml.size() );

            if( this->makeBin_( _pak, FilePath( c_cache_path_xml ), _path ) == false )
            {
                LOGGER_ERROR( "LoaderEngine::import invlid rebild bin %s from xml %s"
                    , _path.c_str()
                    , c_cache_path_xml.c_str()
                );

                return false;
            }

            file_bin = FILE_SERVICE()
                ->openInputFile( _pak, _path, false );

            done = this->importBin_( file_bin, _metadata, nullptr );
        }
#endif

        return done;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderEngine::validation( const FileGroupInterfacePtr & _pak, const FilePath & _path, const Metabuf::Metadata * _metadata ) const
    {
        InputStreamInterfacePtr stream =
            FILE_SERVICE()->openInputFile( _pak, _path, false );

        if( stream == nullptr )
        {
            return false;
        }

        size_t size = stream->size();

        if( size == 0 )
        {
            return false;
        }

        uint8_t header_buff[Metabuf::header_size];

        stream->read( header_buff, Metabuf::header_size );

        size_t header_read = 0;
        uint32_t readVersion;
        uint32_t needVersion;
        uint32_t readProtocol;
        uint32_t needProtocol;
        uint32_t metaMetaVersion = _metadata->getVersion();
        uint32_t needMetaVersion;

        if( Metacode::readHeader( header_buff, Metabuf::header_size, header_read, readVersion, needVersion, readProtocol, needProtocol, metaMetaVersion, needMetaVersion ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderEngine::importBin_( const InputStreamInterfacePtr & _stream, Metabuf::Metadata * _metadata, bool * _reimport ) const
    {
        size_t size = _stream->size();

        if( size == 0 )
        {
            LOGGER_ERROR( "LoaderEngine::importBin_ invalid size (empty)"
            );

            return false;
        }

        uint8_t header_buff[Metabuf::header_size];

        _stream->read( header_buff, Metabuf::header_size );

        size_t header_read = 0;
        uint32_t readVersion;
        uint32_t needVersion;
        uint32_t readProtocol;
        uint32_t needProtocol;
        uint32_t metaMetaVersion = _metadata->getVersion();
        uint32_t needMetaVersion;

        if( Metacode::readHeader( header_buff, Metabuf::header_size, header_read, readVersion, needVersion, readProtocol, needProtocol, metaMetaVersion, needMetaVersion ) == false )
        {
            if( _reimport == nullptr )
            {
                LOGGER_ERROR( "LoaderEngine::loadBinary invalid version read %d need %d or protocol %d need %d (Update you protocol file)"
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

        MemoryInterfacePtr binary_buffer = Helper::createMemoryCacheBuffer( bin_size, "LoaderEngine::importBin_", __FILE__, __LINE__ );

        if( binary_buffer == nullptr )
        {
            return false;
        }

        uint8_t * binary_memory = binary_buffer->getBuffer();

        size_t uncompress_size = 0;
        if( ARCHIVE_SERVICE()
            ->decompressStream( m_archivator, _stream, compress_size, binary_memory, bin_size, uncompress_size ) == false )
        {
            if( _reimport == nullptr )
            {
                LOGGER_ERROR( "LoaderEngine::loadBinary invlid uncompress"
                );
            }
            else
            {
                *_reimport = true;
            }

            return false;
        }

        size_t read_size = 0;

        uint32_t stringCount;
        if( Metacode::readStrings( binary_memory, bin_size, read_size, stringCount ) == false )
        {
            return false;
        }

        m_bufferConstString.resize( stringCount );

        for( VectorConstString::iterator
            it = m_bufferConstString.begin(),
            it_end = m_bufferConstString.end();
            it != it_end;
            ++it )
        {
            uint32_t stringSize;
            int64_t stringHash;
            const char * str = Metacode::readString( binary_memory, bin_size, read_size, stringSize, stringHash );

            if( str == nullptr )
            {
                LOGGER_ERROR( "LoaderEngine::loadBinary invlid read string (error)"
                );

                return false;
            }

            *it = Helper::stringizeStringSizeHash( str, stringSize, stringHash );
        }

        if( _metadata->parseRoot( binary_memory, bin_size, read_size, (void *)this ) == false )
        {
            LOGGER_ERROR( "LoaderEngine::loadBinary invlid parse (error)"
            );

            return false;
        }

        m_bufferConstString.clear();

        if( read_size != bin_size )
        {
            LOGGER_ERROR( "LoaderEngine::loadBinary invlid parse (read != archive)"
            );

            return false;
        }

        return true;
    }
#ifndef MENGINE_MASTER_RELEASE
    //////////////////////////////////////////////////////////////////////////
    bool LoaderEngine::openBin_( const FileGroupInterfacePtr & _pak, const FilePath & _path, InputStreamInterfacePtr & _file, bool & _exist ) const
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
                ->openInputFile( _pak, _path, false );

            if( file_bin == nullptr )
            {
                return false;
            }

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
            ->openInputFile( _pak, _path, false );

        if( file_bin == nullptr )
        {
            return false;
        }

        InputStreamInterfacePtr file_xml = FILE_SERVICE()
            ->openInputFile( _pak, c_cache_path_xml, false );

        if( file_xml == nullptr )
        {
            return false;
        }

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
                ->openInputFile( _pak, _path, false );
        }

        _file = file_bin;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderEngine::makeBin_( const FileGroupInterfacePtr & _pak, const FilePath & _pathXml, const FilePath & _pathBin ) const
    {
        XmlDecoderInterfacePtr decoder = CODEC_SERVICE()
            ->createDecoderT<XmlDecoderInterfacePtr>( STRINGIZE_STRING_LOCAL( "xml2bin" ) );

        if( decoder == nullptr )
        {
            LOGGER_ERROR( "LoaderEngine::makeBin_ invalid create decoder xml2bin for %s:%s"
                , _pak->getName().c_str()
                , _pathXml.c_str()
            );

            return false;
        }

        if( decoder->prepareData( nullptr ) == false )
        {
            LOGGER_ERROR( "LoaderEngine::makeBin_ invalid initialize decoder xml2bin for %s:%s"
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
    bool LoaderEngine::openBin_( const FileGroupInterfacePtr & _pak, const FilePath & _path, InputStreamInterfacePtr & _file, bool & _exist ) const
    {
        if( _pak->existFile( _path ) == false )
        {
            _exist = false;

            return false;
        }

        InputStreamInterfacePtr file_bin = FILE_SERVICE()
            ->openInputFile( _pak, _path, false );

        if( file_bin == nullptr )
        {
            return false;
        }

        _file = file_bin;

        return true;
    }
#endif
    //////////////////////////////////////////////////////////////////////////
    const ConstString & LoaderEngine::getCacheConstString( uint32_t _index ) const
    {
        return m_bufferConstString[_index];
    }
}
