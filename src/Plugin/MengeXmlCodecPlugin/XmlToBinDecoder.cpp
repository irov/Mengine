#	include "XmlToBinDecoder.h"

#	include "Utils/Logger/Logger.h"

#	include "xml2metabuf/Xml2Metabuf.hpp"
#	include "xml2metabuf/Xml2Metacode.hpp"

#   include <stdio.h>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    static bool s_write_wstring( Metabuf::Xml2Metabuf * _metabuf, const char * _value, void * _user )
    {
        (void)_user;

        size_t utf8_size = strlen( _value ); 

        _metabuf->write( utf8_size );
        
        _metabuf->writeCount( &_value[0], utf8_size );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_write_wchar_t( Metabuf::Xml2Metabuf * _metabuf, const char * _value, void * _user )
    {
        (void)_user;

        size_t utf8_size = strlen( _value ); 

        _metabuf->write( utf8_size );

        _metabuf->writeCount( &_value[0], utf8_size );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	XmlToBinDecoder::XmlToBinDecoder()
        : m_serviceProvider(NULL)
        , m_stream(NULL)
	{
	}
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * XmlToBinDecoder::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterface * XmlToBinDecoder::getStream() const
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
	const XmlCodecDataInfo* XmlToBinDecoder::getCodecDataInfo() const
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool XmlToBinDecoder::initialize( ServiceProviderInterface * _serviceProvider, InputStreamInterface * _stream )
	{
        m_serviceProvider = _serviceProvider;
        m_stream = _stream;

		return true;
	}
	////////////////////////////////////////////////////////////////////////////
	unsigned int XmlToBinDecoder::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{
        (void)_buffer;
        (void)_bufferSize;

		Metabuf::XmlProtocol xml_protocol;

        WString unicode_pathProtocol;        
        if( Helper::utf8ToUnicodeSize( m_serviceProvider, m_options.pathProtocol.c_str(), m_options.pathProtocol.size(), unicode_pathProtocol ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error convert %s from utf8 to unicode"
                , m_options.pathProtocol.c_str()
                );

            return 0;
        }

		FILE * file_protocol = _wfopen( unicode_pathProtocol.c_str(), L"rb" );

        if( file_protocol == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error open protocol %ls"
                , unicode_pathProtocol.c_str()
                );

            return 0;
        }

		long size;

		fseek( file_protocol, 0, SEEK_END );
		size = ftell( file_protocol );
		fseek( file_protocol, 0, SEEK_SET );

		TBlobject buf(size);
		fread( &buf[0], 1, size, file_protocol );

		fclose( file_protocol );

		if( xml_protocol.readProtocol( &buf[0], size ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error read protocol %ls\n%s"
                , unicode_pathProtocol.c_str()
                , xml_protocol.getError().c_str()
                );

            return 0;
        }

		Metabuf::Xml2Metacode xml_metacode(&xml_protocol);

		std::string header;
		std::string source;
		xml_metacode.generate( header, source );

		FILE * file_metacode_hpp = _wfopen( L"Metacode.h", L"wb" );
        
        if( file_metacode_hpp == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error open Metacode.h"                
                );

            return 0;
        }

		fwrite( header.c_str(), header.size(), 1, file_metacode_hpp );
		fclose( file_metacode_hpp );

		FILE * file_metacode_cpp = _wfopen( L"Metacode.cpp", L"wb" );

        if( file_metacode_cpp == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error open Metacode.cpp"                
                );

            return 0;
        }
        
		fwrite( source.c_str(), source.size(), 1, file_metacode_cpp );
		fclose( file_metacode_cpp );

        WString unicode_pathXml;
        if( Helper::utf8ToUnicodeSize( m_serviceProvider, m_options.pathXml.c_str(), m_options.pathXml.size(), unicode_pathXml ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error convert %s from utf8 to unicode"
                , m_options.pathXml.c_str()
                );

            return 0;
        }
		
		FILE * file_test = _wfopen( unicode_pathXml.c_str(), L"rb" );

        if( file_test == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error open xml %ls"
                , unicode_pathXml.c_str()
                );

            return 0;
        }

		long size_test;

		fseek(file_test, 0, SEEK_END);
		size_test = ftell(file_test);
		fseek(file_test, 0, SEEK_SET);

        if( size_test == 0 )
        {
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error open xml %ls (file size == 0)"
                , unicode_pathXml.c_str()
                );

            return 0;
        }

        TBlobject buf_test(size_test);

        fread( &buf_test[0], 1, size_test, file_test );

		fclose( file_test );

		size_t write_size;

		TBlobject write_buff(size_test * 2);

		Metabuf::Xml2Metabuf xml_metabuf(&xml_protocol);

        LOGGER_INFO(m_serviceProvider)( "Xml2BinDecoder::decode:\nxml %s\nbin %s"
            , m_options.pathXml.c_str()
            , m_options.pathBin.c_str()
            );

        xml_metabuf.initialize();

        xml_metabuf.addSerializator( "wstring", &s_write_wstring, (void*)m_serviceProvider );
        xml_metabuf.addSerializator( "wchar_t", &s_write_wchar_t, (void*)m_serviceProvider );
		
		if( xml_metabuf.convert( &write_buff[0], size_test * 2, &buf_test[0], size_test, write_size ) == false )
		{
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error convert %ls\n%s"
                , unicode_pathXml.c_str()
                , xml_metabuf.getError().c_str()
                );

			return 0;
		}

        WString unicode_pathBin;        
        if( Helper::utf8ToUnicodeSize( m_serviceProvider, m_options.pathBin.c_str(), m_options.pathBin.size(), unicode_pathBin ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error convert %s from utf8 to unicode"
                , m_options.pathBin.c_str()
                );

            return 0;
        }

		FILE * file_test_bin = _wfopen( unicode_pathBin.c_str(), L"wb" );

        if( file_test == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "Xml2BinDecoder::decode: error create bin %ls"
                , unicode_pathBin.c_str()
                );

            return 0;
        }

		fwrite( &write_buff[0], write_size, 1, file_test_bin );
		fclose( file_test_bin );
		
		return 1;
	}
}