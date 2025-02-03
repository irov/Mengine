#include "ToolUtils.h"

//////////////////////////////////////////////////////////////////////////
void message_error( const char * _format, ... )
{
    va_list argList;

    va_start( argList, _format );

    char str[2048 + 1] = {'\0'};
    ::vsnprintf( str, 2048, _format, argList );

    va_end( argList );

    ::printf( "%s", str);
    ::printf( "\n" );
}
//////////////////////////////////////////////////////////////////////////
void unicode_to_utf8( char * _utf8, size_t _capacity, const wchar_t * _unicode, size_t _size )
{
    DWORD dwConversionFlags = WC_ERR_INVALID_CHARS;

    int utf8_size = ::WideCharToMultiByte(
        CP_UTF8
        , dwConversionFlags
        , _unicode
        , (int)_size
        , _utf8
        , (int)_capacity
        , NULL
        , NULL
    );

    _utf8[utf8_size] = 0;
}
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, bool & _value )
{
    uint32_t value;
    ::swscanf( _str.c_str(), L"%u", &value );

    _value = (value != 0);
}
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, uint32_t & _value )
{
    uint32_t value;
    ::swscanf( _str.c_str(), L"%u", &value );

    _value = value;
}
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, float & _value )
{
    float value;
    ::swscanf( _str.c_str(), L"%g", &value );

    _value = value;
}
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, double & _value )
{
    double value;
    ::swscanf( _str.c_str(), L"%lf", &value );

    _value = value;
}
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, std::wstring & _value )
{
    _value = _str;
}
//////////////////////////////////////////////////////////////////////////
bool has_args( PWSTR lpCmdLine, const wchar_t * _key )
{
    int cmd_num;
    LPWSTR * cmd_args = ::CommandLineToArgvW( lpCmdLine, &cmd_num );

    for( int i = 0; i != cmd_num; ++i )
    {
        wchar_t * arg = cmd_args[i + 0];

        if( ::wcscmp( arg, _key ) != 0 )
        {
            continue;
        }

        return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
void ForcePathQuoteSpaces( WCHAR * _quotePath, const std::wstring & _path )
{
    if( _path.empty() == true )
    {
        ::wcscpy( _quotePath, L"" );

        return;
    }

    std::wstring true_path = _path;

    if( _path[0] == L'/' )
    {
        true_path[0] = true_path[1];
        true_path[1] = L':';
    }

    const WCHAR * pathBuffer = true_path.c_str();

    ::PathCanonicalize( _quotePath, pathBuffer );

    if( _quotePath[0] == L'\"' )
    {
        return;
    }

    size_t pathSize = ::wcslen( _quotePath );

    ::wmemmove( _quotePath + 1, _quotePath, pathSize );
    _quotePath[0] = '\"';
    _quotePath[pathSize + 1] = '\"';
    _quotePath[pathSize + 2] = 0;
};
//////////////////////////////////////////////////////////////////////////
int ForceRemoveDirectory( LPCTSTR dir )
{
    size_t len = ::wcslen( dir ) + 2; // required to set 2 nulls at end of argument to SHFileOperation.
    wchar_t * tempdir = (wchar_t *)::malloc( len * sizeof( wchar_t ) );
    ::memset( tempdir, 0, len * sizeof( wchar_t ) );
    ::wcscpy( tempdir, dir );

    SHFILEOPSTRUCT file_op = {
        NULL,
        FO_DELETE,
        tempdir,
        L"",
        FOF_NOCONFIRMATION |
        FOF_NOERRORUI |
        FOF_SILENT,
        false,
        0,
        L""};

    int ret = ::SHFileOperation( &file_op );

    ::free( tempdir );

    return ret;
}
//////////////////////////////////////////////////////////////////////////
bool SelectFile( LPCTSTR _dir, Files & _files )
{
    WIN32_FIND_DATA fd;
    HANDLE hFind = ::FindFirstFile( _dir, &fd );

    if( hFind == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    do
    {
        if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        {
            continue;
        }

        _files.push_back( fd.cFileName );
    } while( ::FindNextFile( hFind, &fd ) );

    ::FindClose( hFind );

    return true;
}
//////////////////////////////////////////////////////////////////////////
bool read_file_memory( const wchar_t * _file, uint8_t ** _buffer, size_t * const _size )
{
    FILE * f = ::_wfopen( _file, L"rb" );

    if( f == NULL )
    {
        return false;
    }

    ::fseek( f, 0, SEEK_END );
    long size = ::ftell( f );
    ::rewind( f );

    uint8_t * buffer = (uint8_t *)::malloc( size * sizeof( uint8_t ) );
    ::fread( buffer, size, 1, f );
    ::fclose( f );

    *_buffer = buffer;
    *_size = (size_t)size;

    return true;
}
//////////////////////////////////////////////////////////////////////////
bool write_file_memory( const wchar_t * _file, const char * _magic, const uint8_t * _buffer, size_t _size, size_t * const _write )
{
    FILE * f = ::_wfopen( _file, L"wb" );

    if( f == NULL )
    {
        return false;
    }

    size_t magic_size = ::strlen( _magic );
    size_t magic_write_size = ::fwrite( _magic, 1, magic_size, f );
    size_t buffer_write_size = ::fwrite( _buffer, 1, _size, f );

    ::fclose( f );

    size_t write_size = magic_write_size + buffer_write_size;

    *_write = write_size;

    if( write_size != magic_size + _size )
    {
        return false;
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
void free_file_memory( uint8_t * _buffer )
{
    ::free( _buffer );
}
//////////////////////////////////////////////////////////////////////////