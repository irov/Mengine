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

#	include <shellapi.h>

#	include <stdio.h>

#	include <string>
#	include <vector>

#	include "sha1.h"

//////////////////////////////////////////////////////////////////////////
static void message_error( const wchar_t * _format, ... )
{
	va_list argList;

	va_start( argList, _format );

	wchar_t str[2048];

	_vsnwprintf( str, 2048 - 1, _format, argList );

	va_end( argList );

	MessageBoxW( NULL, str, L"FileSHA1", MB_OK );
}
//////////////////////////////////////////////////////////////////////////
static std::wstring s_correct_path( const std::wstring & _path )
{
	std::wstring true_path = _path;

	if( _path.size() <= 2 )
	{

	}
	else if( _path[0] == L'/' && _path[1] == L'/' )
	{

	}
	else if( _path[0] == L'/' )
	{
		true_path[0] = _path[1];
		true_path[1] = L':';
	}

	return true_path;
}
//////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)nShowCmd;

	int cmd_num;
	LPWSTR * cmd_args = CommandLineToArgvW( lpCmdLine, &cmd_num );

	std::wstring file;
	std::wstring sha1;

	for( int i = 0; i < cmd_num; i += 2 )
	{
		LPWSTR arg = cmd_args[i + 0];
		LPWSTR value = cmd_args[i + 1];

		if( wcscmp( arg, L"-file" ) == 0 )
		{
			file = value;
		}
		else if( wcscmp( arg, L"-sha1" ) == 0 )
		{
			sha1 = value;
		}
	}

	if( file.empty() == true )
	{
		message_error( L"not found 'file' param"
			);

		return 0;
	}

	if( sha1.empty() == true )
	{
		message_error( L"not found 'sha1' param"
			);

		return 0;
	}

	file = s_correct_path( file );
	sha1 = s_correct_path( sha1 );

	FILE * f_file = _wfopen( file.c_str(), L"rb" );

	if( f_file == NULL )
	{
		message_error( L"file '%ls' not open"
			, file.c_str()
			);

		return 0;
	}

	fseek( f_file, 0, SEEK_END );
	int size = ftell( f_file );
	rewind( f_file );

	
	std::vector<unsigned char> buff(size);
	void * buff_memory = &buff[0];

	fread( buff_memory, size, 1, f_file );
	fclose( f_file );
	
	unsigned char hash[20];
	sha1::calc( buff_memory, size, hash );

	char hex[41];
	sha1::toHexString( hash, hex );

	FILE * f_sha1 = _wfopen( sha1.c_str(), L"wt" );

	if( f_sha1 == NULL )
	{
		message_error( L"sha1 '%ls' not open"
			, file.c_str()
			);

		return 0;
	}

	fwrite( hex, 40, 1, f_sha1 );
	fclose( f_sha1 );
	
	return 0;
}