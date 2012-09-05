#	include "XmlToBinDecoder.h"

#	include "Utils/Logger/Logger.h"

#	include "xml2metabuf/Xml2Metabuf.hpp"
#	include "xml2metabuf/Xml2Metacode.hpp"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    static bool s_write_wstring( Metabuf::Xml2Metabuf * _metabuf, const char * _value, void * _user )
    {
        UnicodeServiceInterface * unicodeService = (UnicodeServiceInterface *)_user;
        
        size_t value_size = strlen( _value );

        size_t unicode_size;
        if( unicodeService->utf8ToUnicodeSize( _value, value_size, &unicode_size ) == false )
        {
            return false;
        }

        _metabuf->write( unicode_size );

        WString::value_type * buffer = new WString::value_type[unicode_size];

        size_t write_unicode_size;
        if( unicodeService->utf8ToUnicode( _value, value_size, buffer, unicode_size, &write_unicode_size ) == false )
        {
            return false;
        }

        if( unicode_size != write_unicode_size )
        {
            return false;
        }

        _metabuf->writeCount( buffer, unicode_size );

        delete [] buffer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static bool s_write_wchar_t( Metabuf::Xml2Metabuf * _metabuf, const char * _value, void * _user )
    {
        UnicodeServiceInterface * unicodeService = (UnicodeServiceInterface *)_user;

        size_t value_size = strlen( _value );

        WString::value_type wc[2];

        if( unicodeService->utf8ToUnicode( _value, value_size, wc, 2, 0 ) == false )
        {
            return false;
        }

        _metabuf->writeCount( wc, 1 );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	Xml2BinDecoder::Xml2BinDecoder( LogServiceInterface * _logService, UnicodeServiceInterface * _unicodeService )
		: m_logService(_logService)
        , m_unicodeService(_unicodeService)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Xml2BinDecoder::setOptions( CodecOptions * _options )
	{
		m_options = *static_cast<XmlCodecOptions *>(_options);
	}
	//////////////////////////////////////////////////////////////////////////
	const XmlCodecDataInfo* Xml2BinDecoder::getCodecDataInfo() const
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Xml2BinDecoder::initialize()
	{
		return true;
	}
	////////////////////////////////////////////////////////////////////////////
	unsigned int Xml2BinDecoder::decode( unsigned char* _buffer, unsigned int _bufferSize )
	{
		Metabuf::XmlProtocol xml_protocol;

		FILE * file_protocol = _wfopen( m_options.pathProtocol.c_str(), L"rb" );

        if( file_protocol == NULL )
        {
            LOGGER_ERROR(m_logService)( "Xml2BinDecoder::decode: error open protocol %S"
                , m_options.pathProtocol.c_str()
                );

            return 0;
        }

		long size;

		fseek(file_protocol, 0, SEEK_END);
		size = ftell(file_protocol);
		fseek(file_protocol, 0, SEEK_SET);

		std::vector<char> buf(size);

		fread( &buf[0], 1, size, file_protocol );

		fclose( file_protocol );

		if( xml_protocol.readProtocol( &buf[0], size ) == false )
        {
            LOGGER_ERROR(m_logService)( "Xml2BinDecoder::decode: error read protocol %S\n%s"
                , m_options.pathProtocol.c_str()
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
            LOGGER_ERROR(m_logService)( "Xml2BinDecoder::decode: error open Metacode.h"                
                );

            return 0;
        }

		fwrite( header.c_str(), header.size(), 1, file_metacode_hpp );
		fclose( file_metacode_hpp );

		FILE * file_metacode_cpp = _wfopen( L"Metacode.cpp", L"wb" );

        if( file_metacode_cpp == NULL )
        {
            LOGGER_ERROR(m_logService)( "Xml2BinDecoder::decode: error open Metacode.cpp"                
                );

            return 0;
        }
        
		fwrite( source.c_str(), source.size(), 1, file_metacode_cpp );
		fclose( file_metacode_cpp );
		
		FILE * file_test = _wfopen( m_options.pathXml.c_str(), L"rb" );

        if( file_test == NULL )
        {
            LOGGER_ERROR(m_logService)( "Xml2BinDecoder::decode: error open xml %S"
                , m_options.pathXml.c_str()
                );

            return 0;
        }

		long size_test;

		fseek(file_test, 0, SEEK_END);
		size_test = ftell(file_test);
		fseek(file_test, 0, SEEK_SET);

        std::vector<char> buf_test(size_test);

		fread( &buf_test[0], 1, size_test, file_test );

		fclose( file_test );

		size_t write_size;

		std::vector<char> write_buff(size_test * 2);

		Metabuf::Xml2Metabuf xml_metabuf(&write_buff[0], size_test * 2, &xml_protocol);

        xml_metabuf.addSerializator( "wstring", &s_write_wstring, (void*)m_unicodeService );
        xml_metabuf.addSerializator( "wchar_t", &s_write_wchar_t, (void*)m_unicodeService );

        LOGGER_INFO(m_logService)( "Xml2BinDecoder::decode:\nxml %S\nbin %S"
            , m_options.pathXml.c_str()
            , m_options.pathBin.c_str()
            );

		xml_metabuf.initialize();
		if( xml_metabuf.convert( &buf_test[0], size_test, write_size ) == false )
		{
            LOGGER_ERROR(m_logService)( "Xml2BinDecoder::decode: error %S\n%s"
                , m_options.pathXml.c_str()
                , xml_metabuf.getError().c_str()
                );

			return 0;
		}

		FILE * file_test_bin = _wfopen( m_options.pathBin.c_str(), L"wb" );

		fwrite( &write_buff[0], write_size, 1, file_test_bin );
		fclose( file_test_bin );
		
		return 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void Xml2BinDecoder::destroy()
	{
		delete this;
	}
}