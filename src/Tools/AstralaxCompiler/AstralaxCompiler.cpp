#	include "ToolUtils/ToolUtils.h"

#   include "Interface/ParticleSystemInterface.h"

#	include "magic.h"

#   include "lz4.h"
#   include "lz4hc.h"

#	include <vector>
#	include <string>
#	include <sstream>

#	include "Core/Magic.h"
#	include "Logger/Logger.h"
#	include "ToolUtils/ToolUtils.h"

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
    std::wstring convert = parse_kwds( lpCmdLine, L"--convert", std::wstring() );
    
	if( in.empty() == true )
	{
		message_error("not found 'in' param\n"
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

    if( out == L"None" )
    {
        WCHAR tempPath[MAX_PATH];
        GetTempPath( MAX_PATH, tempPath );

        WCHAR tempFileName[MAX_PATH];
        GetTempFileName( tempPath, TEXT( "LibMovieAstralax" ), 0, tempFileName );

        out = tempFileName;
        out += L".ptc";

        convert.clear();
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

    WCHAR outCanonicalize[MAX_PATH];
    PathCanonicalize( outCanonicalize, out.c_str() );
    PathUnquoteSpaces( outCanonicalize );

    FILE * f = _wfopen( outCanonicalize, L"rb" );

    if( f == NULL )
    {
        message_error( "invalid _wfopen %ls\n"
            , outCanonicalize
        );

        return 0;
    }

    fseek( f, 0, SEEK_END );
    int f_size = ftell( f );
    rewind( f );

    if( f_size == 0 )
    {
        message_error( "invalid size %ls\n"
            , outCanonicalize
        );

        return 0;
    }

    std::vector<char> v_buffer( f_size );
    char * mf_buffer = &v_buffer[0];

    fread( mf_buffer, 1, f_size, f );
    fclose( f );

    HM_FILE mf = Magic_OpenFileInMemory( mf_buffer );

    if( mf == MAGIC_ERROR )
    {
        message_error( "invalid mf %ls MAGIC_ERROR\n"
            , outCanonicalize
        );

        return 0;
    }

    if( mf == MAGIC_UNKNOWN )
    {
        message_error( "invalid mf %ls MAGIC_UNKNOWN\n"
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

	if( info.empty() == false )
	{
        WCHAR infoCanonicalizeQuote[MAX_PATH];
        ForcePathQuoteSpaces( infoCanonicalizeQuote, info.c_str() );
        PathUnquoteSpaces( infoCanonicalizeQuote );

        FILE * f_info;
        errno_t err = _wfopen_s( &f_info, infoCanonicalizeQuote, L"wt" );

        if( err != 0 )
        {
            message_error( "invalid _wfopen %ls err %d\n"
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
            fprintf_s( f_info, "%d\n", 0 );
            fprintf_s( f_info, "%d\n", 0 );
			fprintf_s( f_info, "%d\n", desc.width );
			fprintf_s( f_info, "%d\n", desc.height );
		}
		
		fclose( f_info );
	}

    if( convert.empty() == false )
    {
        std::wstring::size_type ext_index = out.find_last_of( L"." );

        std::wstring outz = out.substr( 0, ext_index ) + L".ptz";

        WCHAR outzCanonicalize[MAX_PATH];
        PathCanonicalize( outzCanonicalize, outz.c_str() );
        PathUnquoteSpaces( outzCanonicalize );

        FILE * fz = _wfopen( outzCanonicalize, L"wb" );

        if( fz == NULL )
        {
            message_error( "invalid _wfopen %ls\n"
                , outzCanonicalize
            );

            return 0;
        }

        Menge::magic_number_type numberz = GET_MAGIC_NUMBER( Menge::MAGIC_PTZ );
        Menge::magic_version_type versionz = GET_MAGIC_VERSION( Menge::MAGIC_PTZ );

        fwrite( &numberz, sizeof( numberz ), 1, fz );
        fwrite( &versionz, sizeof( versionz ), 1, fz );

        uint32_t value_crc32 = 0;
        fwrite( &value_crc32, sizeof( value_crc32 ), 1, fz );

        uint32_t write_uncompressSize = (uint32_t)f_size;
        fwrite( &write_uncompressSize, sizeof( write_uncompressSize ), 1, fz );

        int lz_size = ::LZ4_compressBound( f_size );

        std::vector<char> lz4_buffer( lz_size );
        char * lz4_memory = &lz4_buffer[0];
        
        char * dst_buffer = (char *)lz4_memory;
        const char * src_buffer = (const char *)mf_buffer;

        int compressSize = ::LZ4_compress_HC( src_buffer, dst_buffer, f_size, lz_size, LZ4HC_CLEVEL_MAX );

        if( compressSize < 0 )
        {
            message_error( "invalid compress %ls\n"
                , outCanonicalize
            );

            return 0;
        }

        uint32_t write_compressSize = (uint32_t)compressSize;
        fwrite( &write_compressSize, sizeof( write_compressSize ), 1, fz );

        fwrite( dst_buffer, compressSize, 1, fz );
        fclose( fz );

        _wremove( outCanonicalize );
    }

	return 0;
}