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

    bool premultiply = has_args( L"--premultiply" );

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
        if( premultiply == false )
        {
            for( uint32_t i = 0; i != height; ++i )
            {
                for( uint32_t j = 0; j != width; ++j )
                {
                    uint32_t index = j + i * width;
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
                        uint32_t ni = i + di[d];
                        uint32_t nj = j + dj[d];

                        if( ni >= height )
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

                        data_in[index * 4 + 0] = (uint8_t)(r * inv_count);
                        data_in[index * 4 + 1] = (uint8_t)(g * inv_count);
                        data_in[index * 4 + 2] = (uint8_t)(b * inv_count);
                    }
                }
            }
        }
        else
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
                    uint32_t rg = g * a / 255;
                    uint32_t rb = b * a / 255;

                    data_in[index * 4 + 0] = (uint8_t)ra;
                    data_in[index * 4 + 1] = (uint8_t)rg;
                    data_in[index * 4 + 2] = (uint8_t)rb;
                }
            }
        }
    }

    if( png_move_to_file( out_path.c_str(), data_in, width * bpp, width, height, bpp ) == false )
    {
        message_error( "invalid write_file_memory %ls"
            , out_path.c_str()
        );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
