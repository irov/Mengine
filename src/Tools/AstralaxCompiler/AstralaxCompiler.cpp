#include "ToolUtils/ToolUtils.h"

#include "Plugins/AstralaxPlugin/AstralaxInterface.h"

#include "Kernel/Magic.h"
#include "Kernel/Logger.h"
#include "ToolUtils/ToolUtils.h"

#include "magic.h"

#include "lz4.h"
#include "lz4hc.h"

#include <vector>
#include <string>
#include <sstream>

//////////////////////////////////////////////////////////////////////////
extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    void * LZ4_malloc( size_t _size )
    {
        void * p = malloc( _size );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void * LZ4_calloc( size_t _count, size_t _size )
    {
        void * p = calloc( _count, _size );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void  LZ4_free( void * _ptr )
    {
        free( _ptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    MENGINE_UNUSED( argc );
    MENGINE_UNUSED( argv );

    PWSTR pwCmdLine = ::GetCommandLineW();

    std::wstring astralax_path = parse_kwds( pwCmdLine, L"--astralax_path", std::wstring() );
    std::wstring in_path = parse_kwds( pwCmdLine, L"--in_path", std::wstring() );
    std::wstring out_path = parse_kwds( pwCmdLine, L"--out_path", std::wstring() );
    std::wstring csa_path = parse_kwds( pwCmdLine, L"--csa_path", std::wstring() );
    std::wstring result_path = parse_kwds( pwCmdLine, L"--result_path", std::wstring() );
    std::wstring convert = parse_kwds( pwCmdLine, L"--convert", std::wstring() );

    if( in_path.empty() == true )
    {
        message_error( "not found 'in' param" );

        return EXIT_FAILURE;
    }

    if( csa_path.empty() == true )
    {
        message_error( "not found 'csa' param" );

        return EXIT_FAILURE;
    }

    WCHAR szBuffer[MAX_PATH];
    if( astralax_path.empty() == true )
    {
        const WCHAR * regPath = L"Software\\Astralax\\Magic Particles 3D Path";

        HKEY openedKey;
        if( ::RegOpenKeyEx( HKEY_CURRENT_USER, regPath, 0, KEY_READ, &openedKey ) != ERROR_SUCCESS )
        {
            message_error( "invalid RegOpenKeyEx '%ls'"
                , regPath
            );

            return EXIT_SUCCESS;
        }

        DWORD dwBufferSize = sizeof( szBuffer );
        if( RegQueryValueEx( openedKey, NULL, 0, NULL, (LPBYTE)szBuffer, &dwBufferSize ) != ERROR_SUCCESS )
        {
            message_error( "invalid RegQueryValueEx '%ls'"
                , regPath
            );

            return EXIT_FAILURE;
        }
    }
    else
    {
        wcscpy_s( szBuffer, astralax_path.c_str() );
    }

    if( out_path.empty() == true )
    {
        WCHAR tempPath[MAX_PATH];
        GetTempPath( MAX_PATH, tempPath );

        WCHAR tempFileName[MAX_PATH];
        GetTempFileName( tempPath, TEXT( "LibMovieAstralax" ), 0, tempFileName );

        out_path = tempFileName;
        out_path += L".ptc";

        convert.clear();
    }

    std::wstring system_cmd;

    system_cmd += L" /c ";

    WCHAR inCanonicalizeQuote[MAX_PATH];
    ForcePathQuoteSpaces( inCanonicalizeQuote, in_path );

    system_cmd += L" ";
    system_cmd += inCanonicalizeQuote;

    WCHAR outCanonicalizeQuote[MAX_PATH];
    ForcePathQuoteSpaces( outCanonicalizeQuote, out_path );

    system_cmd += L" ";
    system_cmd += outCanonicalizeQuote;

    WCHAR csaCanonicalizeQuote[MAX_PATH];
    ForcePathQuoteSpaces( csaCanonicalizeQuote, csa_path );

    system_cmd += L" ";
    system_cmd += csaCanonicalizeQuote;

    STARTUPINFO lpStartupInfo;
    ZeroMemory( &lpStartupInfo, sizeof( STARTUPINFOW ) );
    lpStartupInfo.cb = sizeof( lpStartupInfo );

    PROCESS_INFORMATION lpProcessInformation;
    ZeroMemory( &lpProcessInformation, sizeof( PROCESS_INFORMATION ) );

    WCHAR lpCommandLine[32768];
    wcscpy_s( lpCommandLine, system_cmd.c_str() );

    if( CreateProcess( szBuffer
        , lpCommandLine
        , NULL
        , NULL
        , FALSE
        , CREATE_NO_WINDOW
        , NULL
        , NULL
        , &lpStartupInfo
        , &lpProcessInformation ) == FALSE )
    {
        message_error( "invalid CreateProcess [%ls] command [%ls]"
            , szBuffer
            , lpCommandLine
        );

        return EXIT_FAILURE;
    }

    CloseHandle( lpProcessInformation.hThread );

    WaitForSingleObject( lpProcessInformation.hProcess, INFINITE );

    DWORD exit_code;
    GetExitCodeProcess( lpProcessInformation.hProcess, &exit_code );

    CloseHandle( lpProcessInformation.hProcess );

    if( exit_code != 0 )
    {
        message_error( "invalid Process '%ls' exit_code [%u]"
            , szBuffer
            , exit_code
        );

        return EXIT_FAILURE;
    }

    WCHAR outCanonicalize[MAX_PATH];
    PathCanonicalize( outCanonicalize, out_path.c_str() );
    PathUnquoteSpaces( outCanonicalize );

    FILE * f = _wfopen( outCanonicalize, L"rb" );

    if( f == NULL )
    {
        message_error( "invalid _wfopen %ls"
            , outCanonicalize
        );

        return EXIT_FAILURE;
    }

    fseek( f, 0, SEEK_END );
    long f_size = ftell( f );
    rewind( f );

    if( f_size == 0 )
    {
        fclose( f );

        message_error( "invalid size %ls"
            , outCanonicalize
        );

        return EXIT_FAILURE;
    }

    std::vector<char> v_buffer( f_size );
    char * mf_buffer = &v_buffer[0];

    fread( mf_buffer, 1, f_size, f );
    fclose( f );

    HM_FILE mf = Magic_OpenFileInMemory( mf_buffer );

    if( mf == MAGIC_ERROR )
    {
        message_error( "invalid mf %ls MAGIC_ERROR"
            , outCanonicalize
        );

        return EXIT_FAILURE;
    }

    if( mf == MAGIC_UNKNOWN )
    {
        message_error( "invalid mf %ls MAGIC_UNKNOWN"
            , outCanonicalize
        );

        return EXIT_FAILURE;
    }

    int atlasCount = Magic_GetStaticAtlasCount( mf );

    struct AtlasDesc
    {
        std::string path;
        int32_t width;
        int32_t height;
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

    if( result_path.empty() == false )
    {
        WCHAR infoCanonicalizeQuote[MAX_PATH];
        ForcePathQuoteSpaces( infoCanonicalizeQuote, result_path.c_str() );
        PathUnquoteSpaces( infoCanonicalizeQuote );

        FILE * f_result;
        errno_t err = _wfopen_s( &f_result, infoCanonicalizeQuote, L"wt" );

        if( err != 0 )
        {
            message_error( "invalid _wfopen %ls err %d"
                , infoCanonicalizeQuote
                , err
            );

            return EXIT_FAILURE;
        }


        fprintf_s( f_result, "%d\n", atlasCount );

        for( const AtlasDesc & desc : atlas )
        {
            fprintf_s( f_result, "%s\n", desc.path.c_str() );
            fprintf_s( f_result, "%d\n", 0 );
            fprintf_s( f_result, "%d\n", 0 );
            fprintf_s( f_result, "%d\n", desc.width );
            fprintf_s( f_result, "%d\n", desc.height );
        }

        fclose( f_result );
    }

    if( convert.empty() == false )
    {
        std::wstring::size_type ext_index = out_path.find_last_of( L"." );

        std::wstring outz = out_path.substr( 0, ext_index ) + L".ptz";

        WCHAR outzCanonicalize[MAX_PATH];
        PathCanonicalize( outzCanonicalize, outz.c_str() );
        PathUnquoteSpaces( outzCanonicalize );

        FILE * fz = _wfopen( outzCanonicalize, L"wb" );

        if( fz == NULL )
        {
            message_error( "invalid _wfopen %ls"
                , outzCanonicalize
            );

            return EXIT_FAILURE;
        }

        Mengine::magic_number_type numberz = GET_MAGIC_NUMBER( Mengine::MAGIC_PTZ );
        Mengine::magic_version_type versionz = GET_MAGIC_VERSION( Mengine::MAGIC_PTZ );

        fwrite( &numberz, sizeof( numberz ), 1, fz );
        fwrite( &versionz, sizeof( versionz ), 1, fz );

        uint32_t value_crc32 = 0;
        fwrite( &value_crc32, sizeof( value_crc32 ), 1, fz );

        uint32_t write_uncompressSize = (uint32_t)f_size;
        fwrite( &write_uncompressSize, sizeof( write_uncompressSize ), 1, fz );

        int lz_size = ::LZ4_compressBound( f_size );

        char * lz4_memory = new char[lz_size];

        char * dst_buffer = (char *)lz4_memory;
        const char * src_buffer = (const char *)mf_buffer;

        int compressSize = ::LZ4_compress_HC( src_buffer, dst_buffer, f_size, lz_size, LZ4HC_CLEVEL_MAX );

        if( compressSize < 0 )
        {
            message_error( "invalid compress %ls"
                , outCanonicalize
            );

            return EXIT_FAILURE;
        }

        uint32_t write_compressSize = (uint32_t)compressSize;
        fwrite( &write_compressSize, sizeof( write_compressSize ), 1, fz );

        fwrite( dst_buffer, compressSize, 1, fz );
        fclose( fz );

        delete [] lz4_memory;

        _wremove( outCanonicalize );
    }

    return EXIT_SUCCESS;
}