#pragma once

#if defined(_WIN32)
#   include "Environment/Windows/WindowsIncluder.h"
#else
#   include <limits.h>
#   include <stdio.h>

using WCHAR = wchar_t;
using CHAR = char;
using LPCTSTR = const wchar_t *;
using errno_t = int;

#   ifndef MAX_PATH
#       define MAX_PATH PATH_MAX
#   endif

FILE * _wfopen( const wchar_t * _path, const wchar_t * _mode );
errno_t _wfopen_s( FILE ** _file, const wchar_t * _path, const wchar_t * _mode );
int _wremove( const wchar_t * _path );
int PathCanonicalize( wchar_t * _destination, const wchar_t * _source );
void PathUnquoteSpaces( wchar_t * _path );

#   define fprintf_s fprintf
#endif

#include <string>
#include <sstream>
#include <vector>

#include <stdint.h>

//////////////////////////////////////////////////////////////////////////
void message_error( const char * _format, ... );
//////////////////////////////////////////////////////////////////////////
size_t unicode_to_utf8( char * _utf8, size_t _capacity, const wchar_t * _unicode, size_t _size );
std::string unicode_to_utf8( const std::wstring & _unicode );
std::wstring utf8_to_unicode( const std::string & _utf8 );
std::wstring path_join( const std::wstring & _left, const std::wstring & _right );
std::wstring path_parent( const std::wstring & _path );
std::wstring get_temporary_directory();
bool create_directories( const std::wstring & _path );
bool execute_process( const std::wstring & _executable, const std::vector<std::wstring> & _arguments, const std::wstring & _logPath, int * const _exitCode );
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, bool & _value );
void parse_arg( const std::wstring & _str, uint32_t & _value );
void parse_arg( const std::wstring & _str, float & _value );
void parse_arg( const std::wstring & _str, double & _value );
void parse_arg( const std::wstring & _str, std::wstring & _value );
//////////////////////////////////////////////////////////////////////////
bool has_args( const wchar_t * _key );
const std::vector<std::wstring> & get_command_line_arguments();
//////////////////////////////////////////////////////////////////////////
template<class T>
static T parse_args( uint32_t _index )
{
    const std::vector<std::wstring> & arguments = get_command_line_arguments();

    if( _index >= arguments.size() )
    {
        return T();
    }

    std::wstring wstr_arg_value = arguments[_index];

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
T parse_kwds( const wchar_t * _key, const T & _default )
{
    const std::vector<std::wstring> & arguments = get_command_line_arguments();

    for( size_t i = 0; i != arguments.size(); ++i )
    {
        if( arguments[i] != _key )
        {
            continue;
        }

        if( i + 1 >= arguments.size() )
        {
            return _default;
        }

        std::wstring wstr_arg_value = arguments[i + 1];

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
static bool parse_vector_kwds( const wchar_t * _key, std::vector<T> & _values )
{
    const std::vector<std::wstring> & arguments = get_command_line_arguments();

    for( size_t i = 0; i != arguments.size(); ++i )
    {
        if( arguments[i] != _key )
        {
            continue;
        }

        if( i + 1 >= arguments.size() )
        {
            return false;
        }

        std::wstring wstr_arg_value = arguments[i + 1];

        if( wstr_arg_value.front() == L'\"' && wstr_arg_value.back() == L'\"' )
        {
            wstr_arg_value = wstr_arg_value.substr( 1, wstr_arg_value.size() - 2 );
        }

        uint32_t arg_num;
        parse_arg( wstr_arg_value, arg_num );

        size_t arg_begin = i + 2;
        size_t arg_end = arg_begin + arg_num;

        if( arg_end > arguments.size() )
        {
            return false;
        }

        for( size_t j = arg_begin; j != arg_end; ++j )
        {
            std::wstring wstr_vector_arg_value = arguments[j];

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
//////////////////////////////////////////////////////////////////////////
bool read_file_to_memory( const wchar_t * _path, uint8_t * _buffer, size_t _capacity, size_t * const _size );
bool read_file_memory( const wchar_t * _path, uint8_t ** _buffer, size_t * const _size );
bool write_file_memory( const wchar_t * _path, const uint8_t * _buffer, size_t _size );
bool move_file_memory( const wchar_t * _path, const uint8_t * _buffer, size_t _size );
bool write_file_magic_memory( const wchar_t * _path, const char * _magic, const uint8_t * _buffer, size_t _size, size_t * const _write );
bool move_file_magic_memory( const wchar_t * _path, const char * _magic, const uint8_t * _buffer, size_t _size, size_t * const _write );
//////////////////////////////////////////////////////////////////////////
