#include "Environment/Windows/WindowsIncluder.h"

#include "ToolUtils/ToolUtils.h"

#include "Config/Config.h"

#include <Shlwapi.h>
#include <shellapi.h>

#include <vector>
#include <string>
#include <sstream>

#include <stdint.h>
//////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )
struct PVRTextureHeader
{
    uint32_t    version;
    uint32_t    flags;
    uint64_t    pixelFormat;
    uint32_t    colourSpace;
    uint32_t    channelType;
    uint32_t    height;
    uint32_t    width;
    uint32_t    depth;
    uint32_t    numSurfaces;
    uint32_t    numFaces;
    uint32_t    numMipmaps;
    uint32_t    metaDataSize;
};
#pragma pack(pop)

const unsigned int PVRTEX_MIPMAP = (1 << 8); // has mip map levels
const unsigned int PVRTEX_TWIDDLE = (1 << 9); // is twiddled
const unsigned int PVRTEX_BUMPMAP = (1 << 10); // has normals encoded for a bump map
const unsigned int PVRTEX_TILING = (1 << 11); // is bordered for tiled pvr
const unsigned int PVRTEX_CUBEMAP = (1 << 12); // is a cubemap/skybox
const unsigned int PVRTEX_FALSEMIPCOL = (1 << 13); //
const unsigned int PVRTEX_VOLUME = (1 << 14);
const unsigned int PVRTEX_PIXELTYPE = 0xff; // pixel type is always in the last 16bits of the flags
const unsigned int PVRTEX_IDENTIFIER = 0x21525650;  // the pvr identifier is the characters 'P','V','R'

const unsigned int PVRTEX_V1_HEADER_SIZE = 44; // old header size was 44 for identification purposes
//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    MENGINE_UNUSED( argc );
    MENGINE_UNUSED( argv );

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

    WCHAR inCanonicalizeQuote[MAX_PATH];
    ForcePathQuoteSpaces( inCanonicalizeQuote, in.c_str() );
    PathUnquoteSpaces( inCanonicalizeQuote );

    FILE * file_in = _wfopen( inCanonicalizeQuote, L"rb" );

    if( file_in == NULL )
    {
        message_error( "in file not found %ls"
            , inCanonicalizeQuote
        );

        return EXIT_FAILURE;
    }

    PVRTextureHeader header;
    fread( &header, sizeof( header ), 1, file_in );

    fseek( file_in, header.metaDataSize, SEEK_CUR );

    uint32_t w = (header.width + 3) / 4;
    uint32_t h = (header.height + 3) / 4;

    size_t size = w * h * 1 * 8;

    uint8_t * etc1_byte = new uint8_t[size];
    fread( etc1_byte, 1, size, file_in );
    fclose( file_in );

    WCHAR outCanonicalizeQuote[MAX_PATH];
    ForcePathQuoteSpaces( outCanonicalizeQuote, out.c_str() );
    PathUnquoteSpaces( outCanonicalizeQuote );

    FILE * file_out = _wfopen( outCanonicalizeQuote, L"wb" );

    if( file_out == NULL )
    {
        message_error( "out file not fount %ls"
            , inCanonicalizeQuote
        );

        return EXIT_FAILURE;
    }

    fwrite( etc1_byte, 1, size, file_out );
    fclose( file_out );

    delete[] etc1_byte;

    return EXIT_SUCCESS;
}