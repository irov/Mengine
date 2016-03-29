#	include <Windows.h>

#	include <shellapi.h>

#	include <stdio.h>

#	include <string>
#	include <vector>

#	include "sha1.h"

//////////////////////////////////////////////////////////////////////////
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
		message_error( "not found 'file' param"
			);

		return 0;
	}

	if( sha1.empty() == true )
	{
		message_error( "not found 'sha1' param"
			);

		return 0;
	}

	HANDLE hFile = ::CreateFile( file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	
	if( hFile == INVALID_HANDLE_VALUE )
	{
		DWORD er = GetLastError();

		message_error( "file '%ls' not open '%d'"
			, file.c_str()
			, er
			);

		return 0;
	}

	DWORD size = ::GetFileSize( hFile, NULL );

	std::vector<unsigned char> buff(size);
	void * buff_memory = &buff[0];

	DWORD bytesRead = 0;
	if( ::ReadFile( hFile, buff_memory, size, &bytesRead, NULL ) == FALSE )
	{
		message_error( "file '%ls' invalid read"
			, file.c_str()
			);

		return 0;
	}

	::CloseHandle( hFile );
	hFile = INVALID_HANDLE_VALUE;
	
	unsigned char hash[20];
	sha1::calc( buff_memory, size, hash );

	char hex[41];
	sha1::toHexString( hash, hex );
	
	HANDLE hSHA1 = ::CreateFile( sha1.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hSHA1 == INVALID_HANDLE_VALUE )
	{
		message_error( "sha1 '%ls' not open"
			, sha1.c_str()
			);

		return 0;
	}

	DWORD bytesWritten = 0;
	if( ::WriteFile( hSHA1, hex, 40, &bytesWritten, NULL ) == FALSE )
	{
		message_error( "sha1 '%ls' invalid write"
			, sha1.c_str()
			);

		return 0;
	}

	::CloseHandle( hSHA1 );
	hSHA1 = INVALID_HANDLE_VALUE;
	
	return 0;
}