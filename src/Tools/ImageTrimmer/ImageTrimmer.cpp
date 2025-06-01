#include "Config/Config.h"

#include "ToolUtils/ToolUtils.h"
#include "ToolUtils/ToolPNG.h"

#include <stdio.h>
#include <malloc.h>

//////////////////////////////////////////////////////////////////////////
static bool write_result( const std::wstring & _path, bool _transparent, int32_t _width, int32_t _height, int32_t _new_width, int32_t _new_height, int32_t _offset_i, int32_t _offset_j, bool _copy_only )
{
    if( _path.empty() == false )
    {
        WCHAR infoCanonicalizeQuote[MAX_PATH];
        ForcePathQuoteSpaces( infoCanonicalizeQuote, _path.c_str() );
        PathUnquoteSpaces( infoCanonicalizeQuote );

        FILE * f_result;
        errno_t err = ::_wfopen_s( &f_result, infoCanonicalizeQuote, L"wt" );

        if( err != 0 )
        {
            message_error( "invalid _wfopen %ls err %d"
                , infoCanonicalizeQuote
                , err
            );

            return false;
        }

        ::fprintf_s( f_result, "%d\n", _transparent == true ? 1 : 0 );
        ::fprintf_s( f_result, "%d\n", _width );
        ::fprintf_s( f_result, "%d\n", _height );
        ::fprintf_s( f_result, "%d\n", _new_width );
        ::fprintf_s( f_result, "%d\n", _new_height );
        ::fprintf_s( f_result, "%d\n", _offset_i );
        ::fprintf_s( f_result, "%d\n", _offset_j );
        ::fprintf_s( f_result, "%d\n", _copy_only == true ? 1 : 0 );


        fclose( f_result );
    }
    else
    {
        ::fprintf_s( stdout, "%d\n", _transparent == true ? 1 : 0 );
        ::fprintf_s( stdout, "%d\n", _width );
        ::fprintf_s( stdout, "%d\n", _height );
        ::fprintf_s( stdout, "%d\n", _new_width );
        ::fprintf_s( stdout, "%d\n", _new_height );
        ::fprintf_s( stdout, "%d\n", _offset_i );
        ::fprintf_s( stdout, "%d\n", _offset_j );
        ::fprintf_s( stdout, "%d\n", _copy_only == true ? 1 : 0 );
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    MENGINE_UNUSED( argc );
    MENGINE_UNUSED( argv );

    std::wstring in_path = parse_kwds( L"--in_path", std::wstring() );
    std::wstring out_path = parse_kwds( L"--out_path", std::wstring() );
    std::wstring result_path = parse_kwds( L"--result_path", std::wstring() );
    bool border = parse_kwds( L"--border", false );
    bool trim = parse_kwds( L"--trim", false );
    bool premultiplied = parse_kwds( L"--premultiplied", false );

    if( in_path.empty() == true )
    {
        message_error( "not found 'in' param" );

        return EXIT_FAILURE;
    }

    if( is_png_file( in_path.c_str() ) == false )
    {
        if( write_result( result_path, false, 0, 0, 0, 0, 0, 0, true ) == false )
        {
            message_error( "invalid write_result %ls"
                , result_path.c_str()
            );

            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
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

    if( width == 0 || height == 0 )
    {
        return EXIT_FAILURE;
    }

    if( bpp != 4 )
    {
        ::free( data_in );

        if( write_result( result_path, false, width, height, 0, 0, 0, 0, true ) == false )
        {
            message_error( "invalid write_result %ls"
                , result_path.c_str()
            );

            return EXIT_FAILURE;
        }

        return EXIT_FAILURE;
    }

    bool transparent = false;

    uint32_t new_width;
    uint32_t new_height;

    uint32_t min_i;
    uint32_t min_j;
    uint32_t max_i;
    uint32_t max_j;

    int32_t offset_i;
    int32_t offset_j;

    if( trim == true )
    {
        min_i = width;
        min_j = height;
        max_i = 0;
        max_j = 0;

        for( uint32_t i = 0; i != width; ++i )
        {
            for( uint32_t j = 0; j != height; ++j )
            {
                uint32_t index = i + j * width;
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

        if( border == true )
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

        uint8_t * data_out = (uint8_t *)::malloc( new_bufferSize );

        if( data_out == nullptr )
        {
            return EXIT_FAILURE;
        }

        if( border == true && trim == true )
        {
            uint32_t copy_width_end = new_width - 1;
            uint32_t copy_height_end = new_height - 1;

            for( uint32_t i = 0; i != new_width; ++i )
            {
                for( uint32_t j = 0; j != new_height; ++j )
                {
                    uint32_t new_index = i + j * new_width;

                    if( (i == 0 || j == 0 || i == copy_width_end || j == copy_height_end) )
                    {
                        for( uint32_t k = 0; k != bpp; ++k )
                        {
                            data_out[new_index * bpp + k] = 0;
                        }
                    }
                }
            }

            if( premultiplied == true )
            {
                for( uint32_t i = 1; i != copy_width_end; ++i )
                {
                    for( uint32_t j = 1; j != copy_height_end; ++j )
                    {
                        uint32_t old_index = (min_i + (i - 1)) + (min_j + (j - 1)) * width;
                        uint32_t new_index = i + j * new_width;

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
                for( uint32_t i = 1; i != copy_width_end; ++i )
                {
                    for( uint32_t j = 1; j != copy_height_end; ++j )
                    {
                        uint32_t old_index = (min_i + (i - 1)) + (min_j + (j - 1)) * width;
                        uint32_t new_index = i + j * new_width;

                        uint32_t r = data_in[old_index * 4 + 0];
                        uint32_t g = data_in[old_index * 4 + 1];
                        uint32_t b = data_in[old_index * 4 + 2];
                        uint32_t a = data_in[old_index * 4 + 3];

                        data_out[new_index * 4 + 0] = (uint8_t)r;
                        data_out[new_index * 4 + 1] = (uint8_t)g;
                        data_out[new_index * 4 + 2] = (uint8_t)b;
                        data_out[new_index * 4 + 3] = (uint8_t)a;
                    }
                }
            }
        }
        else
        {
            if( premultiplied == true )
            {
                for( uint32_t i = 0; i != new_width; ++i )
                {
                    for( uint32_t j = 0; j != new_height; ++j )
                    {
                        uint32_t old_index = (min_i + i) + (min_j + j) * width;
                        uint32_t new_index = i + j * new_width;

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
                for( uint32_t i = 0; i != new_width; ++i )
                {
                    for( uint32_t j = 0; j != new_height; ++j )
                    {
                        uint32_t old_index = (min_i + i) + (min_j + j) * width;
                        uint32_t new_index = i + j * new_width;

                        uint32_t r = data_in[old_index * 4 + 0];
                        uint32_t g = data_in[old_index * 4 + 1];
                        uint32_t b = data_in[old_index * 4 + 2];
                        uint32_t a = data_in[old_index * 4 + 3];

                        data_out[new_index * 4 + 0] = (uint8_t)r;
                        data_out[new_index * 4 + 1] = (uint8_t)g;
                        data_out[new_index * 4 + 2] = (uint8_t)b;
                        data_out[new_index * 4 + 3] = (uint8_t)a;
                    }
                }
            }
        }

        if( premultiplied == false )
        {
            for( uint32_t i = 0; i != new_width; ++i )
            {
                for( uint32_t j = 0; j != new_height; ++j )
                {
                    uint32_t new_index = i + j * new_width;

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
                        uint32_t test_index = (i - 1) + (j + 0) * new_width;

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

                        data_out[new_index * 4 + 0] = (uint8_t)r;
                        data_out[new_index * 4 + 1] = (uint8_t)g;
                        data_out[new_index * 4 + 2] = (uint8_t)b;
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

        ::free( data_in );

        if( png_move_to_file( out_path.c_str(), data_out, new_width * bpp, new_width, new_height, bpp ) == false )
        {
            message_error( "invalid _wfopen %ls"
                , out_path.c_str()
            );

            return EXIT_FAILURE;
        }
    }

    if( write_result( result_path, transparent, width, height, new_width, new_height, offset_i, offset_j, false ) == false )
    {
        message_error( "invalid write_result %ls"
            , result_path.c_str()
        );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}