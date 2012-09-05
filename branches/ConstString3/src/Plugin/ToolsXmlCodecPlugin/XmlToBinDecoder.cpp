#	include "XmlToBinDecoder.h"

#	include <Windows.h>

#	include "xml2metabuf/Xml2Metabuf.hpp"
#	include "xml2metabuf/Xml2Metacode.hpp"

//////////////////////////////////////////////////////////////////////////
static bool s_write_wstring( Metabuf::Xml2Metabuf * _metabuf, const char * _value, void * _user )
{
    int size = ::MultiByteToWideChar( CP_UTF8, 0, _value, -1, 0, 0 );

    if( size == 0 )
    {
        return false;
    }

    _metabuf->write( 1 );

    wchar_t * buffer = new wchar_t[1];
    int wc = ::MultiByteToWideChar( CP_UTF8, 0, _value, -1, buffer, 1 );

    if( wc != size )
    {
        return false;
    }

    _metabuf->writeCount( buffer, 1 );

    delete [] buffer;

    return true;
}
//////////////////////////////////////////////////////////////////////////
static bool s_write_wchar_t( Metabuf::Xml2Metabuf * _metabuf, const char * _value, void * _user )
{
    int size = ::MultiByteToWideChar( CP_UTF8, 0, _value, -1, 0, 0 );

    if( size == 0 )
    {
        return false;
    }

    wchar_t wch[2];
    int wc = ::MultiByteToWideChar( CP_UTF8, 0, _value, -1, wch, size );

    if( wc != size )
    {
        return false;
    }

    _metabuf->writeCount( wch, 1 );

    return true;
}
//////////////////////////////////////////////////////////////////////////
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

    xml_metabuf.addSerializator( "wstring", &s_write_wstring, 0 );
    xml_metabuf.addSerializator( "wchar_t", &s_write_wchar_t, 0 );

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