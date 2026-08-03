#include "ToolUtils/ToolUtils.h"

#include "Config/Config.h"

#include <vector>
#include <string>
#include <sstream>
#include <cstdlib>

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

    const std::wstring in = parse_kwds( L"-in", std::wstring() );
    const std::wstring out = parse_kwds( L"-out", std::wstring() );

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

    uint8_t * etc1_byte = static_cast<uint8_t *>(::malloc( size ));

    if( etc1_byte == nullptr )
    {
        fclose( file_in );

        message_error( "unable to allocate etc1 buffer" );

        return EXIT_FAILURE;
    }

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

        ::free( etc1_byte );

        return EXIT_FAILURE;
    }

    fwrite( etc1_byte, 1, size, file_out );
    fclose( file_out );

    ::free( etc1_byte );

    return EXIT_SUCCESS;
}
