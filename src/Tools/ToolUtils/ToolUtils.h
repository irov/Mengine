#	pragma once

#	ifdef _WIN32_WINNT	
#       undef _WIN32_WINNT
#       define _WIN32_WINNT 0x0500
#   endif

#   ifdef _WIN32_WINDOWS
#       undef _WIN32_WINDOWS
#       define _WIN32_WINDOWS 0x0500
#   endif

#	define WIN32_LEAN_AND_MEAN

#	include <Windows.h>

#	include <Shlwapi.h>
#	include <shellapi.h>

#	include <string>
#	include <sstream>
#	include <vector>

#	include <stdint.h>

//////////////////////////////////////////////////////////////////////////
void message_error( const char * _format, ... );
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void unicode_to_utf8( char * _utf8, size_t _capacity, const wchar_t * _unicode, size_t _size );
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, bool & _value );
void parse_arg( const std::wstring & _str, uint32_t & _value );
void parse_arg( const std::wstring & _str, float & _value );
void parse_arg( const std::wstring & _str, double & _value );
void parse_arg( const std::wstring & _str, std::wstring & _value );
//////////////////////////////////////////////////////////////////////////
template<class T>
static T parse_args( PWSTR lpCmdLine, uint32_t _index )
{
    int cmd_num;
    LPWSTR * cmd_args = CommandLineToArgvW( lpCmdLine, &cmd_num );

    wchar_t * arg_value = cmd_args[_index];

    std::wstring wstr_arg_value = arg_value;

    if( wstr_arg_value.front() == L'\"' && wstr_arg_value.back() == L'\"' )
    {
        wstr_arg_value = wstr_arg_value.substr( 1, wstr_arg_value.size() - 2 );
    }

    T value;
    parse_arg( wstr_arg_value, value );

    return value;
}
//////////////////////////////////////////////////////////////////////////
template<class T>
static T parse_kwds( PWSTR lpCmdLine, const wchar_t * _key, const T & _default )
{
    int cmd_num;
    LPWSTR * cmd_args = CommandLineToArgvW( lpCmdLine, &cmd_num );

    for( int i = 0; i != cmd_num; ++i )
    {
        wchar_t * arg = cmd_args[i + 0];

        if( wcscmp( arg, _key ) != 0 )
        {
            continue;
        }

        wchar_t * arg_value = cmd_args[i + 1];

        std::wstring wstr_arg_value = arg_value;

        if( wstr_arg_value.front() == L'\"' && wstr_arg_value.back() == L'\"' )
        {
            wstr_arg_value = wstr_arg_value.substr( 1, wstr_arg_value.size() - 2 );
        }

        T value;
        parse_arg( wstr_arg_value, value );

        return value;
    }

    return _default;
}
//////////////////////////////////////////////////////////////////////////
template<class T>
static bool parse_vector_kwds( PWSTR lpCmdLine, const wchar_t * _key, std::vector<T> & _values )
{
    int cmd_num;
    LPWSTR * cmd_args = CommandLineToArgvW( lpCmdLine, &cmd_num );

    for( int i = 0; i != cmd_num; ++i )
    {
        wchar_t * arg = cmd_args[i + 0];

        if( wcscmp( arg, _key ) != 0 )
        {
            continue;
        }

        wchar_t * arg_value = cmd_args[i + 1];

        std::wstring wstr_arg_value = arg_value;

        if( wstr_arg_value.front() == L'\"' && wstr_arg_value.back() == L'\"' )
        {
            wstr_arg_value = wstr_arg_value.substr( 1, wstr_arg_value.size() - 2 );
        }

        uint32_t arg_num;
        parse_arg( wstr_arg_value, arg_num );

        int arg_begin = i + 2;
        int arg_end = arg_begin + arg_num;
        for( int j = arg_begin; j != arg_end; ++j )
        {
            wchar_t * vector_arg_value = cmd_args[j];

            std::wstring wstr_vector_arg_value = vector_arg_value;

            if( wstr_vector_arg_value.front() == L'\"' && wstr_vector_arg_value.back() == L'\"' )
            {
                wstr_vector_arg_value = wstr_vector_arg_value.substr( 1, wstr_vector_arg_value.size() - 2 );
            }

            T value;
            parse_arg( wstr_vector_arg_value, value );

            _values.push_back( value );
        }

        return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
void ForcePathQuoteSpaces( WCHAR * _quotePath, const std::wstring & _path );
int ForceRemoveDirectory( LPCTSTR dir );
typedef std::vector<std::wstring> Files;
bool SelectFile( LPCTSTR dir, Files & _files );
uint8_t * ReadFileMemory( const WCHAR * _file, const WCHAR * _mode );