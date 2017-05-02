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

static void message_error( const char * _format, ... )
{
	va_list argList;

	va_start( argList, _format );

	char str[2048];

	vsnprintf( str, 2048 - 1, _format, argList );

	va_end( argList );

	printf( str );
}
//////////////////////////////////////////////////////////////////////////
static void parse_arg( const std::wstring & _str, bool & _value )
{
	uint32_t value;
	swscanf( _str.c_str(), L"%u", &value );

	_value = (value != 0);
}
//////////////////////////////////////////////////////////////////////////
static void parse_arg( const std::wstring & _str, uint32_t & _value )
{
	uint32_t value;
	swscanf( _str.c_str(), L"%u", &value );

	_value = value;
}
//////////////////////////////////////////////////////////////////////////
static void parse_arg( const std::wstring & _str, float & _value )
{
	float value;
	swscanf( _str.c_str(), L"%f", &value );

	_value = value;
}
//////////////////////////////////////////////////////////////////////////
static void parse_arg( const std::wstring & _str, double & _value )
{
	double value;
	swscanf( _str.c_str(), L"%lf", &value );

	_value = value;
}
//////////////////////////////////////////////////////////////////////////
static void parse_arg( const std::wstring & _str, std::wstring & _value )
{
	_value = _str;
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
static void ForcePathQuoteSpaces( WCHAR * _quotePath, const std::wstring & _path )
{
	if( _path.empty() == true )
	{
		wcscpy_s( _quotePath, 2, L"" );

		return;
	}

	std::wstring true_path = _path;

	if( _path[0] == L'/' )
	{
		true_path[0] = true_path[1];
		true_path[1] = L':';
	}

	const WCHAR * pathBuffer = true_path.c_str();
	size_t pathSize = true_path.size();

	PathCanonicalize( _quotePath, pathBuffer );
	if( PathQuoteSpaces( _quotePath ) == FALSE )
	{
		wmemmove( _quotePath + 1, _quotePath, pathSize );
		_quotePath[0] = '\"';
		_quotePath[pathSize + 1] = '\"';
		_quotePath[pathSize + 2] = 0;
	}
};