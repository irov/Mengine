#ifdef _WIN32_WINNT	
#   undef _WIN32_WINNT
#   define _WIN32_WINNT 0x0500
#endif

#ifdef _WIN32_WINDOWS
#   undef _WIN32_WINDOWS
#   define _WIN32_WINDOWS 0x0500
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "ToolUtils/ToolUtils.h"

#include <shellapi.h>

#include <stdio.h>

#include <string>
#include <vector>

#include "stdex/sha1.h"

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
		message_error( "FileSHA1 not found 'file' param"
			);

		return 0;
	}

	if( sha1.empty() == true )
	{
		message_error( "FileSHA1 not found 'sha1' param"
			);

		return 0;
	}

	HANDLE hFile = ::CreateFile( file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	
	if( hFile == INVALID_HANDLE_VALUE )
	{
		DWORD er = GetLastError();

		message_error( "FileSHA1 file '%ls' not open '%d'"
			, file.c_str()
			, er
			);

		return 0;
	}

	DWORD size = ::GetFileSize( hFile, NULL );

	if( size == INVALID_FILE_SIZE )
	{
		message_error( "FileSHA1 file '%ls' invalid get file size"
			, file.c_str()
			);

		return 0;
	}

	std::vector<unsigned char> buff(size);
	void * buff_memory = &buff[0];

	DWORD bytesRead = 0;
	if( ::ReadFile( hFile, buff_memory, size, &bytesRead, NULL ) == FALSE )
	{
		message_error( "FileSHA1 file '%ls' invalid read"
			, file.c_str()
			);
				
		return 0;
	}

	::CloseHandle( hFile );
	hFile = INVALID_HANDLE_VALUE;
	
	unsigned char hash[20];
	stdex::sha1_calc( buff_memory, size, hash );

	char hex[41];
	stdex::sha1_hex( hash, hex );
	
	HANDLE hSHA1 = ::CreateFile( sha1.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hSHA1 == INVALID_HANDLE_VALUE )
	{
		message_error( "FileSHA1 sha1 '%ls' not open"
			, sha1.c_str()
			);

		return 0;
	}

	DWORD bytesWritten = 0;
	if( ::WriteFile( hSHA1, hex, 40, &bytesWritten, NULL ) == FALSE )
	{
		message_error( "FileSHA1 sha1 '%ls' invalid write"
			, sha1.c_str()
			);

		return 0;
	}

	::CloseHandle( hSHA1 );
	hSHA1 = INVALID_HANDLE_VALUE;
	
	return 0;
}