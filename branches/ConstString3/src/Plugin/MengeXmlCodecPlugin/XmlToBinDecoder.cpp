#	include "XmlToBinDecoder.h"

#	include <Windows.h>

#	include "Utils/Logger/Logger.h"

#	include "XmlToBin.h"

#	include "xml2metabuf/Xml2Metabuf.hpp"
#	include "xml2metabuf/Xml2Metacode.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Xml2BinDecoder::Xml2BinDecoder( InputStreamInterface * _stream, LogServiceInterface * _logService )
		: m_stream(_stream)
		, m_logService(_logService)
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
	InputStreamInterface * Xml2BinDecoder::getStream() const
	{
		return m_stream;
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
            LOGGER_ERROR(m_logService)( "Xml2BinDecoder::decode: error read protocol %S"
                , m_options.pathProtocol.c_str()
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

extern "C" bool writeBinary( const wchar_t * _protocol, const wchar_t * _source, const wchar_t * _bin, char * _error )
{
	Metabuf::XmlProtocol xml_protocol;

	FILE * file_protocol = _wfopen( _protocol, L"rb" );

	if( file_protocol == NULL )
	{
		sprintf(_error,"Xml2BinDecoder::decode: error open protocol %S"
			, _protocol
			);

		return false;
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
		sprintf(_error,"Xml2BinDecoder::decode: error read protocol %S"
			, _protocol
			);

		return false;
	}

	Metabuf::Xml2Metacode xml_metacode(&xml_protocol);

	FILE * file_test = _wfopen( _source, L"rb" );

	if( file_test == NULL )
	{
		sprintf( _error, "Xml2BinDecoder::decode: error open xml %S"
			, _source
			);

		return false;
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

	xml_metabuf.initialize();
	if( xml_metabuf.convert( &buf_test[0], size_test, write_size ) == false )
	{
		sprintf( _error, "Xml2BinDecoder::decode: error\n%s"
			, xml_metabuf.getError().c_str()
			);

		return false;
	}

	FILE * file_test_bin = _wfopen( _bin, L"wb" );

	fwrite( &write_buff[0], write_size, 1, file_test_bin );
	fclose( file_test_bin );

	return true;
}