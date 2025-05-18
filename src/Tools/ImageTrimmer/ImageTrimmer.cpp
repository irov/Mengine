#include "Config/Config.h"

#include "ToolUtils/ToolUtils.h"

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <malloc.h>

//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    MENGINE_UNUSED( argc );
    MENGINE_UNUSED( argv );

    PWSTR pwCmdLine = ::GetCommandLineW();

    std::wstring in_path = parse_kwds( pwCmdLine, L"--in_path", std::wstring() );
    std::wstring out_path = parse_kwds( pwCmdLine, L"--out_path", std::wstring() );
    std::wstring result_path = parse_kwds( pwCmdLine, L"--result_path", std::wstring() );
    std::wstring border = parse_kwds( pwCmdLine, L"--border", std::wstring() );
    std::wstring trim = parse_kwds( pwCmdLine, L"--trim", std::wstring() );
    std::wstring premultiplied = parse_kwds( pwCmdLine, L"--premultiplied", std::wstring() );

    if( in_path.empty() == true )
    {
        message_error( "not found 'in' param" );

        return EXIT_FAILURE;
    }

    if( in_path.front() == L'\"' && in_path.back() == L'\"' )
    {
        in_path = in_path.substr( 1, in_path.size() - 2 );
    }

    if( out_path.empty() == false && out_path.front() == L'\"' && out_path.back() == L'\"' )
    {
        out_path = out_path.substr( 1, out_path.size() - 2 );
    }

    if( result_path.empty() == false && result_path.front() == L'\"' && result_path.back() == L'\"' )
    {
        result_path = result_path.substr( 1, result_path.size() - 2 );
    }

    bool flag_border = false;
    if( border == L"1" )
    {
        flag_border = true;
    }

    bool flag_trim = false;
    if( trim == L"1" )
    {
        flag_trim = true;
    }

    bool flag_premultiplied = false;
    if( premultiplied == L"1" )
    {
        flag_premultiplied = true;
    }

    FILE * f_in = _wfopen( in_path.c_str(), L"rb" );

    if( f_in == NULL )
    {
        message_error( "invalid _wfopen %ls"
            , in_path.c_str()
        );

        return EXIT_FAILURE;
    }

    ::fseek( f_in, 0, SEEK_END );
    long f_in_size = ::ftell( f_in );
    ::rewind( f_in );

    if( f_in_size == 0 )
    {
        fclose( f_in );

        message_error( "invalid size %ls"
            , in_path.c_str()
        );

        return EXIT_FAILURE;
    }

    stbi_uc * memory_in = (stbi_uc *)malloc( f_in_size );

    if( memory_in == nullptr )
    {
        fclose( f_in );

        message_error( "invalid malloc size %ld"
            , f_in_size
        );

        return EXIT_FAILURE;
    }

    fread( memory_in, 1, f_in_size, f_in );
    fclose( f_in );

    int32_t width, height, bpp;
    stbi_uc * data_in = stbi_load_from_memory( memory_in, f_in_size, &width, &height, &bpp, STBI_default );

    free( memory_in );

    if( width == 0 || height == 0 )
    {
        return EXIT_FAILURE;
    }

    bool transparent = false;

    int32_t new_width;
    int32_t new_height;

    int32_t min_i;
    int32_t min_j;
    int32_t max_i;
    int32_t max_j;

    int32_t offset_i;
    int32_t offset_j;

    if( bpp == 4 && flag_trim == true )
    {
        min_i = width;
        min_j = height;
        max_i = 0;
        max_j = 0;

        for( int32_t i = 0; i != width; ++i )
        {
            for( int32_t j = 0; j != height; ++j )
            {
                int32_t index = i + j * width;
                uint8_t alpha = data_in[index * 4 + 3];

                if( alpha != 255 )
                {
                    transparent = true;
                }

                if( alpha == 0 )
                {
                    continue;
                }

                if( min_i > i )
                {
                    min_i = i;
                }

                if( min_j > j )
                {
                    min_j = j;
                }

                if( max_i < i )
                {
                    max_i = i;
                }

                if( max_j < j )
                {
                    max_j = j;
                }
            }
        }

        if( min_i > max_i || min_j > max_j )
        {
            new_width = 0;
            new_height = 0;

            min_i = 0;
            min_j = 0;
            max_i = 0;
            max_j = 0;
        }
        else
        {
            new_width = max_i - min_i + 1;
            new_height = max_j - min_j + 1;
        }

        if( flag_border == true )
        {
            new_width += 2;
            new_height += 2;

            offset_i = min_i - 1;
            offset_j = min_j - 1;
        }
        else
        {
            offset_i = min_i;
            offset_j = min_j;
        }
    }
    else
    {
        new_width = width;
        new_height = height;

        min_i = 0;
        min_j = 0;
        max_i = width;
        max_j = height;

        offset_i = min_i;
        offset_j = min_j;
    }

    if( out_path.empty() == false )
    {
        size_t new_bufferSize = new_width * new_height * bpp;

        stbi_uc * data_out = (stbi_uc *)malloc( new_bufferSize );

        if( data_out == nullptr )
        {
            return EXIT_FAILURE;
        }

        if( bpp == 4 )
        {
            if( flag_border == true && flag_trim == true )
            {
                int32_t copy_width_end = new_width - 1;
                int32_t copy_height_end = new_height - 1;

                for( int32_t i = 0; i != new_width; ++i )
                {
                    for( int32_t j = 0; j != new_height; ++j )
                    {
                        int32_t new_index = i + j * new_width;

                        if( (i == 0 || j == 0 || i == copy_width_end || j == copy_height_end) )
                        {
                            for( int32_t k = 0; k != bpp; ++k )
                            {
                                data_out[new_index * bpp + k] = 0;
                            }
                        }
                    }
                }

                if( flag_premultiplied == true )
                {
                    for( int32_t i = 1; i != copy_width_end; ++i )
                    {
                        for( int32_t j = 1; j != copy_height_end; ++j )
                        {
                            int32_t old_index = (min_i + (i - 1)) + (min_j + (j - 1)) * width;
                            int32_t new_index = i + j * new_width;

                            uint32_t r = data_in[old_index * 4 + 0];
                            uint32_t g = data_in[old_index * 4 + 1];
                            uint32_t b = data_in[old_index * 4 + 2];
                            uint32_t a = data_in[old_index * 4 + 3];

                            data_out[new_index * 4 + 0] = (uint8_t)(r * a / 255);
                            data_out[new_index * 4 + 1] = (uint8_t)(g * a / 255);
                            data_out[new_index * 4 + 2] = (uint8_t)(b * a / 255);
                            data_out[new_index * 4 + 3] = (uint8_t)a;
                        }
                    }
                }
                else
                {
                    for( int32_t i = 1; i != copy_width_end; ++i )
                    {
                        for( int32_t j = 1; j != copy_height_end; ++j )
                        {
                            int32_t old_index = (min_i + (i - 1)) + (min_j + (j - 1)) * width;
                            int32_t new_index = i + j * new_width;

                            uint32_t r = data_in[old_index * 4 + 0];
                            uint32_t g = data_in[old_index * 4 + 1];
                            uint32_t b = data_in[old_index * 4 + 2];
                            uint32_t a = data_in[old_index * 4 + 3];

                            data_out[new_index * 4 + 0] = (stbi_uc)r;
                            data_out[new_index * 4 + 1] = (stbi_uc)g;
                            data_out[new_index * 4 + 2] = (stbi_uc)b;
                            data_out[new_index * 4 + 3] = (stbi_uc)a;
                        }
                    }
                }
            }
            else
            {
                if( flag_premultiplied == true )
                {
                    for( int32_t i = 0; i != new_width; ++i )
                    {
                        for( int32_t j = 0; j != new_height; ++j )
                        {
                            int32_t old_index = (min_i + i) + (min_j + j) * width;
                            int32_t new_index = i + j * new_width;

                            uint32_t r = data_in[old_index * 4 + 0];
                            uint32_t g = data_in[old_index * 4 + 1];
                            uint32_t b = data_in[old_index * 4 + 2];
                            uint32_t a = data_in[old_index * 4 + 3];

                            data_out[new_index * 4 + 0] = (stbi_uc)(r * a / 255);
                            data_out[new_index * 4 + 1] = (stbi_uc)(g * a / 255);
                            data_out[new_index * 4 + 2] = (stbi_uc)(b * a / 255);
                            data_out[new_index * 4 + 3] = (stbi_uc)a;
                        }
                    }
                }
                else
                {
                    for( int32_t i = 0; i != new_width; ++i )
                    {
                        for( int32_t j = 0; j != new_height; ++j )
                        {
                            int32_t old_index = (min_i + i) + (min_j + j) * width;
                            int32_t new_index = i + j * new_width;

                            uint32_t r = data_in[old_index * 4 + 0];
                            uint32_t g = data_in[old_index * 4 + 1];
                            uint32_t b = data_in[old_index * 4 + 2];
                            uint32_t a = data_in[old_index * 4 + 3];

                            data_out[new_index * 4 + 0] = (stbi_uc)r;
                            data_out[new_index * 4 + 1] = (stbi_uc)g;
                            data_out[new_index * 4 + 2] = (stbi_uc)b;
                            data_out[new_index * 4 + 3] = (stbi_uc)a;
                        }
                    }
                }
            }

            if( flag_premultiplied == false )
            {
                for( int32_t i = 0; i != new_width; ++i )
                {
                    for( int32_t j = 0; j != new_height; ++j )
                    {
                        int32_t new_index = i + j * new_width;

                        uint8_t alpha = data_out[new_index * 4 + 3];

                        if( alpha != 0 )
                        {
                            continue;
                        }

                        uint32_t r = 0;
                        uint32_t g = 0;
                        uint32_t b = 0;
                        uint32_t colorNum = 0;

                        if( i > 0 ) // left pixel
                        {
                            int32_t test_index = (i - 1) + (j + 0) * new_width;

                            uint8_t test_alpha = data_out[test_index * 4 + 3];

                            if( test_alpha != 0 )
                            {
                                ++colorNum;
                                r += data_out[test_index * 4 + 0];
                                g += data_out[test_index * 4 + 1];
                                b += data_out[test_index * 4 + 2];
                            }
                        }

                        if( i < new_width - 1 ) // right pixel
                        {
                            int32_t test_index = (i + 1) + (j + 0) * new_width;

                            uint8_t test_alpha = data_out[test_index * 4 + 3];

                            if( test_alpha != 0 )
                            {
                                ++colorNum;
                                r += data_out[test_index * 4 + 0];
                                g += data_out[test_index * 4 + 1];
                                b += data_out[test_index * 4 + 2];
                            }
                        }

                        if( j > 0 ) // top pixel
                        {
                            int32_t test_index = (i + 0) + (j - 1) * new_width;

                            uint8_t test_alpha = data_out[test_index * 4 + 3];

                            if( test_alpha != 0 )
                            {
                                ++colorNum;
                                r += data_out[test_index * 4 + 0];
                                g += data_out[test_index * 4 + 1];
                                b += data_out[test_index * 4 + 2];
                            }
                        }

                        if( j < new_height - 1 ) // bottom pixel
                        {
                            int32_t test_index = (i + 0) + (j + 1) * new_width;

                            uint8_t test_alpha = data_out[test_index * 4 + 3];

                            if( test_alpha != 0 )
                            {
                                ++colorNum;
                                r += data_out[test_index * 4 + 0];
                                g += data_out[test_index * 4 + 1];
                                b += data_out[test_index * 4 + 2];
                            }
                        }

                        if( colorNum > 0 )
                        {
                            r /= colorNum;
                            g /= colorNum;
                            b /= colorNum;

                            data_out[new_index * 4 + 0] = (stbi_uc)r;
                            data_out[new_index * 4 + 1] = (stbi_uc)g;
                            data_out[new_index * 4 + 2] = (stbi_uc)b;
                        }
                        else
                        {
                            data_out[new_index * 4 + 0] = 0;
                            data_out[new_index * 4 + 1] = 0;
                            data_out[new_index * 4 + 2] = 0;
                        }
                    }
                }
            }
        }
        else
        {
            for( int32_t i = 0; i != new_width; ++i )
            {
                for( int32_t j = 0; j != new_height; ++j )
                {
                    int32_t index = i + j * new_width;

                    for( int32_t k = 0; k != bpp; ++k )
                    {
                        data_out[index * bpp + k] = data_in[index * bpp + k];
                    }
                }
            }
        }

        int len_out;
        unsigned char * memory_out = stbi_write_png_to_mem( data_out, new_width * bpp, new_width, new_height, bpp, &len_out );

        free( data_out );

        if( memory_out == NULL )
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

        fwrite( memory_out, len_out, 1, f_out );
        fclose( f_out );

        STBI_FREE( data_in );
        STBI_FREE( memory_out );
    }

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

        ::fprintf_s( f_result, "%d\n", transparent == true ? 1 : 0 );
        ::fprintf_s( f_result, "%d\n", width );
        ::fprintf_s( f_result, "%d\n", height );
        ::fprintf_s( f_result, "%d\n", new_width );
        ::fprintf_s( f_result, "%d\n", new_height );
        ::fprintf_s( f_result, "%d\n", offset_i );
        ::fprintf_s( f_result, "%d\n", offset_j );

        fclose( f_result );
    }
    else
    {
        ::fprintf( stdout, "%d\n", transparent == true ? 1 : 0 );
        ::fprintf( stdout, "%d\n", width );
        ::fprintf( stdout, "%d\n", height );
        ::fprintf( stdout, "%d\n", new_width );
        ::fprintf( stdout, "%d\n", new_height );
        ::fprintf( stdout, "%d\n", offset_i );
        ::fprintf( stdout, "%d\n", offset_j );
    }

    return EXIT_SUCCESS;
}