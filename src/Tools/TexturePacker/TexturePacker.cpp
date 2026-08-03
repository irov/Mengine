#include "ToolUtils/ToolUtils.h"

#include "Config/Config.h"

#include "pugixml.hpp"

#include <vector>
#include <string>
#include <sstream>

#include "jansson.h"

//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    MENGINE_UNUSED( argc );
    MENGINE_UNUSED( argv );

    //uint32_t image_count = parse_kwds( lpCmdLine, L"--image_count", 0U );
    std::wstring in_path = parse_kwds( L"--in_path", std::wstring() );
    std::wstring out_path = parse_kwds( L"--out_path", std::wstring() );
    std::wstring result_path = parse_kwds( L"--result_path", std::wstring() );
    std::wstring texturepacker_path = parse_kwds( L"--texturepacker_path", std::wstring() );
    std::wstring log_path = parse_kwds( L"--log_path", std::wstring() );
    std::wstring premultiply = parse_kwds( L"--premultiply", std::wstring() );

    std::vector<std::wstring> images_path;

    FILE * f_in = _wfopen( in_path.c_str(), L"r" );

    if( f_in == NULL )
    {
        message_error( "invalid open in_path %ls"
            , in_path.c_str()
        );

        return EXIT_FAILURE;
    }

    WCHAR wimage_path[MAX_PATH];
    while( fgetws( wimage_path, MAX_PATH, f_in ) )
    {
        wchar_t * pos;
        if( (pos = wcschr( wimage_path, L'\n' )) != NULL )
        {
            *pos = '\0';
        }

        images_path.push_back( wimage_path );
    }

    fclose( f_in );

    if( texturepacker_path.empty() == true )
    {
        message_error( "not found 'texturepacker' param" );

        return EXIT_FAILURE;
    }

    if( images_path.empty() == true )
    {
        message_error( "not found 'images' param" );

        return EXIT_FAILURE;
    }

    const std::wstring libmovieTempDir = path_join( get_temporary_directory(), L".libmovie/TexturePacker" );
    const std::wstring dataTempDir = path_join( libmovieTempDir, L"data" );
    const std::wstring dataFormat = path_join( dataTempDir, L"atlas_{n}.json" );
    const std::wstring sheetFormat = path_join( out_path, L"atlas_{n}.png" );

    ::ForceRemoveDirectory( libmovieTempDir.c_str() );

    if( create_directories( dataTempDir ) == false )
    {
        message_error( "invalid create temporary directory %ls", dataTempDir.c_str() );
        return EXIT_FAILURE;
    }

    std::vector<std::wstring> processArguments = {
        L"--multipack",
        L"--enable-rotation",
        L"--trim-mode", L"Trim",
        L"--size-constraints", L"POT",
        L"--data", dataFormat,
        L"--sheet", sheetFormat,
        L"--format", L"json-array",
        L"--texture-format", L"png",
        L"--alpha-handling", L"PremultiplyAlpha",
        L"--max-width", L"2048",
        L"--max-height", L"2048",
        L"--max-size", L"2048"
    };

    processArguments.insert( processArguments.end(), images_path.begin(), images_path.end() );

    int exit_code = EXIT_FAILURE;

    if( execute_process( texturepacker_path, processArguments, log_path, &exit_code ) == false )
    {
        message_error( "invalid start TexturePacker %ls", texturepacker_path.c_str() );
        return EXIT_FAILURE;
    }

    if( exit_code != 0 )
    {
        message_error( "invalid TexturePacker process %ls exit_code %d", texturepacker_path.c_str(), exit_code );
        return EXIT_FAILURE;
    }

    const std::wstring dataPath = path_join( dataTempDir, L"*.json" );

    std::vector<std::wstring> files;
    SelectFile( dataPath.c_str(), files );

    struct AtlasImageDesc
    {
        std::string name;
        std::string atlas;

        json_int_t aw;
        json_int_t ah;

        json_int_t ox;
        json_int_t oy;

        json_int_t ow;
        json_int_t oh;

        json_int_t fw;
        json_int_t fh;

        json_int_t x0;
        json_int_t y0;
        json_int_t x1;
        json_int_t y1;
        json_int_t x2;
        json_int_t y2;
        json_int_t x3;
        json_int_t y3;

        bool rotated;
    };

    std::vector<AtlasImageDesc> atlas_images;

    for( const std::wstring & path : files )
    {
        const std::wstring sheetPath = path_join( dataTempDir, path );

        FILE * f = _wfopen( sheetPath.c_str(), L"rb" );

        if( f == NULL )
        {
            message_error( "invalid _wfopen %ls"
                , sheetPath.c_str()
            );

            return EXIT_FAILURE;
        }

        fseek( f, 0, SEEK_END );
        long f_size = ftell( f );
        rewind( f );

        void * buff = malloc( f_size );

        if( buff == nullptr )
        {
            return EXIT_FAILURE;
        }

        fread( buff, f_size, 1, f );
        fclose( f );

        json_error_t jerror;
        json_t * root = json_loadb( (const char *)buff, f_size, 0, &jerror );

        json_t * meta = json_object_get( root, "meta" );

        json_t * meta_image = json_object_get( meta, "image" );
        const char * atlasname = json_string_value( meta_image );

        json_t * meta_size = json_object_get( meta, "size" );

        json_t * meta_size_w = json_object_get( meta_size, "w" );
        json_int_t atlas_w = json_integer_value( meta_size_w );

        json_t * meta_size_h = json_object_get( meta_size, "h" );
        json_int_t atlas_h = json_integer_value( meta_size_h );

        json_t * frames = json_object_get( root, "frames" );

        size_t frames_size = json_array_size( frames );

        for( size_t i = 0; i != frames_size; ++i )
        {
            json_t * frame_data = json_array_get( frames, i );

            json_t * frame_data_filename = json_object_get( frame_data, "filename" );

            const char * filename = json_string_value( frame_data_filename );

            json_t * frame_data_sourceSize = json_object_get( frame_data, "sourceSize" );

            json_t * frame_data_sourceSize_w = json_object_get( frame_data_sourceSize, "w" );
            json_int_t fw = json_integer_value( frame_data_sourceSize_w );

            json_t * frame_data_sourceSize_h = json_object_get( frame_data_sourceSize, "h" );
            json_int_t fh = json_integer_value( frame_data_sourceSize_h );

            json_t * frame_data_spriteSourceSize = json_object_get( frame_data, "spriteSourceSize" );

            json_t * frame_data_spriteSourceSize_x = json_object_get( frame_data_spriteSourceSize, "x" );
            json_int_t ox = json_integer_value( frame_data_spriteSourceSize_x );

            json_t * frame_data_spriteSourceSize_y = json_object_get( frame_data_spriteSourceSize, "y" );
            json_int_t oy = json_integer_value( frame_data_spriteSourceSize_y );

            json_t * frame_data_frame = json_object_get( frame_data, "frame" );

            json_t * frame_data_frame_x = json_object_get( frame_data_frame, "x" );
            json_int_t x = json_integer_value( frame_data_frame_x );

            json_t * frame_data_frame_y = json_object_get( frame_data_frame, "y" );
            json_int_t y = json_integer_value( frame_data_frame_y );

            json_t * frame_data_frame_w = json_object_get( frame_data_frame, "w" );
            json_int_t w = json_integer_value( frame_data_frame_w );

            json_t * frame_data_frame_h = json_object_get( frame_data_frame, "h" );
            json_int_t h = json_integer_value( frame_data_frame_h );

            json_t * frame_data_rotated = json_object_get( frame_data, "rotated" );
            bool rotated = json_boolean_value( frame_data_rotated );

            AtlasImageDesc image;
            image.name = filename;
            image.atlas = atlasname;

            image.aw = atlas_w;
            image.ah = atlas_h;

            image.ox = ox;
            image.oy = oy;

            image.ow = w;
            image.oh = h;

            image.fw = fw;
            image.fh = fh;

            if( rotated == true )
            {
                image.x0 = x + h;
                image.y0 = y + 0;
                image.x1 = x + h;
                image.y1 = y + w;
                image.x2 = x + 0;
                image.y2 = y + w;
                image.x3 = x + 0;
                image.y3 = y + 0;
            }
            else
            {
                image.x0 = x + 0;
                image.y0 = y + 0;
                image.x1 = x + w;
                image.y1 = y + 0;
                image.x2 = x + w;
                image.y2 = y + h;
                image.x3 = x + 0;
                image.y3 = y + h;
            }

            image.rotated = rotated;

            atlas_images.push_back( image );
        }

        json_decref( root );
        free( buff );
    }

    WCHAR infoCanonicalizeQuote[MAX_PATH];
    ForcePathQuoteSpaces( infoCanonicalizeQuote, result_path.c_str() );
    PathUnquoteSpaces( infoCanonicalizeQuote );

    FILE * f_result;
    errno_t err = ::_wfopen_s( &f_result, infoCanonicalizeQuote, L"wt" );

    if( err != 0 )
    {
        message_error( "invalid _wfopen %ls err %d"
            , infoCanonicalizeQuote
            , err
        );

        return EXIT_FAILURE;
    }

	std::vector<AtlasImageDesc>::size_type atlasCount = atlas_images.size();
    fprintf_s( f_result, "%zu\n", atlasCount );

    for( const AtlasImageDesc & image : atlas_images )
    {
        fprintf_s( f_result, "%s;%s;%lld;%lld;%lld;%lld;%lld;%lld;%lld;%lld;%lld;%lld;%lld;%lld;%lld;%lld;%lld;%lld;%d\n"
            , image.name.c_str()
            , image.atlas.c_str()
            , image.aw
            , image.ah
            , image.ox
            , image.oy
            , image.ow
            , image.oh
            , image.fw
            , image.fh
            , image.x0
            , image.y0
            , image.x1
            , image.y1
            , image.x2
            , image.y2
            , image.x3
            , image.y3
            , image.rotated == true ? 1 : 0
        );
    }

    fclose( f_result );

    return EXIT_SUCCESS;
}
