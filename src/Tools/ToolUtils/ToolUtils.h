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

void message_error( const char * _format, ... );
//////////////////////////////////////////////////////////////////////////
void parse_arg( const std::wstring & _str, bool & _value );
void parse_arg( const std::wstring & _str, uint32_t & _value );
void parse_arg( const std::wstring & _str, float & _value );
void parse_arg( const std::wstring & _str, double & _value );
void parse_arg( const std::wstring & _str, std::wstring & _value );
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

		std::wstring wstr_value = arg_value;

		if( wstr_value.front() == L'\"' && wstr_value.back() == L'\"' )
		{
			wstr_value = wstr_value.substr( 1, wstr_value.size() - 2 );
		}

		T value;
		parse_arg( wstr_value, value );

		return value;
	}

	return _default;
}
//////////////////////////////////////////////////////////////////////////
void ForcePathQuoteSpaces( WCHAR * _quotePath, const std::wstring & _path );