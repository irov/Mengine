#include "Config/Config.h"

#include "ToolUtils/ToolUtils.h"
#include "ToolUtils/ToolPNG.h"

#include <malloc.h>

//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    MENGINE_UNUSED( argc );
    MENGINE_UNUSED( argv );

    std::wstring in_path = parse_kwds( L"--in", std::wstring() );
    std::wstring out_path = parse_kwds( L"--out", std::wstring() );

    if( in_path.empty() == true )
    {
        message_error( "not found 'in' param" );

        return EXIT_FAILURE;
    }

    if( out_path.empty() == true )
    {
        message_error( "not found 'out' param" );

        return EXIT_FAILURE;
    }

    uint32_t width, height, bpp;
    uint8_t * data_in = png_load_from_file( in_path.c_str(), &width, &height, &bpp );

    if( data_in == nullptr )
    {
        message_error( "invalid read_file_memory %ls"
            , in_path.c_str()
        );

        return EXIT_FAILURE;
    }

    if( bpp == 4 )
    {
        for( uint32_t i = 0; i != height; ++i )
        {
            for( uint32_t j = 0; j != width; ++j )
            {
                uint32_t index = j + i * width;

                uint32_t r = data_in[index * 4 + 0];
                uint32_t g = data_in[index * 4 + 1];
                uint32_t b = data_in[index * 4 + 2];
                uint32_t a = data_in[index * 4 + 3];

                uint32_t ra = r * a / 255;
                uint32_t ga = g * a / 255;
                uint32_t ba = b * a / 255;

                data_in[index * 4 + 0] = (uint8_t)ra;
                data_in[index * 4 + 1] = (uint8_t)ga;
                data_in[index * 4 + 2] = (uint8_t)ba;
            }
        }
    }

    if( png_move_to_file( out_path.c_str(), data_in, width * bpp, width, height, bpp ) == false )
    {
        message_error( "invalid write png %ls"
            , out_path.c_str()
        );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
