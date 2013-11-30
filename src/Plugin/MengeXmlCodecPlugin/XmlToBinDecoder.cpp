#	include "XmlToBinDecoder.h"

#   include "Interface/StringizeInterface.h"
#   include "Interface/ArchiveInterface.h"

#   include "Config/Blobject.h"

#	include "Utils/Logger/Logger.h"

#	include "metabuf/Metabuf.hpp"

#	include "xml2metabuf/Xml2Metabuf.hpp"
#	include "xml2metabuf/Xml2Metacode.hpp"

#   include <stdio.h>
#   include <utf8.h>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    static bool s_write_wstring( Metabuf::Xml2Metabuf * _metabuf, const char * _value, void * _user )
    {
        (void)_user;

        size_t utf8_size = strlen( _value ); 

        _metabuf->writeSize( utf8_size );        
        _metabuf->writeCount( &_value[0], utf8_size );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_write_wchar_t( Metabuf::Xml2Metabuf * _metabuf, const char * _value, void * _user )
    {
        (void)_user;

        size_t utf8_size = strlen( _value ); 

        _metabuf->writeSize( utf8_size );
        _metabuf->writeCount( &_value[0], utf8_size );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_write_utf8( Metabuf::Xml2Metabuf * _metabuf, const char * _value, void * _user )
    {
        (void)_user;

        size_t len = strlen(_value);
        const char * text_it = _value;
        const char * text_end = _value + len + 1;
        
        size_t code = utf8::next(text_it, text_end);

        _metabuf->write( code );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	XmlToBinDecoder::XmlToBinDecoder()
        : m_serviceProvider(nullptr)
        , m_stream(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void XmlToBinDecoder::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * XmlToBinDecoder::getServiceProvider() const
	{
		return m_serviceProvider;
	}
    //////////////////////////////////////////////////////////////////////////
    const InputStreamInterfacePtr & XmlToBinDecoder::getStream() const
    {
        return m_stream;
    }
	//////////////////////////////////////////////////////////////////////////
	bool XmlToBinDecoder::setOptions( CodecOptions * _options )
	{
		m_options = *static_cast<XmlCodecOptions *>(_options);

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const XmlCodecDataInfo * XmlToBinDecoder::getCodecDataInfo() const
	{
		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlToBinDecoder::initialize( const InputStreamInterfacePtr & _stream, bool & _version )
	{
        m_stream = _stream;

		_version = true;

		return true;
	}
	////////////////////////////////////////////////////////////////////////////
	size_t XmlToBinDecoder::decode( void * _buffer, size_t _bufferSize )
	{
        (void)_buffer;
        (void)_bufferSize;

        LOGGER_WARNING(m_serviceProvider)( "Xml2BinDecoder::decode: xml to bin:\nxml - %s\nbin - %s"
            , m_options.pathXml.c_str()
            , m_options.pathBin.c_str()
            );

        ConstString dev = Helper::stringizeStringSize( m_serviceProvider, "dev", 3 );

        InputStreamInterfacePtr protocol_stream = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( dev, m_options.pathProtocol );

		//FILE * file_protocol = _wfopen( unicode_pathProtocol.c_str(), L"rb" );

        if( protocol_stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error open protocol %s"
                , m_options.pathProtocol.c_str()
                );

            return 0;
        }

		size_t protocol_size = protocol_stream->size();

		TBlobject protocol_buf(protocol_size);
        protocol_stream->read( &protocol_buf[0], protocol_size );
        protocol_stream = nullptr;

        Metabuf::XmlProtocol xml_protocol;

		if( xml_protocol.readProtocol( &protocol_buf[0], protocol_size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error read protocol %s error:\n%s"
                , m_options.pathProtocol.c_str()
                , xml_protocol.getError().c_str()
                );

            return 0;
        }
        	
        InputStreamInterfacePtr xml_stream = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( dev, m_options.pathXml );

        if( xml_stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error open xml %s"
                , m_options.pathXml.c_str()
                );

            return 0;
        }

        size_t xml_size = xml_stream->size();

        if( xml_size == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error open xml %s (file size == 0)"
                , m_options.pathXml.c_str()
                );

            return 0;
        }

        TBlobject xml_buf(xml_size);
        xml_stream->read( &xml_buf[0], xml_size );
        xml_stream = nullptr;

		Metabuf::Xml2Metabuf xml_metabuf(&xml_protocol);

        LOGGER_INFO(m_serviceProvider)( "Xml2BinDecoder::decode:\nxml %s\nbin %s"
            , m_options.pathXml.c_str()
            , m_options.pathBin.c_str()
            );

        xml_metabuf.initialize();

        xml_metabuf.addSerializator( "wstring", &s_write_wstring, (void*)m_serviceProvider );
        xml_metabuf.addSerializator( "wchar_t", &s_write_wchar_t, (void*)m_serviceProvider );
        xml_metabuf.addSerializator( "utf8", &s_write_utf8, (void*)m_serviceProvider );
		
        TBlobject header_buf(Metabuf::header_size);

        size_t header_size;
        if( xml_metabuf.header( &header_buf[0], 16, header_size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error header %s error:\n%s"
                , m_options.pathXml.c_str()
                , xml_metabuf.getError().c_str()
                );

            return 0;
        }

        TBlobject bin_buf(xml_size * 2);

        size_t bin_size;
		if( xml_metabuf.convert( &bin_buf[0], xml_size * 2, &xml_buf[0], xml_size, bin_size ) == false )
		{
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error convert %s error:\n%s"
                , m_options.pathXml.c_str()
                , xml_metabuf.getError().c_str()
                );

			return 0;
		}

        size_t bound_size = ARCHIVE_SERVICE(m_serviceProvider)
            ->compressBound( bin_size );

        TBlobject compress_buf(bound_size);
        
        size_t compress_size;
        if( ARCHIVE_SERVICE(m_serviceProvider)
            ->compress( &compress_buf[0], bound_size, compress_size, &bin_buf[0], bin_size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error compress bin %s"
                , m_options.pathBin.c_str()
                );

            return 0;
        }

        OutputStreamInterfacePtr bin_stream = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( dev, m_options.pathBin );

        if( bin_stream == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error create bin %s"
                , m_options.pathBin.c_str()
                );

            return 0;
        }

        bin_stream->write( &header_buf[0], Metabuf::header_size );
        
        bin_stream->write( &bin_size, sizeof(bin_size) );
        bin_stream->write( &compress_size, sizeof(compress_size) );
        bin_stream->write( &compress_buf[0], compress_size );
		
		return bin_size;
	}
}