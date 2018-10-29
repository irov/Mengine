#include "XmlToBinDecoder.h"

#include "Interface/LoaderServiceInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/FileSystemInterface.h"

#include "Kernel/Logger.h"

#include "metabuf/Metadata.hpp"

#include "xml2metabuf/Xml2Metabuf.hpp"
#include "xml2metabuf/Xml2Metacode.hpp"

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
        const ArchivatorInterfacePtr & archivator = ARCHIVE_SERVICE()
            ->getArchivator( STRINGIZE_STRING_LOCAL( "lz4" ) );

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

        LOGGER_WARNING( "Xml2BinDecoder::decode: xml to bin:\nxml - %s\nbin - %s"
            , m_options.pathXml.c_str()
            , m_options.pathBin.c_str()
        );

        InputStreamInterfacePtr protocol_stream = FILE_SERVICE()
            ->openInputFile( m_fileGroup, m_options.pathProtocol, false );

        if( protocol_stream == nullptr )
        {
            LOGGER_ERROR( "Xml2BinDecoder::decode: error open protocol %s"
                , m_options.pathProtocol.c_str()
            );

            return 0;
        }

        size_t protocol_size = protocol_stream->size();

        Blobject protocol_buf;
        protocol_buf.resize( protocol_size );

        if( protocol_stream->read( &protocol_buf[0], protocol_size ) != protocol_size )
        {
            LOGGER_ERROR( "Xml2BinDecoder::decode: error read protocol %s error invalid read size"
                , m_options.pathProtocol.c_str()
            );

            return 0;
        }

        protocol_stream = nullptr;

        Metabuf::XmlProtocol xml_protocol;

        if( xml_protocol.readProtocol( &protocol_buf[0], protocol_size ) == false )
        {
            LOGGER_ERROR( "Xml2BinDecoder::decode: error read protocol %s error:\n%s"
                , m_options.pathProtocol.c_str()
                , xml_protocol.getError().c_str()
            );

            return 0;
        }

        InputStreamInterfacePtr xml_stream = FILE_SERVICE()
            ->openInputFile( m_fileGroup, m_options.pathXml, false );

        if( xml_stream == nullptr )
        {
            LOGGER_ERROR( "Xml2BinDecoder::decode: error open xml %s"
                , m_options.pathXml.c_str()
            );

            return 0;
        }

        size_t xml_size = xml_stream->size();

        if( xml_size == 0 )
        {
            LOGGER_ERROR( "Xml2BinDecoder::decode: error open xml %s (file size == 0)"
                , m_options.pathXml.c_str()
            );

            return 0;
        }

        Blobject xml_buf;
        xml_buf.resize( xml_size );

        xml_stream->read( &xml_buf[0], xml_size );
        xml_stream = nullptr;

        const Metabuf::XmlMeta * xml_meta = xml_protocol.getMeta( "Data" );

        Metabuf::Xml2Metabuf xml_metabuf( &xml_protocol, xml_meta );

        LOGGER_INFO( "Xml2BinDecoder::decode:\nxml %s\nbin %s"
            , m_options.pathXml.c_str()
            , m_options.pathBin.c_str()
        );

        xml_metabuf.initialize();

        xml_metabuf.addSerializator( "wstring", &s_write_wstring, (void*)nullptr );
        xml_metabuf.addSerializator( "wchar_t", &s_write_wchar_t, (void*)nullptr );
        xml_metabuf.addSerializator( "utf8", &s_write_utf8, (void*)nullptr );

        Blobject header_buf;
        header_buf.resize( Metabuf::header_size );

        size_t header_size;
        if( xml_metabuf.header( &header_buf[0], 16, xml_meta->getVersion(), header_size ) == false )
        {
            LOGGER_ERROR( "Xml2BinDecoder::decode: error header %s error:\n%s"
                , m_options.pathXml.c_str()
                , xml_metabuf.getError().c_str()
            );

            return 0;
        }

        Blobject bin_buf;
        bin_buf.resize( xml_size * 2 );

        size_t bin_size;
        if( xml_metabuf.convert( &bin_buf[0], xml_size * 2, &xml_buf[0], xml_size, bin_size ) == false )
        {
            LOGGER_ERROR( "Xml2BinDecoder::decode: error convert %s error:\n%s"
                , m_options.pathXml.c_str()
                , xml_metabuf.getError().c_str()
            );

            return 0;
        }

        MemoryInputInterfacePtr compress_memory = ARCHIVE_SERVICE()
            ->compressBuffer( m_archivator, &bin_buf[0], bin_size, EAC_BEST );

        if( compress_memory == nullptr )
        {
            LOGGER_ERROR( "Xml2BinDecoder::decode: error convert %s invalid compress buffer"
                , m_options.pathXml.c_str()
            );

            return 0;
        }

        OutputStreamInterfacePtr bin_stream = FILE_SERVICE()
            ->openOutputFile( m_fileGroup, m_options.pathBin );

        if( bin_stream == nullptr )
        {
            LOGGER_ERROR( "Xml2BinDecoder::decode: error create bin %s"
                , m_options.pathBin.c_str()
            );

            return 0;
        }

        bin_stream->write( &header_buf[0], Metabuf::header_size );

        uint32_t write_bin_size = (uint32_t)bin_size;
        bin_stream->write( &write_bin_size, sizeof( write_bin_size ) );

        const void * compress_buffer = compress_memory->getBuffer();
        size_t compress_size = compress_memory->getSize();

        if( compress_buffer == nullptr )
        {
            LOGGER_ERROR( "Xml2BinDecoder::decode: error create bin %s invalid get memory"
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