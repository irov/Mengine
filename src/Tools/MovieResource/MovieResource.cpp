#include "ToolUtils/ToolUtils.h"

#include "Config/Config.h"

#include "movie/movie.hpp"
#include "pugixml.hpp"

#include <vector>
#include <string>
#include <sstream>

//////////////////////////////////////////////////////////////////////////
static void * stdlib_movie_alloc( void * _data, size_t _size )
{
    MENGINE_UNUSED( _data );

    return malloc( _size );
}
//////////////////////////////////////////////////////////////////////////
static void * stdlib_movie_alloc_n( void * _data, size_t _size, size_t _count )
{
    MENGINE_UNUSED( _data );

    size_t total = _size * _count;

    return malloc( total );
}
//////////////////////////////////////////////////////////////////////////
static void stdlib_movie_free( void * _data, const void * _ptr )
{
    MENGINE_UNUSED( _data );

    free( (void *)_ptr );
}
//////////////////////////////////////////////////////////////////////////
static void stdlib_movie_free_n( void * _data, const void * _ptr )
{
    MENGINE_UNUSED( _data );

    free( (void *)_ptr );
}
//////////////////////////////////////////////////////////////////////////
static void stdlib_movie_logerror( void * _data, aeMovieErrorCode _code, const char * _format, ... )
{
    MENGINE_UNUSED( _data );

    switch( _code )
    {
    case AE_ERROR_STREAM:
        {
            return;
        }break;
    default:
        {
        }break;
    }

    va_list argList;

    va_start( argList, _format );
    char msg[4096];
    vsprintf( msg, _format, argList );
    va_end( argList );

    printf( "# %s code %d"
        , msg
        , _code
    );
}
//////////////////////////////////////////////////////////////////////////
struct resource_provider_t
{
    pugi::xml_node * xmlDataBlock;
    CHAR movie_name[128];
};
//////////////////////////////////////////////////////////////////////////
static std::string sf( float _f )
{
    char data[256];
    sprintf( data, "%.127f", _f );

    size_t len = strlen( data );

    for( size_t index = len; index != 0; --index )
    {
        if( data[index - 1] == '.' )
        {
            data[index] = '0';
            data[index + 1] = '\0';

            break;
        }

        if( data[index - 1] != '0' )
        {
            break;
        }

        data[index - 1] = '\0';
    }

    return data;
}
//////////////////////////////////////////////////////////////////////////
static std::string sm( ae_matrix34_ptr_t m )
{
    char xmlMatrix[4096];
    sprintf( xmlMatrix, "%s;%s;%s;%s"";""%s;%s;%s;%s"";""%s;%s;%s;%s"";""%s;%s;%s;%s"
        , sf( m[3 * 0 + 0] ).c_str()
        , sf( m[3 * 0 + 1] ).c_str()
        , sf( m[3 * 0 + 2] ).c_str()
        , "0.0"
        , sf( m[3 * 1 + 0] ).c_str()
        , sf( m[3 * 1 + 1] ).c_str()
        , sf( m[3 * 1 + 2] ).c_str()
        , "0.0"
        , sf( m[3 * 2 + 0] ).c_str()
        , sf( m[3 * 2 + 1] ).c_str()
        , sf( m[3 * 2 + 2] ).c_str()
        , "0.0"
        , sf( m[3 * 3 + 0] ).c_str()
        , sf( m[3 * 3 + 1] ).c_str()
        , sf( m[3 * 3 + 2] ).c_str()
        , "1.0"
    );

    return xmlMatrix;
}
//////////////////////////////////////////////////////////////////////////
static std::string sc( const ae_color_t & c, float o )
{
    char xmlColor[4096];
    sprintf( xmlColor, "%s;%s;%s;%s"
        , sf( c.r ).c_str()
        , sf( c.g ).c_str()
        , sf( c.b ).c_str()
        , sf( o ).c_str()
    );

    return xmlColor;
}
//////////////////////////////////////////////////////////////////////////
static ae_bool_t my_resource_provider( const aeMovieResource * _resource, ae_voidptrptr_t _rd, ae_voidptr_t _data )
{
    resource_provider_t * provider = (resource_provider_t *)_data;

    pugi::xml_node * xmlDataBlock = provider->xmlDataBlock;

    aeMovieResourceTypeEnum resource_type = _resource->type;

    switch( resource_type )
    {
    case AE_MOVIE_RESOURCE_IMAGE:
        {
            const aeMovieResourceImage * resource_image = (const aeMovieResourceImage *)_resource;

            if( resource_image->trim_width == 0.f || resource_image->trim_height == 0.f )
            {
                pugi::xml_node xmlResource = xmlDataBlock->append_child( "Resource" );
                xmlResource.append_attribute( "Name" ).set_value( resource_image->name );
                xmlResource.append_attribute( "Type" ).set_value( "ResourceImageEmpty" );
                xmlResource.append_attribute( "Unique" ).set_value( 0U );

                pugi::xml_node xmlResourceFile = xmlResource.append_child( "File" );

                char xmlMaxSize[256];
                sprintf( xmlMaxSize, "%u;%u"
                    , (uint32_t)resource_image->trim_width
                    , (uint32_t)resource_image->trim_height
                );

                xmlResourceFile.append_attribute( "MaxSize" ).set_value( xmlMaxSize );
            }
            else if( resource_image->atlas_image == AE_NULLPTR )
            {
                pugi::xml_node xmlResource = xmlDataBlock->append_child( "Resource" );
                xmlResource.append_attribute( "Name" ).set_value( resource_image->name );
                xmlResource.append_attribute( "Type" ).set_value( "ResourceImageDefault" );
                xmlResource.append_attribute( "Unique" ).set_value( 0U );

                if( strstr( resource_image->name, ".ptc_" ) == nullptr )
                {
                    pugi::xml_node xmlResourceFile = xmlResource.append_child( "File" );

                    char xmlFilePath[MAX_PATH];
                    sprintf( xmlFilePath, "Movies2/cache_images/%s"
                        , resource_image->path
                    );

                    xmlResourceFile.append_attribute( "Path" ).set_value( xmlFilePath );

                    char xmlMaxSize[256];
                    sprintf( xmlMaxSize, "%u;%u"
                        , (uint32_t)resource_image->base_width
                        , (uint32_t)resource_image->base_height
                    );

                    xmlResourceFile.append_attribute( "MaxSize" ).set_value( xmlMaxSize );

                    if( resource_image->base_width != resource_image->trim_width || resource_image->base_height != resource_image->trim_height )
                    {
                        char xmlTrimSize[256];
                        sprintf( xmlTrimSize, "%u;%u"
                            , (uint32_t)resource_image->trim_width
                            , (uint32_t)resource_image->trim_height
                        );

                        xmlResourceFile.append_attribute( "Size" ).set_value( xmlTrimSize );
                    }

                    if( resource_image->offset_x != 0.f || resource_image->offset_y != 0.f )
                    {
                        char xmlOffsetSize[256];
                        sprintf( xmlOffsetSize, "%u;%u"
                            , (uint32_t)resource_image->offset_x
                            , (uint32_t)resource_image->offset_y
                        );

                        xmlResourceFile.append_attribute( "Offset" ).set_value( xmlOffsetSize );
                    }
                }
                else
                {
                    pugi::xml_node xmlResourceFile = xmlResource.append_child( "File" );

                    char xmlFilePath[MAX_PATH];
                    sprintf( xmlFilePath, "Movies2/cache_particles/%s"
                        , resource_image->path
                    );

                    xmlResourceFile.append_attribute( "Path" ).set_value( xmlFilePath );

                    char xmlMaxSize[256];
                    sprintf( xmlMaxSize, "%u;%u"
                        , (uint32_t)resource_image->base_width
                        , (uint32_t)resource_image->base_height
                    );

                    xmlResourceFile.append_attribute( "MaxSize" ).set_value( xmlMaxSize );

                    if( resource_image->base_width != resource_image->trim_width || resource_image->base_height != resource_image->trim_height )
                    {
                        char xmlTrimSize[256];
                        sprintf( xmlTrimSize, "%u;%u"
                            , (uint32_t)resource_image->trim_width
                            , (uint32_t)resource_image->trim_height
                        );

                        xmlResourceFile.append_attribute( "Size" ).set_value( xmlTrimSize );
                    }

                    if( resource_image->offset_x != 0.f || resource_image->offset_y != 0.f )
                    {
                        char xmlOffsetSize[256];
                        sprintf( xmlOffsetSize, "%u;%u"
                            , (uint32_t)resource_image->offset_x
                            , (uint32_t)resource_image->offset_y
                        );

                        xmlResourceFile.append_attribute( "Offset" ).set_value( xmlOffsetSize );
                    }

                    xmlResourceFile.append_attribute( "NoConvert" ).set_value( 1U );
                    xmlResourceFile.append_attribute( "NoAtlas" ).set_value( 1U );
                }
            }
            else
            {
                pugi::xml_node xmlResource = xmlDataBlock->append_child( "Resource" );
                xmlResource.append_attribute( "Name" ).set_value( resource_image->name );
                xmlResource.append_attribute( "Type" ).set_value( "ResourceImageSubstract" );
                xmlResource.append_attribute( "Unique" ).set_value( 0U );

                pugi::xml_node xmlResourceImage = xmlResource.append_child( "Image" );
                xmlResourceImage.append_attribute( "Alpha" ).set_value( "1" );

                char xmlMaxSize[256];
                sprintf( xmlMaxSize, "%u;%u"
                    , (uint32_t)resource_image->trim_width
                    , (uint32_t)resource_image->trim_height
                );
                xmlResourceImage.append_attribute( "MaxSize" ).set_value( xmlMaxSize );

                xmlResourceImage.append_attribute( "Name" ).set_value( resource_image->atlas_image->name );

                char xmlUV[256];
                sprintf( xmlUV, "%s;%s;%s;%s;%s;%s;%s;%s"
                    , sf( 0.f ).c_str()
                    , sf( 0.f ).c_str()
                    , sf( 1.f ).c_str()
                    , sf( 0.f ).c_str()
                    , sf( 1.f ).c_str()
                    , sf( 1.f ).c_str()
                    , sf( 0.f ).c_str()
                    , sf( 1.f ).c_str()
                );

                xmlResourceImage.append_attribute( "UV" ).set_value( xmlUV );

                xmlResourceImage.append_attribute( "UVRotate" ).set_value( resource_image->atlas_rotate );
            }
        }break;
    case AE_MOVIE_RESOURCE_SEQUENCE:
        {
            const aeMovieResourceSequence * resource_sequence = (const aeMovieResourceSequence *)_resource;

            pugi::xml_node xmlResource = xmlDataBlock->append_child( "Resource" );
            xmlResource.append_attribute( "Name" ).set_value( resource_sequence->name );
            xmlResource.append_attribute( "Type" ).set_value( "ResourceImageSequence" );
            xmlResource.append_attribute( "Unique" ).set_value( 0U );

            float frameDuration = 1.f / resource_sequence->frameDurationInv;

            for( ae_uint32_t index = 0; index != resource_sequence->image_count; ++index )
            {
                const aeMovieResourceImage * resource_image = resource_sequence->images[index];

                pugi::xml_node xmlSequence = xmlResource.append_child( "Sequence" );

                xmlSequence.append_attribute( "ResourceImageName" ).set_value( resource_image->name );
                xmlSequence.append_attribute( "Delay" ).set_value( sf( frameDuration ).c_str() );
            }
        }break;
    case AE_MOVIE_RESOURCE_VIDEO:
        {
            const aeMovieResourceVideo * resource_video = (const aeMovieResourceVideo *)_resource;

            pugi::xml_node xmlResource = xmlDataBlock->append_child( "Resource" );
            xmlResource.append_attribute( "Name" ).set_value( resource_video->name );
            xmlResource.append_attribute( "Type" ).set_value( "ResourceVideo" );
            xmlResource.append_attribute( "Unique" ).set_value( 0U );

            pugi::xml_node xmlResourceFile = xmlResource.append_child( "File" );

            char xmlFilePath[MAX_PATH];
            sprintf( xmlFilePath, "Movies2/cache_video/%s"
                , resource_video->path
            );

            xmlResourceFile.append_attribute( "Path" ).set_value( xmlFilePath );

            char xmlSize[256];
            sprintf( xmlSize, "%u;%u"
                , (uint32_t)resource_video->base_width
                , (uint32_t)resource_video->base_height
            );

            xmlResourceFile.append_attribute( "Size" ).set_value( xmlSize );

            xmlResourceFile.append_attribute( "Alpha" ).set_value( resource_video->has_alpha_channel == AE_TRUE ? 1U : 0U );
            xmlResourceFile.append_attribute( "Codec" ).set_value( resource_video->has_alpha_channel == AE_TRUE ? "ogvaVideo" : "ogvVideo" );
            xmlResourceFile.append_attribute( "FrameRate" ).set_value( sf( resource_video->frameRate ).c_str() );
            xmlResourceFile.append_attribute( "Duration" ).set_value( sf( resource_video->duration * 1000.f ).c_str() );
        }break;
    case AE_MOVIE_RESOURCE_SOUND:
        {
            const aeMovieResourceSound * resource_sound = (const aeMovieResourceSound *)_resource;

            pugi::xml_node xmlResource = xmlDataBlock->append_child( "Resource" );
            xmlResource.append_attribute( "Name" ).set_value( resource_sound->name );
            xmlResource.append_attribute( "Type" ).set_value( "ResourceSound" );
            xmlResource.append_attribute( "Unique" ).set_value( 0U );

            pugi::xml_node xmlResourceFile = xmlResource.append_child( "File" );

            char xmlFilePath[MAX_PATH];
            sprintf( xmlFilePath, "Movies2/cache_sounds/%s"
                , resource_sound->path
            );

            xmlResourceFile.append_attribute( "Path" ).set_value( xmlFilePath );

            xmlResourceFile.append_attribute( "Codec" ).set_value( "oggSound" );

            if( resource_sound->duration > 3.f )
            {
                xmlResource.append_child( "IsStreamable" ).append_attribute( "Value" ).set_value( 1U );
            }
        }break;
    case AE_MOVIE_RESOURCE_PARTICLE:
        {
            const aeMovieResourceParticle * resource_particle = (const aeMovieResourceParticle *)_resource;

            pugi::xml_node xmlResource = xmlDataBlock->append_child( "Resource" );
            xmlResource.append_attribute( "Name" ).set_value( resource_particle->name );
            xmlResource.append_attribute( "Type" ).set_value( "ResourceAstralax" );
            xmlResource.append_attribute( "Unique" ).set_value( 0U );

            pugi::xml_node xmlResourceFile = xmlResource.append_child( "File" );

            char xmlFilePath[MAX_PATH];
            sprintf( xmlFilePath, "Movies2/cache_particles/%s"
                , resource_particle->path
            );

            xmlResourceFile.append_attribute( "Path" ).set_value( xmlFilePath );

            xmlResource.append_child( "AtlasCount" ).append_attribute( "Value" ).set_value( resource_particle->image_count );

            for( uint32_t i = 0; i != resource_particle->image_count; ++i )
            {
                const aeMovieResourceImage * image = resource_particle->images[i];

                pugi::xml_node xmlResourceAtlas = xmlResource.append_child( "Atlas" );

                xmlResourceAtlas.append_attribute( "Index" ).set_value( i );
                xmlResourceAtlas.append_attribute( "ResourceName" ).set_value( image->name );
            }
        }break;
    }

    *_rd = AE_NULLPTR;
    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
struct node_provider_t
{
    pugi::xml_node * xmlComposition;
    CHAR movie_name[128];
};
//////////////////////////////////////////////////////////////////////////
static void __add_node_layer_data( node_provider_t * np, const ae_char_t * _type, const aeMovieNodeProviderCallbackData * _callbackData )
{
    pugi::xml_node xmlLayer = np->xmlComposition->append_child( "Layer" );

    const aeMovieLayerData * layer_data = _callbackData->layer_data;

    const ae_char_t * layer_name = ae_get_movie_layer_data_name( layer_data );

    xmlLayer.append_attribute( "Index" ).set_value( _callbackData->index );
    xmlLayer.append_attribute( "Name" ).set_value( layer_name );
    xmlLayer.append_attribute( "Type" ).set_value( _type );


    if( _callbackData->matrix[0] != 1.f ||
        _callbackData->matrix[1] != 0.f ||
        _callbackData->matrix[2] != 0.f ||
        _callbackData->matrix[3] != 0.f ||
        _callbackData->matrix[4] != 1.f ||
        _callbackData->matrix[5] != 0.f ||
        _callbackData->matrix[3] != 0.f ||
        _callbackData->matrix[4] != 0.f ||
        _callbackData->matrix[5] != 1.f ||
        _callbackData->matrix[3] != 0.f ||
        _callbackData->matrix[4] != 0.f ||
        _callbackData->matrix[5] != 0.f )
    {
        xmlLayer.append_attribute( "Matrix" ).set_value( sm( _callbackData->matrix ).c_str() );
    }

    if( _callbackData->color.r != 1.f ||
        _callbackData->color.g != 1.f ||
        _callbackData->color.b != 1.f ||
        _callbackData->opacity != 1.f )
    {
        xmlLayer.append_attribute( "Color" ).set_value( sc( _callbackData->color, _callbackData->opacity ).c_str() );
    }

    uint32_t option_count = ae_get_movie_layer_data_options_count( layer_data );

    if( option_count != 0 )
    {
        std::stringstream ss;

        ae_uint32_t option0 = ae_get_movie_layer_data_option( layer_data, 0 );

        ss << option0;

        for( uint32_t index = 1; index != option_count; ++index )
        {
            ae_uint32_t option = ae_get_movie_layer_data_option( layer_data, index );

            ss << ';' << option;
        }

        xmlLayer.append_attribute( "Options" ).set_value( ss.str().c_str() );
    }

    ae_aabb_t aabb;
    if( ae_get_movie_layer_data_dimension( layer_data, &aabb ) == AE_TRUE )
    {
        char xmlDimension[4096];
        sprintf( xmlDimension, "%s;%s;%s;%s"
            , sf( aabb.minimal_x ).c_str()
            , sf( aabb.minimal_x ).c_str()
            , sf( aabb.minimal_x ).c_str()
            , sf( aabb.minimal_x ).c_str()
        );

        xmlLayer.append_attribute( "Dimension" ).set_value( xmlDimension );
    }
}
//////////////////////////////////////////////////////////////////////////
static ae_bool_t __movie_composition_node_provider( const aeMovieNodeProviderCallbackData * _callbackData, ae_voidptrptr_t _nd, ae_voidptr_t _ud )
{
    node_provider_t * np = (node_provider_t *)_ud;

    const aeMovieLayerData * layer_data = _callbackData->layer_data;

    ae_bool_t is_track_matte = ae_is_movie_layer_data_track_mate( layer_data );

    if( is_track_matte == AE_TRUE )
    {
        *_nd = AE_NULLPTR;

        return AE_TRUE;
    }

    aeMovieLayerTypeEnum type = ae_get_movie_layer_data_type( layer_data );

    //ToDo remove string add Enum Type

    switch( type )
    {
    case AE_MOVIE_LAYER_TYPE_MOVIE:
        {
            __add_node_layer_data( np, "Movie", _callbackData );
        }break;
    case AE_MOVIE_LAYER_TYPE_SPRITE:
        {
            __add_node_layer_data( np, "ShapeQuadFixed", _callbackData );
        }break;
    case AE_MOVIE_LAYER_TYPE_TEXT:
        {
            __add_node_layer_data( np, "TextField", _callbackData );
        }break;
    case AE_MOVIE_LAYER_TYPE_EVENT:
        {
            __add_node_layer_data( np, "Event", _callbackData );
        }break;
    case AE_MOVIE_LAYER_TYPE_SOCKET:
        {
            __add_node_layer_data( np, "HotSpotPolygon", _callbackData );
        }break;
    case AE_MOVIE_LAYER_TYPE_SHAPE:
        {
            __add_node_layer_data( np, "Shape", _callbackData );
        }break;
    case AE_MOVIE_LAYER_TYPE_SLOT:
        {
            __add_node_layer_data( np, "Movie2Slot", _callbackData );
        }break;
    case AE_MOVIE_LAYER_TYPE_NULL:
        {
            __add_node_layer_data( np, "Null", _callbackData );
        }break;
    case AE_MOVIE_LAYER_TYPE_SCENE_EFFECT:
        {
            __add_node_layer_data( np, "SceneEffect", _callbackData );
        }break;
    case AE_MOVIE_LAYER_TYPE_SOLID:
        {
            __add_node_layer_data( np, "Solid", _callbackData );
        }break;
    case AE_MOVIE_LAYER_TYPE_SEQUENCE:
        {
            __add_node_layer_data( np, "Sequence", _callbackData );
        }break;
    case AE_MOVIE_LAYER_TYPE_VIDEO:
        {
            __add_node_layer_data( np, "Video", _callbackData );
        }break;
    case AE_MOVIE_LAYER_TYPE_SOUND:
        {
            __add_node_layer_data( np, "Sound", _callbackData );
        }break;
    case AE_MOVIE_LAYER_TYPE_PARTICLE:
        {
            __add_node_layer_data( np, "AstralaxEmitter", _callbackData );
        }break;
    case AE_MOVIE_LAYER_TYPE_IMAGE:
        {
            __add_node_layer_data( np, "Image", _callbackData );
        }break;
    case AE_MOVIE_LAYER_TYPE_SUB_MOVIE:
        {
            __add_node_layer_data( np, "SubMovie", _callbackData );
        }break;
    default:
        break;
    };

    *_nd = AE_NULLPTR;

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
static ae_bool_t __visit_sub_composition( const aeMovieComposition * _compositionData, ae_uint32_t _index, const ae_char_t * _name, const aeMovieSubComposition * _subcomposition, ae_voidptr_t _ud )
{
    (void)_compositionData;
    (void)_subcomposition;

    node_provider_t * np = (node_provider_t *)_ud;

    pugi::xml_node xmlSubComposition = np->xmlComposition->append_child( "SubComposition" );

    xmlSubComposition.append_attribute( "Index" ).set_value( _index );
    xmlSubComposition.append_attribute( "Name" ).set_value( _name );

    const aeMovieCompositionData * subcomposition_composition_data = ae_get_movie_sub_composition_composition_data( _subcomposition );

    ae_float_t subcomposition_composition_data_duration = ae_get_movie_composition_data_duration( subcomposition_composition_data );
    ae_float_t subcomposition_composition_data_frame_duration = ae_get_movie_composition_data_frame_duration( subcomposition_composition_data );

    xmlSubComposition.append_attribute( "Duration" ).set_value( sf( subcomposition_composition_data_duration * 1000.f ).c_str() );
    xmlSubComposition.append_attribute( "FrameDuration" ).set_value( sf( subcomposition_composition_data_frame_duration * 1000.f ).c_str() );

    return AE_TRUE;
}
//////////////////////////////////////////////////////////////////////////
static void my_resource_deleter( aeMovieResourceTypeEnum _type, ae_voidptr_t _data, ae_voidptr_t _ud )
{
    (void)_type;
    (void)_data;
    (void)_ud;
}
//////////////////////////////////////////////////////////////////////////
static size_t my_read_stream( void * _buff, size_t _carriage, size_t _size, ae_userdata_t _data )
{
    AE_UNUSED( _carriage );

    FILE * f = (FILE *)_data;

    ae_size_t s = fread( _buff, 1, _size, f );

    return s;
}
//////////////////////////////////////////////////////////////////////////
static void my_copy_stream( const void * _src, void * _dst, size_t _size, ae_userdata_t _data )
{
    AE_UNUSED( _data );

    memcpy( _dst, _src, _size );
}
//////////////////////////////////////////////////////////////////////////
static bool save_xml( const pugi::xml_document & xmldoc, const std::wstring & out_path )
{
    FILE * f_out = _wfopen( out_path.c_str(), L"wb" );

    if( f_out == NULL )
    {
        message_error( "invalid open out path '%ls'"
            , out_path.c_str()
        );

        return false;
    }

    std::stringstream ss;
    xmldoc.save( ss );

    std::string s = ss.str();

    fprintf( f_out, s.c_str() );
    fclose( f_out );

    return true;
}
//////////////////////////////////////////////////////////////////////////
int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
    (void)hInstance;
    (void)hPrevInstance;
    (void)nShowCmd;

    std::wstring in_path = parse_kwds( lpCmdLine, L"--in_path", std::wstring() );
    std::wstring out_path = parse_kwds( lpCmdLine, L"--out_path", std::wstring() );
    std::wstring movie_name = parse_kwds( lpCmdLine, L"--movie_name", std::wstring() );

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

    CHAR utf8_movie_name[128];
    unicode_to_utf8( utf8_movie_name, 128, movie_name.c_str(), movie_name.size() );

    const aeMovieInstance * movieInstance = ae_create_movie_instance( "f86464bbdebf0fe3e684b03ec263d049d079e6f1"
        , &stdlib_movie_alloc
        , &stdlib_movie_alloc_n
        , &stdlib_movie_free
        , &stdlib_movie_free_n
        , 0
        , &stdlib_movie_logerror, AE_NULLPTR );

    if( movieInstance == nullptr )
    {
        message_error( "invalid create movie instance" );

        return EXIT_FAILURE;
    }

    FILE * f_in = _wfopen( in_path.c_str(), L"rb" );

    if( f_in == NULL )
    {
        message_error( "invalid open in path '%ls'"
            , in_path.c_str()
        );

        return EXIT_FAILURE;
    }

    pugi::xml_document xmldoc;

    pugi::xml_node xmlDataBlock = xmldoc.append_child( "DataBlock" );
    pugi::xml_attribute xmlDataBlockName = xmlDataBlock.append_attribute( "Name" );
    xmlDataBlockName.set_value( utf8_movie_name );

    resource_provider_t rp;
    rp.xmlDataBlock = &xmlDataBlock;
    strcpy( rp.movie_name, utf8_movie_name );

    aeMovieDataProviders data_providers;
    ae_clear_movie_data_providers( &data_providers );
    data_providers.resource_provider = &my_resource_provider;
    data_providers.resource_deleter = &my_resource_deleter;

    aeMovieData * movieData = ae_create_movie_data( movieInstance, &data_providers, &rp );

    aeMovieStream * movieStream = ae_create_movie_stream( movieInstance, &my_read_stream, &my_copy_stream, f_in );

    //MessageBox( NULL, L"Test", L"Test", MB_OK );

    ae_uint32_t major_version;
    ae_uint32_t minor_version;
    ae_result_t result_load_movie_data = ae_load_movie_data( movieData, movieStream, &major_version, &minor_version );

    if( result_load_movie_data != AE_RESULT_SUCCESSFUL )
    {
        message_error( "invalid load movie data '%s' version '%d.%d' load '%d.%d'"
            , ae_get_movie_result_string_info( result_load_movie_data )
            , ae_get_movie_sdk_major_version()
            , ae_get_movie_sdk_minor_version()
            , major_version
            , minor_version
        );

        return EXIT_FAILURE;
    }

    pugi::xml_node xmlDataResource = xmlDataBlock.append_child( "Resource" );

    char xmlMovieName[256];
    sprintf( xmlMovieName, "Movie2_%s"
        , utf8_movie_name
    );

    xmlDataResource.append_attribute( "Name" ).set_value( xmlMovieName );
    xmlDataResource.append_attribute( "Type" ).set_value( "ResourceMovie2" );

    pugi::xml_node xmlDataResourceFile = xmlDataResource.append_child( "File" );

    char xmlMoviePath[MAX_PATH];
    sprintf( xmlMoviePath, "Movies2/%s/%s.aez"
        , utf8_movie_name
        , utf8_movie_name
    );

    xmlDataResourceFile.append_attribute( "Path" ).set_value( xmlMoviePath );

    ae_uint32_t movieDataCount = ae_get_movie_composition_data_count( movieData );

    for( ae_uint32_t index = 0; index != movieDataCount; ++index )
    {
        const aeMovieCompositionData * compositionData = ae_get_movie_composition_data_by_index( movieData, index );

        if( ae_is_movie_composition_data_master( compositionData ) == AE_FALSE )
        {
            continue;
        }

        ae_bool_t master = ae_is_movie_composition_data_master( compositionData );

        const ae_char_t * compositionDataName = ae_get_movie_composition_data_name( compositionData );

        //ToDo fix duration milliseconds
        ae_float_t duration = ae_get_movie_composition_data_duration( compositionData );
        ae_float_t frame_duration = ae_get_movie_composition_data_frame_duration( compositionData );

        pugi::xml_node xmlDataResourceComposition = xmlDataResource.append_child( "Composition" );

        xmlDataResourceComposition.append_attribute( "Name" ).set_value( compositionDataName );
        xmlDataResourceComposition.append_attribute( "Duration" ).set_value( sf( duration ).c_str() );
        xmlDataResourceComposition.append_attribute( "FrameDuration" ).set_value( sf( frame_duration ).c_str() );
        xmlDataResourceComposition.append_attribute( "Master" ).set_value( master ? "1" : "0" );

        ae_viewport_t viewport;
        if( ae_get_movie_composition_data_bounds( compositionData, &viewport ) == AE_TRUE )
        {
            char xmlCompositionBounds[MAX_PATH];
            sprintf( xmlCompositionBounds, "%s;%s;%s;%s"
                , sf( viewport.begin_x ).c_str()
                , sf( viewport.begin_y ).c_str()
                , sf( viewport.end_x ).c_str()
                , sf( viewport.end_y ).c_str()
            );

            xmlDataResourceComposition.append_attribute( "Bounds" ).set_value( xmlCompositionBounds );
        }

        aeMovieCompositionProviders composition_providers;
        ae_initialize_movie_composition_providers( &composition_providers );

        composition_providers.node_provider = &__movie_composition_node_provider;

        node_provider_t np;
        np.xmlComposition = &xmlDataResourceComposition;
        strcpy( np.movie_name, utf8_movie_name );

        const aeMovieComposition * composition = ae_create_movie_composition( compositionData, AE_TRUE, &composition_providers, &np );

        ae_visit_movie_sub_composition( composition, __visit_sub_composition, &np );

        ae_delete_movie_composition( composition );
    }

    ae_delete_movie_stream( movieStream );

    fclose( f_in );

    ae_delete_movie_data( movieData );

    ae_delete_movie_instance( movieInstance );

    if( save_xml( xmldoc, out_path ) == false )
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}