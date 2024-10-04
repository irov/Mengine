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

#include "Config/Config.h"

#include <Shlwapi.h>
#include <shellapi.h>

#include <vector>
#include <string>
#include <sstream>

#include <stdint.h>
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
static const uint32_t DDS_MAGIC = 0x20534444;

static const uint32_t DDSD_CAPS = 0x00000001;
static const uint32_t DDSD_HEIGHT = 0x00000002;
static const uint32_t DDSD_WIDTH = 0x00000004;
static const uint32_t DDSD_PITCH = 0x00000008;
static const uint32_t DDSD_PIXELFORMAT = 0x00001000;
static const uint32_t DDSD_MIPMAPCOUNT = 0x00020000;
static const uint32_t DDSD_LINEARSIZE = 0x00080000;
static const uint32_t DDSD_DEPTH = 0x00800000;

//
// DDPIXELFORMAT flags
//
static const uint32_t DDPF_ALPHAPIXELS = 0x00000001l;
static const uint32_t DDPF_FOURCC = 0x00000004l;        // Compressed formats
static const uint32_t DDPF_RGB = 0x00000040l;        // Uncompressed formats
static const uint32_t DDPF_ALPHA = 0x00000002l;
static const uint32_t DDPF_COMPRESSED = 0x00000080l;
static const uint32_t DDPF_LUMINANCE = 0x00020000l;
static const uint32_t DDPF_BUMPLUMINANCE = 0x00040000l;        // L,U,V
static const uint32_t DDPF_BUMPDUDV = 0x00080000l;        // U,V

#pragma pack( push, 1 )
struct DDS_PIXELFORMAT
{
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwFourCC;
    uint32_t dwRGBBitCount;
    uint32_t dwRBitMask;
    uint32_t dwGBitMask;
    uint32_t dwBBitMask;
    uint32_t dwABitMask;
};

typedef struct
{
    uint32_t           dwSize;
    uint32_t           dwFlags;
    uint32_t           dwHeight;
    uint32_t           dwWidth;
    uint32_t           dwPitchOrLinearSize;
    uint32_t           dwDepth;
    uint32_t           dwMipMapCount;
    uint32_t           dwReserved1[11];
    DDS_PIXELFORMAT    ddspf;
    uint32_t           dwSurfaceFlags;
    uint32_t           dwCubemapFlags;
    uint32_t           dwReserved2[3];
} DDS_HEADER;
#pragma pack(pop)
//////////////////////////////////////////////////////////////////////////
#   define FOURCC(c0, c1, c2, c3) (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24))
//////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
    MENGINE_UNUSED( hInstance );
    MENGINE_UNUSED( hPrevInstance );
    MENGINE_UNUSED( lpCmdLine );
    MENGINE_UNUSED( nShowCmd );

    PWSTR pwCmdLine = ::GetCommandLineW();

    int cmd_num;
    LPWSTR * cmd_args = ::CommandLineToArgvW( pwCmdLine, &cmd_num );

    std::wstring in;
    std::wstring out;

    for( int i = 0; i < cmd_num; i += 2 )
    {
        LPWSTR arg = cmd_args[i + 0];
        LPWSTR value = cmd_args[i + 1];

        if( wcscmp( arg, L"-in" ) == 0 )
        {
            in = value;
        }
        else if( wcscmp( arg, L"-out" ) == 0 )
        {
            out = value;
        }
    }

    if( in.empty() == true )
    {
        message_error( "not found 'in' param" );

        return EXIT_FAILURE;
    }

    if( out.empty() == true )
    {
        message_error( "not found 'out' param" );

        return EXIT_FAILURE;
    }

    WCHAR inCanonicalizeQuote[MAX_PATH] = {L'\0'};
    ::ForcePathQuoteSpaces( inCanonicalizeQuote, in.c_str() );
    ::PathUnquoteSpaces( inCanonicalizeQuote );

    FILE * file_in = _wfopen( inCanonicalizeQuote, L"rb" );

    uint32_t magic;
    fread( &magic, sizeof( magic ), 1, file_in );

    if( magic != FOURCC( 'D', 'D', 'S', ' ' ) )
    {
        message_error( "ExtractDXT1 %ls invalid dds magic"
            , in.c_str()
        );

        return EXIT_FAILURE;
    }

    DDS_HEADER header;
    fread( &header, sizeof( header ), 1, file_in );

    //Check valid structure sizes
    if( header.dwSize != 124 && header.ddspf.dwSize != 32 )
    {
        message_error( "ExtractDXT1 %ls invalid dds file header"
            , in.c_str()
        );

        return 1;
    }

    if( (header.dwFlags & DDSD_MIPMAPCOUNT) == DDSD_MIPMAPCOUNT )
    {
        message_error( "ExtractDXT1 %ls dds has mipmap"
            , in.c_str()
        );

        return EXIT_FAILURE;
    }

    if( (header.ddspf.dwFlags & DDPF_FOURCC) == 0 )
    {
        message_error( "ExtractDXT1 dds file no compress" );

        return EXIT_FAILURE;
    }

    if( header.ddspf.dwFourCC != FOURCC( 'D', 'X', 'T', '1' ) )
    {
        message_error( "ExtractDXT1 dds file no DXT1" );

        return EXIT_FAILURE;
    }

    uint32_t w = (header.dwWidth + 3) / 4;
    uint32_t h = (header.dwHeight + 3) / 4;

    size_t size = w * h * 1 * 8;

    uint8_t * dxt1_byte = new uint8_t[size];
    fread( dxt1_byte, 1, size, file_in );
    fclose( file_in );

    WCHAR outCanonicalizeQuote[MAX_PATH] = {'\0'};
    ::ForcePathQuoteSpaces( outCanonicalizeQuote, out.c_str() );
    ::PathUnquoteSpaces( outCanonicalizeQuote );

    FILE * file_out = _wfopen( outCanonicalizeQuote, L"wb" );

    fwrite( dxt1_byte, 1, size, file_out );
    fclose( file_out );

    delete[] dxt1_byte;

    return EXIT_SUCCESS;
}