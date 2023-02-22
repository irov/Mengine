#include "ToolUtils/ToolUtils.h"

#include "Config/Config.h"

#include "pugixml.hpp"

#include <vector>
#include <string>
#include <sstream>

//////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
    MENGINE_UNUSED( hInstance );
    MENGINE_UNUSED( hPrevInstance );
    MENGINE_UNUSED( lpCmdLine );
    MENGINE_UNUSED( nShowCmd );

    PWSTR pwCmdLine = ::GetCommandLineW();

    std::wstring texturepacker_path = parse_kwds( pwCmdLine, L"--texturepacker", std::wstring() );
    std::wstring in_path = parse_kwds( pwCmdLine, L"--in_path", std::wstring() );
    std::wstring result_path = parse_kwds( pwCmdLine, L"--result_path", std::wstring() );
    uint32_t offset_x = parse_kwds( pwCmdLine, L"--offset_x", 0U );
    uint32_t offset_y = parse_kwds( pwCmdLine, L"--offset_y", 0U );
    float width = parse_kwds( pwCmdLine, L"--width", -1.f );
    float height = parse_kwds( pwCmdLine, L"--height", -1.f );
    uint32_t tolerance = parse_kwds( pwCmdLine, L"--tolerance", 200U );

    if( texturepacker_path.empty() == true )
    {
        message_error( "not found 'texturepacker' param" );

        return 0;
    }

    if( in_path.empty() == true )
    {
        message_error( "not found 'image' param" );

        return 0;
    }

    std::wstring system_cmd;

    system_cmd += L" --shape-padding 0 ";
    system_cmd += L" --border-padding 0 ";
    system_cmd += L" --padding 0 ";
    system_cmd += L" --disable-rotation ";
    system_cmd += L" --extrude 0 ";
    system_cmd += L" --trim-mode Polygon ";
    system_cmd += L" --trim-threshold 0 ";
    system_cmd += L" --tracer-tolerance ";
    system_cmd += L" --max-width 8192 ";
    system_cmd += L" --max-height 8192 ";
    system_cmd += L" --max-size 8192 ";

    std::wstringstream ss;
    ss << tolerance;
    system_cmd += ss.str();
    system_cmd += L" ";

    WCHAR ImagePathCanonicalizeQuote[MAX_PATH];
    ForcePathQuoteSpaces( ImagePathCanonicalizeQuote, in_path );

    system_cmd += ImagePathCanonicalizeQuote;

    WCHAR tempPath[MAX_PATH];
    GetTempPath( MAX_PATH, tempPath );

    WCHAR outCanonicalize[MAX_PATH];
    PathCombine( outCanonicalize, tempPath, L"aemovie_temp_texturepacker_sheet.xml" );
    //PathCanonicalize( outCanonicalize, L"%TEMP%/temp_texturepacker_sheet.xml" );
    //PathUnquoteSpaces( outCanonicalize );

    system_cmd += L" --data ";
    system_cmd += outCanonicalize;
    system_cmd += L" --format xml ";
    system_cmd += L" --quiet ";

    STARTUPINFO lpStartupInfo;
    ZeroMemory( &lpStartupInfo, sizeof( STARTUPINFOW ) );
    lpStartupInfo.cb = sizeof( lpStartupInfo );

    PROCESS_INFORMATION lpProcessInformation;
    ZeroMemory( &lpProcessInformation, sizeof( PROCESS_INFORMATION ) );

    WCHAR lpCommandLine[32768];
    wcscpy_s( lpCommandLine, system_cmd.c_str() );

    WCHAR TexturePathCanonicalizeQuote[MAX_PATH];
    ForcePathQuoteSpaces( TexturePathCanonicalizeQuote, texturepacker_path );
    PathUnquoteSpaces( TexturePathCanonicalizeQuote );

    if( CreateProcess( TexturePathCanonicalizeQuote
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
        message_error( "invalid CreateProcess %ls %ls"
            , TexturePathCanonicalizeQuote
            , lpCommandLine
        );

        return 0;
    }

    CloseHandle( lpProcessInformation.hThread );

    WaitForSingleObject( lpProcessInformation.hProcess, INFINITE );

    DWORD exit_code;
    GetExitCodeProcess( lpProcessInformation.hProcess, &exit_code );

    CloseHandle( lpProcessInformation.hProcess );

    if( exit_code != 0 )
    {
        message_error( "invalid Process %ls exit_code %d"
            , TexturePathCanonicalizeQuote
            , exit_code
        );

        return 0;
    }

    FILE * f = _wfopen( outCanonicalize, L"rb" );

    if( f == NULL )
    {
        message_error( "invalid _wfopen %ls"
            , outCanonicalize
        );

        return 0;
    }

    fseek( f, 0, SEEK_END );
    long f_size = ftell( f );
    rewind( f );

    std::vector<char> v_buffer( f_size );
    char * mf_buffer = &v_buffer[0];

    fread( mf_buffer, 1, f_size, f );
    fclose( f );

    pugi::xml_document doc;

    pugi::xml_parse_result result = doc.load_buffer( mf_buffer, f_size );

    pugi::xml_node xml_sprite = doc.first_element_by_path( "TextureAtlas/sprite" );
    pugi::xml_attribute xml_sprite_x = xml_sprite.attribute( "x" );
    pugi::xml_attribute xml_sprite_y = xml_sprite.attribute( "y" );
    pugi::xml_attribute xml_sprite_w = xml_sprite.attribute( "w" );
    pugi::xml_attribute xml_sprite_h = xml_sprite.attribute( "h" );
    pugi::xml_attribute xml_sprite_oX = xml_sprite.attribute( "oX" );
    pugi::xml_attribute xml_sprite_oY = xml_sprite.attribute( "oY" );
    pugi::xml_attribute xml_sprite_oW = xml_sprite.attribute( "oW" );
    pugi::xml_attribute xml_sprite_oH = xml_sprite.attribute( "oH" );

    uint32_t x = xml_sprite_x.as_uint();
    uint32_t y = xml_sprite_y.as_uint();
    uint32_t w = xml_sprite_w.as_uint();
    uint32_t h = xml_sprite_h.as_uint();
    uint32_t oX = xml_sprite_oX.as_uint();
    uint32_t oY = xml_sprite_oY.as_uint();
    uint32_t oW = xml_sprite_oW.as_uint();
    uint32_t oH = xml_sprite_oH.as_uint();

    //TODO
    MENGINE_UNUSED( oW );
    MENGINE_UNUSED( oH );

    pugi::xml_node xml_vertices = doc.first_element_by_path( "TextureAtlas/sprite/vertices" );

    const char * vertices = xml_vertices.child_value();

    std::stringstream ss_vertices( vertices );

    std::vector<float> positions;

    for( ;; )
    {
        int32_t pos_x;
        int32_t pos_y;
        if( ss_vertices >> pos_x && ss_vertices >> pos_y )
        {
            pos_x -= x;
            pos_y -= y;

            pos_x += offset_x;
            pos_y += offset_y;

            float xf = (float)pos_x;
            float yf = (float)pos_y;

            positions.push_back( xf );
            positions.push_back( yf );
        }
        else
        {
            break;
        }
    }

    pugi::xml_node xml_verticesUV = doc.first_element_by_path( "TextureAtlas/sprite/verticesUV" );

    const char * verticesUV = xml_verticesUV.child_value();

    if( width < 0.f )
    {
        width = (float)w;
    }

    if( height < 0.f )
    {
        height = (float)h;
    }

    std::stringstream ss_verticesUV( verticesUV );

    std::vector<float> uvs;

    for( ;; )
    {
        uint32_t uv_x;
        uint32_t uv_y;
        if( ss_verticesUV >> uv_x && ss_verticesUV >> uv_y )
        {
            uv_x -= x;
            uv_y -= y;
            uv_x += oX;
            uv_y += oY;

            float xf = (float)uv_x / (float)width;
            float yf = (float)uv_y / (float)height;

            uvs.push_back( xf );
            uvs.push_back( yf );
        }
        else
        {
            break;
        }
    }

    pugi::xml_node xml_triangles = doc.first_element_by_path( "TextureAtlas/sprite/triangles" );

    const char * triangles = xml_triangles.child_value();

    std::stringstream ss_triangles( triangles );

    std::vector<uint16_t> indices;

    for( ;; )
    {
        uint16_t index;
        if( ss_triangles >> index )
        {
            indices.push_back( index );
        }
        else
        {
            break;
        }
    }

    uint32_t vertex_count = (uint32_t)positions.size() / 2;
    uint32_t index_count = (uint32_t)indices.size();

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

        return 0;
    }

    fprintf_s( f_result, "%u\n", vertex_count );
    fprintf_s( f_result, "%u\n", index_count );

    fprintf_s( f_result, "" );

    for( float v : positions )
    {
        fprintf_s( f_result, " %12f", v );
    }

    fprintf_s( f_result, "\n" );

    fprintf_s( f_result, "" );

    for( float v : uvs )
    {
        fprintf_s( f_result, " %12f", v );
    }

    fprintf_s( f_result, "\n" );

    fprintf_s( f_result, "" );

    for( uint16_t v : indices )
    {
        fprintf_s( f_result, " %u", v );
    }

    fprintf_s( f_result, "\n" );

    fclose( f_result );

    return 0;
}