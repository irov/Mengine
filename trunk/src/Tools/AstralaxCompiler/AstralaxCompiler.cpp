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

#	include "magic.h"

#	include <vector>
#	include <string>
#	include <sstream>

static void ForcePathQuoteSpaces( WCHAR * _quotePath, const std::wstring & _path )
{
	const WCHAR * pathBuffer = _path.c_str();
	size_t pathSize = _path.size();

	PathCanonicalize( _quotePath, pathBuffer );
	if( PathQuoteSpaces( _quotePath ) == FALSE )
	{
		wmemmove( _quotePath + 1, _quotePath, pathSize );
		_quotePath[0] = '\"';
		_quotePath[pathSize + 1] = '\"';
		_quotePath[pathSize + 2] = 0;
	}
};

int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
	int cmd_num;
	LPWSTR * cmd_args = CommandLineToArgvW( lpCmdLine, &cmd_num );

	std::wstring astralax;
	std::wstring in;
	std::wstring out;
	std::wstring csa;
	std::wstring info;

	for( int i = 0; i < cmd_num; i += 2 )
	{
		LPWSTR arg = cmd_args[i + 0];
		LPWSTR value = cmd_args[i + 1];

		if( wcscmp( arg, L"-astralax") == 0 )
		{
			astralax = value;
		}
		else if( wcscmp( arg, L"-in" ) == 0 )
		{
			in = value;
		}
		else if( wcscmp( arg, L"-out" ) == 0 )
		{
			out = value;
		}
		else if( wcscmp( arg, L"-csa" ) == 0 )
		{
			csa = value;
		}
		else if( wcscmp( arg, L"-info" ) == 0 )
		{
			info = value;
		}
	}

	if( in.empty() == true )
	{
		printf("not found 'in' param\n"
			);

		return 1;
	}

	if( out.empty() == true )
	{
		printf("not found 'out' param\n"
			);

		return 1;
	}

	if( csa.empty() == true )
	{
		printf("not found 'csa' param\n"
			);

		return 1;
	}

	WCHAR szBuffer[512];
	if( astralax.empty() == true )
	{
		const WCHAR * regPath = L"Software\\Astralax\\Magic Particles (Dev) 2.24\\Path";

		HKEY openedKey;
		if( ::RegOpenKeyEx( HKEY_CURRENT_USER, L"Software\\Astralax\\Magic Particles (Dev) 2.24\\Path", 0, KEY_READ, &openedKey ) != ERROR_SUCCESS )
		{
			printf("invalid RegOpenKeyEx %ls\n"
				, regPath
				);

			return 1;
		}

		DWORD dwBufferSize = sizeof(szBuffer);
		if( RegQueryValueEx( openedKey, NULL, 0, NULL, (LPBYTE)szBuffer, &dwBufferSize) != ERROR_SUCCESS )
		{
			printf("invalid RegQueryValueEx %ls\n"
				, regPath
				);

			return 1;
		}
	}
	else
	{
		wcscpy_s( szBuffer, astralax.c_str() );
	}
	
	std::wstring system_cmd;

	system_cmd += L" /c ";

	WCHAR inCanonicalizeQuote[MAX_PATH];
	ForcePathQuoteSpaces( inCanonicalizeQuote, in );

	system_cmd += L" ";
	system_cmd += inCanonicalizeQuote;

	WCHAR outCanonicalizeQuote[MAX_PATH];
	ForcePathQuoteSpaces( outCanonicalizeQuote, out );
	
	system_cmd += L" ";
	system_cmd += outCanonicalizeQuote;
	
	WCHAR csaCanonicalizeQuote[MAX_PATH];
	ForcePathQuoteSpaces( csaCanonicalizeQuote, csa );

	system_cmd += L" ";
	system_cmd += csaCanonicalizeQuote;

	STARTUPINFO lpStartupInfo;
	ZeroMemory( &lpStartupInfo, sizeof(STARTUPINFOW) );
	lpStartupInfo.cb = sizeof(lpStartupInfo);

	PROCESS_INFORMATION lpProcessInformation;
	ZeroMemory( &lpProcessInformation, sizeof(PROCESS_INFORMATION) );

	WCHAR lpCommandLine[MAX_PATH];
	wcscpy_s( lpCommandLine, system_cmd.c_str() );

	if( CreateProcess( szBuffer
		, lpCommandLine
		, NULL
		, NULL
		, FALSE
		, BELOW_NORMAL_PRIORITY_CLASS
		, NULL
		, NULL
		, &lpStartupInfo
		, &lpProcessInformation ) == FALSE )
	{
		printf("invalid CreateProcess %ls %ls\n"
			, szBuffer
			, lpCommandLine
			);

		return 1;
	}

	CloseHandle( lpProcessInformation.hThread );

	WaitForSingleObject( lpProcessInformation.hProcess, INFINITE );

	DWORD exit_code;//Код завершения процесса
	GetExitCodeProcess( lpProcessInformation.hProcess, &exit_code );

	CloseHandle( lpProcessInformation.hProcess );

	if( exit_code != 0 )
	{
		printf("invalid Process %ls exit_code %d\n"
			, szBuffer
			, exit_code
			);

		return 1;
	}

	if( info.empty() == false )
	{
		WCHAR outCanonicalize[MAX_PATH];
		PathCanonicalize( outCanonicalize, out.c_str() );
		PathUnquoteSpaces( outCanonicalize );		

		FILE * f = _wfopen( outCanonicalize, L"rb" );

		if( f == NULL )
		{
			printf("invalid _wfopen %ls\n"
				, outCanonicalize
				);

			return 1;
		}

		fseek( f, 0, SEEK_END );
		int f_size = ftell( f );
		rewind( f );

		char * mf_buffer = new char [f_size];
		fread( mf_buffer, 1, f_size, f );
		fclose( f );

		HM_FILE mf = Magic_OpenFileInMemory( mf_buffer );

		if( mf == MAGIC_ERROR )
		{
			printf("invalid mf %ls MAGIC_ERROR\n"
				, outCanonicalize
				);

			return 1;
		}

		if( mf == MAGIC_UNKNOWN )
		{
			printf("invalid mf %ls MAGIC_UNKNOWN\n"
				, outCanonicalize
				);

			return 1;
		}

		int atlasCount = Magic_GetStaticAtlasCount( mf );

		typedef std::vector<std::string> TVectorAtlas;
		TVectorAtlas atlas;

		for( int i = 0; i != atlasCount; ++i )
		{
			MAGIC_STATIC_ATLAS magicAtlas;
			if( Magic_GetStaticAtlas( mf, i, &magicAtlas ) == MAGIC_ERROR )
			{
				continue;
			}

			atlas.push_back( magicAtlas.file );		
		}

		Magic_CloseFile( mf );

		WCHAR infoCanonicalize[MAX_PATH];
		PathCanonicalize( infoCanonicalize, info.c_str() );
		PathUnquoteSpaces( infoCanonicalize );

		FILE * f_info = _wfopen( infoCanonicalize, L"wt" );

		if( f_info == NULL )
		{
			printf("invalid _wfopen %ls\n"
				, infoCanonicalize
				);

			return 1;
		}

		CHAR s_atlasCount[16];
		sprintf_s( s_atlasCount, 16, "%d", atlasCount );

		fputs( s_atlasCount, f_info );
		fputs( "\n", f_info );

		for( TVectorAtlas::const_iterator
			it = atlas.begin(),
			it_end = atlas.end();
		it != it_end;
		++it )
		{
			const std::string & atlas_path = *it;

			fputs( atlas_path.c_str(), f_info );
			fputs( "\n", f_info );
		}

		fclose( f_info );
	}

	printf("complete\n"
		);
			
	return 0;
}