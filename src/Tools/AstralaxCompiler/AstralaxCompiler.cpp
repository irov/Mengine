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

static void message_error( const char * _format, ... )
{
	va_list argList;

	va_start(argList, _format);

	char str[2048];

	vsnprintf( str, 2048 - 1, _format, argList );

	va_end(argList);

	MessageBoxA( NULL, str, "AstralaxCompiler", MB_OK );
}

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

int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)nShowCmd;

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
		message_error("not found 'in' param\n"
			);

		return 0;
	}

	if( out.empty() == true )
	{
		message_error("not found 'out' param\n"
			);

		return 0;
	}

	if( csa.empty() == true )
	{
		message_error("not found 'csa' param\n"
			);

		return 0;
	}
	
	WCHAR szBuffer[MAX_PATH];
	if( astralax.empty() == true )
	{
		const WCHAR * regPath = L"Software\\Astralax\\Magic Particles 3D Path";

		HKEY openedKey;
		if( ::RegOpenKeyEx( HKEY_CURRENT_USER, regPath, 0, KEY_READ, &openedKey ) != ERROR_SUCCESS )
		{
			message_error("invalid RegOpenKeyEx %ls\n"
				, regPath
				);

			return 0;
		}

		DWORD dwBufferSize = sizeof(szBuffer);
		if( RegQueryValueEx( openedKey, NULL, 0, NULL, (LPBYTE)szBuffer, &dwBufferSize) != ERROR_SUCCESS )
		{
			message_error("invalid RegQueryValueEx %ls\n"
				, regPath
				);

			return 0;
		}

		wcscat( szBuffer, L"\\Particles.exe" );
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

	WCHAR lpCommandLine[32768];
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
		message_error("invalid CreateProcess %ls %ls\n"
			, szBuffer
			, lpCommandLine
			);

		return 0;
	}

	CloseHandle( lpProcessInformation.hThread );

	WaitForSingleObject( lpProcessInformation.hProcess, INFINITE );

	DWORD exit_code;//Код завершения процесса
	GetExitCodeProcess( lpProcessInformation.hProcess, &exit_code );

	CloseHandle( lpProcessInformation.hProcess );

	if( exit_code != 0 )
	{
		message_error("invalid Process %ls exit_code %d\n"
			, szBuffer
			, exit_code
			);

		return 0;
	}

	if( info.empty() == false )
	{
		WCHAR outCanonicalize[MAX_PATH];
		PathCanonicalize( outCanonicalize, out.c_str() );
		PathUnquoteSpaces( outCanonicalize );		

		FILE * f = _wfopen( outCanonicalize, L"rb" );

		if( f == NULL )
		{
			message_error("invalid _wfopen %ls\n"
				, outCanonicalize
				);

			return 0;
		}

		fseek( f, 0, SEEK_END );
		int f_size = ftell( f );
		rewind( f ); 

		std::vector<char> v_buffer(f_size);
		char * mf_buffer = &v_buffer[0];

		fread( mf_buffer, 1, f_size, f );
		fclose( f );

		HM_FILE mf = Magic_OpenFileInMemory( mf_buffer );

		if( mf == MAGIC_ERROR )
		{
			message_error("invalid mf %ls MAGIC_ERROR\n"
				, outCanonicalize
				);

			return 0;
		}

		if( mf == MAGIC_UNKNOWN )
		{
			message_error("invalid mf %ls MAGIC_UNKNOWN\n"
				, outCanonicalize
				);

			return 0;
		}

		int atlasCount = Magic_GetStaticAtlasCount( mf );

		struct AtlasDesc
		{
			std::string path;
			size_t width;
			size_t height;
		};

		typedef std::vector<AtlasDesc> TVectorAtlas;
		TVectorAtlas atlas;

		for( int i = 0; i != atlasCount; ++i )
		{
			MAGIC_STATIC_ATLAS magicAtlas;
			if( Magic_GetStaticAtlas( mf, i, &magicAtlas ) == MAGIC_ERROR )
			{
				continue;
			}

			AtlasDesc desc;
			desc.path = magicAtlas.file;
			desc.width = magicAtlas.width;
			desc.height = magicAtlas.height;

			atlas.push_back( desc );		
		}

		Magic_CloseFile( mf );

		WCHAR infoCanonicalizeQuote[MAX_PATH];
		ForcePathQuoteSpaces( infoCanonicalizeQuote, info.c_str() );
		PathUnquoteSpaces( infoCanonicalizeQuote );

		FILE * f_info; 
		errno_t err = _wfopen_s( &f_info, infoCanonicalizeQuote, L"wt" );

		if( err != 0 )
		{
			message_error("invalid _wfopen %ls err %d\n"
				, infoCanonicalizeQuote
				, err
				);

			return 0;
		}

		//CHAR s_atlasCount[16];
		//sprintf_s( s_atlasCount, 16, "%d", atlasCount );

		//fputs( s_atlasCount, f_info );
		//fputs( "\n", f_info );
		fprintf_s( f_info, "%d\n", atlasCount );

		for( TVectorAtlas::const_iterator
			it = atlas.begin(),
			it_end = atlas.end();
		it != it_end;
		++it )
		{
			const AtlasDesc & desc = *it;

			fprintf_s( f_info, "%s\n", desc.path.c_str() );

			fprintf_s( f_info, "%d\n", desc.width );
			fprintf_s( f_info, "%d\n", desc.height );
		}
		
		fclose( f_info );
	}
			
	return 0;
}