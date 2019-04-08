#include "XmlToBinDecoder.h"

#include "Interface/LoaderServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "Metacode/Metacode.h"

#include "metabuf/Metadata.hpp"

#include "Xml2Metabuf.hpp"
#include "Xml2Metacode.hpp"

#include "Config/Blobject.h"

#include <stdio.h>
#include "utf8.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static bool s_write_wstring( Metabuf::Xml2Metabuf * _metabuf, const char * _value, void * _user )
    {
        (void)_user;

        uint32_t utf8_size = (uint32_t)strlen( _value );

        _metabuf->writeSize( utf8_size );
        _metabuf->writeCount( &_value[0], utf8_size );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_write_wchar_t( Metabuf::Xml2Metabuf * _metabuf, const char * _value, void * _user )
    {
        (void)_user;

        uint32_t utf8_size = (uint32_t)strlen( _value );

        _metabuf->writeSize( utf8_size );
        _metabuf->writeCount( &_value[0], utf8_size );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_write_utf8( Metabuf::Xml2Metabuf * _metabuf, const char * _value, void * _user )
    {
        (void)_user;

        size_t len = strlen( _value );
        const char * text_it = _value;
        const char * text_end = _value + len + 1;

        uint32_t code;
        utf8::internal::validate_next( text_it, text_end, code );

        _metabuf->write( code );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    XmlToBinDecoder::XmlToBinDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    XmlToBinDecoder::~XmlToBinDecoder()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const InputStreamInterfacePtr & XmlToBinDecoder::getStream() const
    {
        return m_stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool XmlToBinDecoder::setOptions( const CodecOptions * _options )
    {
        m_options = *static_cast<const XmlCodecOptions *>(_options);

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void XmlToBinDecoder::setCodecDataInfo( const CodecDataInfo * _dataInfo )
    {
        (void)_dataInfo;
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    const XmlCodecDataInfo * XmlToBinDecoder::getCodecDataInfo() const
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool XmlToBinDecoder::initialize()
    {
        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        if( archivator == nullptr )
        {
            return false;
        }

        m_archivator = archivator;

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "dev" ) );

        if( fileGroup == nullptr )
        {
            return false;
        }

        m_fileGroup = fileGroup;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void XmlToBinDecoder::finalize()
    {
        m_stream = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool XmlToBinDecoder::prepareData( const InputStreamInterfacePtr & _stream )
    {
        m_stream = _stream;

        return true;
    }
    ////////////////////////////////////////////////////////////////////////////
    size_t XmlToBinDecoder::decode( void * _buffer, size_t _bufferSize )
    {
        (void)_buffer;
        (void)_bufferSize;

        LOGGER_MESSAGE( "xml to bin:\nxml - %s\nbin - %s"
            , m_options.pathXml.c_str()
            , m_options.pathBin.c_str()
        );

        InputStreamInterfacePtr protocol_stream = FILE_SERVICE()
            ->openInputFile( m_fileGroup, m_options.pathProtocol, false, MENGINE_DOCUMENT_FUNCTION );

        if( protocol_stream == nullptr )
        {
            LOGGER_ERROR( "error open protocol %s"
                , m_options.pathProtocol.c_str()
            );

            return 0;
        }

        size_t protocol_size = protocol_stream->size();

        MemoryBufferInterfacePtr memory_protocol = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FUNCTION );

        if( memory_protocol == nullptr )
        {
            LOGGER_ERROR( "invalid create memory for protocol"
            );

            return 0;
        }

        void * memory_protocol_buffer = memory_protocol->newBuffer( protocol_size, "XmlToBinDecoder::decode", __FILE__, __LINE__ );

        if( memory_protocol_buffer == nullptr )
        {
            LOGGER_ERROR( "invalid new memory buffer '%u'"
                , protocol_size
            );

            return 0;
        }

        if( protocol_stream->read( memory_protocol_buffer, protocol_size ) != protocol_size )
        {
            LOGGER_ERROR( "error read protocol %s error invalid read size"
                , m_options.pathProtocol.c_str()
            );

            return 0;
        }

        protocol_stream = nullptr;

        Metabuf::XmlProtocol xml_protocol;

        if( xml_protocol.readProtocol( memory_protocol_buffer, protocol_size ) == false )
        {
            LOGGER_ERROR( "error read protocol %s error:\n%s"
                , m_options.pathProtocol.c_str()
                , xml_protocol.getError().c_str()
            );

            return 0;
        }

        if( m_options.useProtocolVersion != xml_protocol.getVersion() )
        {
            LOGGER_ERROR( "protocol '%s' invalid version '%d' use '%d'"
                , m_options.pathProtocol.c_str()
                , xml_protocol.getVersion()
                , m_options.useProtocolVersion
            );

            return 0;
        }

        if( m_options.useProtocolCrc32 != xml_protocol.getCrc32() )
        {
            LOGGER_ERROR( "protocol '%s' invalid version '%d' use '%d'"
                , m_options.pathProtocol.c_str()
                , xml_protocol.getVersion()
                , m_options.useProtocolCrc32
            );

            return 0;
        }

        memory_protocol = nullptr;

        InputStreamInterfacePtr xml_stream = FILE_SERVICE()
            ->openInputFile( m_fileGroup, m_options.pathXml, false, MENGINE_DOCUMENT_FUNCTION );

        if( xml_stream == nullptr )
        {
            LOGGER_ERROR( "error open xml %s"
                , m_options.pathXml.c_str()
            );

            return 0;
        }

        size_t xml_size = xml_stream->size();

        if( xml_size == 0 )
        {
            LOGGER_ERROR( "error open xml %s (file size == 0)"
                , m_options.pathXml.c_str()
            );

            return 0;
        }

        MemoryBufferInterfacePtr memory_xml = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FUNCTION );

        if( memory_xml == nullptr )
        {
            LOGGER_ERROR( "invalid create memory for xml"
            );

            return 0;
        }

        void * memory_xml_buffer = memory_xml->newBuffer( xml_size, "XmlToBinDecoder::decode", __FILE__, __LINE__ );

        if( memory_xml == nullptr )
        {
            LOGGER_ERROR( "invalid new memory buffer '%u'"
                , xml_size
            );

            return 0;
        }

        if( xml_stream->read( memory_xml_buffer, xml_size ) != xml_size )
        {
            return 0;
        }

        xml_stream = nullptr;

        const Metabuf::XmlMeta * xml_meta = xml_protocol.getMeta( "Data" );

        if( xml_meta == nullptr )
        {
            return 0;
        }

        Metabuf::Xml2Metabuf xml_metabuf( &xml_protocol, xml_meta );

        LOGGER_INFO( "\nxml %s\nbin %s"
            , m_options.pathXml.c_str()
            , m_options.pathBin.c_str()
        );

        xml_metabuf.initialize();

        xml_metabuf.addSerializator( "wstring", &s_write_wstring, (void*)nullptr );
        xml_metabuf.addSerializator( "wchar_t", &s_write_wchar_t, (void*)nullptr );
        xml_metabuf.addSerializator( "utf8", &s_write_utf8, (void*)nullptr );

        MemoryBufferInterfacePtr memory_header = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FUNCTION );

        if( memory_header == nullptr )
        {
            LOGGER_ERROR( "invalid create memory for bin"
            );

            return 0;
        }

        void * memory_header_buffer = memory_header->newBuffer( Metacode::header_size, "XmlToBinDecoder::decode", __FILE__, __LINE__ );

        if( memory_header_buffer == nullptr )
        {
            LOGGER_ERROR( "invalid new memory buffer '%u'"
                , Metacode::header_size
            );

            return 0;
        }

        uint32_t xml_meta_version = xml_meta->getVersion();

        size_t header_size;
        if( xml_metabuf.header( memory_header_buffer, Metacode::header_size, xml_meta_version, header_size ) == false )
        {
            LOGGER_ERROR( "error header '%s' version '%u' error:\n%s"
                , m_options.pathXml.c_str()
                , xml_meta_version
                , xml_metabuf.getError().c_str()
            );

            return 0;
        }

        MemoryBufferInterfacePtr memory_bin = MEMORY_SERVICE()
            ->createMemoryBuffer( MENGINE_DOCUMENT_FUNCTION );

        if( memory_bin == nullptr )
        {
            LOGGER_ERROR( "invalid create memory for bin"
            );

            return 0;
        }

        void * memory_bin_buffer = memory_bin->newBuffer( xml_size * 2, "XmlToBinDecoder::decode", __FILE__, __LINE__ );

        if( memory_bin_buffer == nullptr )
        {
            LOGGER_ERROR( "invalid new memory buffer '%u'"
                , xml_size * 2
            );

            return 0;
        }

        size_t bin_size;
        if( xml_metabuf.convert( memory_bin_buffer, xml_size * 2, memory_xml_buffer, xml_size, bin_size ) == false )
        {
            LOGGER_ERROR( "error convert %s error:\n%s"
                , m_options.pathXml.c_str()
                , xml_metabuf.getError().c_str()
            );

            return 0;
        }

        MemoryInputInterfacePtr compress_memory = ARCHIVE_SERVICE()
            ->compressBuffer( m_archivator, memory_bin_buffer, bin_size, EAC_BEST );

        if( compress_memory == nullptr )
        {
            LOGGER_ERROR( "error convert %s invalid compress buffer"
                , m_options.pathXml.c_str()
            );

            return 0;
        }

        OutputStreamInterfacePtr bin_stream = FILE_SERVICE()
            ->openOutputFile( m_fileGroup, m_options.pathBin, MENGINE_DOCUMENT_FUNCTION );

        if( bin_stream == nullptr )
        {
            LOGGER_ERROR( "error create bin %s"
                , m_options.pathBin.c_str()
            );

            return 0;
        }

        bin_stream->write( memory_header_buffer, Metacode::header_size );

        uint32_t write_bin_size = (uint32_t)bin_size;
        bin_stream->write( &write_bin_size, sizeof( write_bin_size ) );

        const void * compress_buffer = compress_memory->getBuffer();
        size_t compress_size = compress_memory->getSize();

        if( compress_buffer == nullptr )
        {
            LOGGER_ERROR( "error create bin '%s' invalid get memory"
                , m_options.pathBin.c_str()
            );

            return 0;
        }

        uint32_t write_compress_size = (uint32_t)compress_size;
        bin_stream->write( &write_compress_size, sizeof( write_compress_size ) );
        bin_stream->write( compress_buffer, write_compress_size );

        return bin_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool XmlToBinDecoder::rewind()
    {
        bool successful = m_stream->seek( 0 );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool XmlToBinDecoder::seek( float _timing )
    {
        (void)_timing;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    float XmlToBinDecoder::tell() const
    {
        return 0.0;
    }
}
