#include "ToolUtils/ToolUtils.h"

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <malloc.h>

//////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
    (void)hInstance;
    (void)hPrevInstance;
    (void)lpCmdLine;
    (void)nShowCmd;

    PWSTR pwCmdLine = GetCommandLineW();

    std::wstring in_path = parse_kwds( pwCmdLine, L"--in", std::wstring() );
    std::wstring out_path = parse_kwds( pwCmdLine, L"--out", std::wstring() );

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

    FILE * f_in = _wfopen( in_path.c_str(), L"rb" );

    if( f_in == NULL )
    {
        message_error( "invalid _wfopen %ls"
            , in_path.c_str()
        );

        return EXIT_FAILURE;
    }

    fseek( f_in, 0, SEEK_END );
    long f_in_size = ftell( f_in );
    rewind( f_in );

    if( f_in_size == 0 )
    {
        fclose( f_in );

        message_error( "invalid size %ls"
            , in_path.c_str()
        );

        return EXIT_FAILURE;
    }

    stbi_uc * memory_in = (stbi_uc *)malloc( f_in_size );

    fread( memory_in, 1, f_in_size, f_in );
    fclose( f_in );

    int32_t width, height, bpp;
    stbi_uc * data_in = stbi_load_from_memory( memory_in, f_in_size, &width, &height, &bpp, STBI_default );

    free( memory_in );

    if( bpp == 4 )
    {
        for( int32_t i = 0; i != height; ++i )
        {
            for( int32_t j = 0; j != width; ++j )
            {
                int32_t index = j + i * width;

                uint8_t r = data_in[index * 4 + 0];
                uint8_t g = data_in[index * 4 + 1];
                uint8_t b = data_in[index * 4 + 2];
                uint8_t a = data_in[index * 4 + 3];

                data_in[index * 4 + 0] = (stbi_uc)((uint32_t)r * a / 255);
                data_in[index * 4 + 1] = (stbi_uc)((uint32_t)g * a / 255);
                data_in[index * 4 + 2] = (stbi_uc)((uint32_t)b * a / 255);
            }
        }
    }

    int len_out;
    unsigned char * data_out = stbi_write_png_to_mem( data_in, width * bpp, width, height, bpp, &len_out );

    if( data_out == NULL )
    {
        message_error( "invalid write png to mem" );

        return EXIT_FAILURE;
    }

    FILE * f_out = _wfopen( out_path.c_str(), L"wb" );

    if( f_out == NULL )
    {
        message_error( "invalid _wfopen %ls"
            , out_path.c_str()
        );

        return EXIT_FAILURE;
    }

    fwrite( data_out, len_out, 1, f_out );
    fclose( f_out );

    STBI_FREE( data_in );
    STBI_FREE( data_out );

    return EXIT_SUCCESS;
}
