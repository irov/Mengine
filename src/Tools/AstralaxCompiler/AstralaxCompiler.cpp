#	include "ToolUtils/ToolUtils.h"

#	include "magic.h"

#	include <vector>
#	include <string>
#	include <sstream>

//////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)nShowCmd;

	std::wstring astralax = parse_kwds( lpCmdLine, L"--astralax", std::wstring() );
	std::wstring in = parse_kwds( lpCmdLine, L"--in", std::wstring() );
	std::wstring out = parse_kwds( lpCmdLine, L"--out", std::wstring() );
	std::wstring csa = parse_kwds( lpCmdLine, L"--csa", std::wstring() );
	std::wstring info = parse_kwds( lpCmdLine, L"--info", std::wstring() );

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

		//std::string emitterName = getEmitterName( mf );

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
		//fprintf_s( f_info, "%s\n", emitterName );
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