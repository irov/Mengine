﻿#include "ToolUtils/ToolUtils.h"

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <malloc.h>

//////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
    (void)hInstance;
    (void)hPrevInstance;
    (void)nShowCmd;

    std::wstring in_path = parse_kwds( lpCmdLine, L"--in", std::wstring() );
    std::wstring out_path = parse_kwds( lpCmdLine, L"--out", std::wstring() );

    if( in_path.empty() == true )
    {
        message_error( "not found 'in' param\n" );

        return EXIT_FAILURE;
    }

    if( out_path.empty() == true )
    {
        message_error( "not found 'out' param\n" );

        return EXIT_FAILURE;
    }

    FILE * f_in = _wfopen( in_path.c_str(), L"rb" );

    if( f_in == NULL )
    {
        message_error( "invalid _wfopen %ls\n"
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

        message_error( "invalid size %ls\n"
            , in_path.c_str()
        );

        return EXIT_FAILURE;
    }

    stbi_uc * memory_in = (stbi_uc *)malloc( f_in_size );

    fread( memory_in, 1, f_in_size, f_in );
    fclose( f_in );

    int32_t width, height, bpp;
    stbi_uc * data_in = stbi_load_from_memory( memory_in, f_in_size, &width, &height, &bpp, STBI_rgb_alpha );

    free( memory_in );

    if( bpp == 4 )
    {
        for( int32_t i = 0; i != height; ++i )
        {
            for( int32_t j = 0; j != width; ++j )
            {
                int32_t index = j + i * width;
                uint8_t alpha = data_in[index * 4 + 3];

                if( alpha != 0 )
                {
                    continue;
                }

                float count = 0.f;

                float r = 0.f;
                float g = 0.f;
                float b = 0.f;

                int32_t di[] = {-1, 0, 1, 1, 1, 0, -1, -1};
                int32_t dj[] = {1, 1, 1, 0, -1, -1, -1, 0};

                for( uint32_t d = 0; d != 8; ++d )
                {
                    int32_t ni = i + di[d];
                    int32_t nj = j + dj[d];

                    if( ni < 0 )
                    {
                        continue;
                    }

                    if( ni >= height )
                    {
                        continue;
                    }

                    if( nj < 0 )
                    {
                        continue;
                    }

                    if( nj >= width )
                    {
                        continue;
                    }

                    uint32_t index_kl = nj + ni * width;
                    uint8_t alpha_kl = data_in[index_kl * 4 + 3];

                    if( alpha_kl != 0 )
                    {
                        r += data_in[index_kl * 4 + 0];
                        g += data_in[index_kl * 4 + 1];
                        b += data_in[index_kl * 4 + 2];

                        count += 1.f;
                    }
                }

                if( count != 0.f )
                {
                    float inv_count = 1.f / count;

                    data_in[index * 4 + 0] = (stbi_uc)(r * inv_count);
                    data_in[index * 4 + 1] = (stbi_uc)(g * inv_count);
                    data_in[index * 4 + 2] = (stbi_uc)(b * inv_count);
                }
            }
        }
    }

    int len_out;
    unsigned char * data_out = stbi_write_png_to_mem( data_in, width * bpp, width, height, bpp, &len_out );
    
    if( data_out == NULL )
    {
        message_error( "invalid write png to mem\n" );

        return EXIT_FAILURE;
    }

    FILE * f_out = _wfopen( out_path.c_str(), L"wb" );

    if( f_out == NULL )
    {
        message_error( "invalid _wfopen %ls\n"
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
