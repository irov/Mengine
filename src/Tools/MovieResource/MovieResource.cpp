#	include "ToolUtils/ToolUtils.h"

#	include <vector>
#	include <string>
#	include <sstream>

#   include "movie/movie.hpp"

//////////////////////////////////////////////////////////////////////////
static void unicode_to_utf8( char * _utf8, size_t _capacity, const wchar_t * _unicode, size_t _size )
{
    DWORD dwConversionFlags = WC_ERR_INVALID_CHARS;

    int utf8_size = ::WideCharToMultiByte(
        CP_UTF8
        , dwConversionFlags
        , _unicode
        , _size
        , _utf8
        , _capacity
        , NULL
        , NULL
    );

    _utf8[utf8_size] = 0;
}
//////////////////////////////////////////////////////////////////////////
static void * stdlib_movie_alloc( void * _data, size_t _size )
{
    (void)_data;

    return malloc( _size );
}
//////////////////////////////////////////////////////////////////////////
static void * stdlib_movie_alloc_n( void * _data, size_t _size, size_t _count )
{
    (void)_data;

    size_t total = _size * _count;

    return malloc( total );
}
//////////////////////////////////////////////////////////////////////////
static void stdlib_movie_free( void * _data, const void * _ptr )
{
    (void)_data;

    free( (void *)_ptr );
}
//////////////////////////////////////////////////////////////////////////
static void stdlib_movie_free_n( void * _data, const void * _ptr )
{
    (void)_data;

    free( (void *)_ptr );
}
//////////////////////////////////////////////////////////////////////////
static void stdlib_movie_logerror( void * _data, aeMovieErrorCode _code, const char * _format, ... )
{
    (void)_data;

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
    FILE * f;
    CHAR movie_name[128];
};
//////////////////////////////////////////////////////////////////////////
static ae_voidptr_t my_resource_provider( const aeMovieResource * _resource, ae_voidptr_t _data )
{
    resource_provider_t * provider = (resource_provider_t *)_data;

    FILE * f = provider->f;

    aeMovieResourceTypeEnum resource_type = _resource->type;

    switch( resource_type )
    {
    case AE_MOVIE_RESOURCE_IMAGE:
        {
            const aeMovieResourceImage * resource_image = (const aeMovieResourceImage *)_resource;

#if AE_MOVIE_SDK_MAJOR_VERSION >= 17
            fprintf( f, "   <Resource Name = \"%s\" Type = \"ResourceImageDefault\" Unique = \"0\">\n"
                , resource_image->name
            );
#endif

            fprintf( f, "       <File Path = \"Movies2/%s/%s\" MaxSize = \"%u;%u\"/>\n"
                , provider->movie_name
                , resource_image->path
                , (uint32_t)resource_image->trim_width
                , (uint32_t)resource_image->trim_height
            );

            fprintf( f, "   </Resource>\n" );
        }break;
    case AE_MOVIE_RESOURCE_VIDEO:
        {
            const aeMovieResourceVideo * resource_video = (const aeMovieResourceVideo *)_resource;

#if AE_MOVIE_SDK_MAJOR_VERSION >= 17
            fprintf( f, "   <Resource Name = \"%s\" Type = \"ResourceVideo\" Unique = \"0\">\n"
                , resource_video->name
            );
#endif
            
            fprintf( f, "       <File Path = \"Movie2/%s/%s\" Alpha = \"%u\" Codec = \"%s\" FrameRate = \"%f\" Duration = \"%f\"/>\n"
                , provider->movie_name
                , resource_video->path
                , resource_video->alpha
                , resource_video->alpha == AE_TRUE ? "ogvaVideo" : "ogvVideo"
                , resource_video->frameRate * 1000.0
                , resource_video->duration * 1000.0
            );

            fprintf( f, "   </Resource>\n" );
        }break;
    case AE_MOVIE_RESOURCE_SOUND:
        {
            const aeMovieResourceSound * resource_sound = (const aeMovieResourceSound *)_resource;
            
#if AE_MOVIE_SDK_MAJOR_VERSION >= 17
            fprintf( f, "   <Resource Name = \"%s\" Type = \"ResourceSound\" Unique = \"0\">\n"
                , resource_sound->name
            );
#endif

            fprintf( f, "       <File Path = \"%s\"  Codec = \"oggSound\"/>\n"
                , resource_sound->path
            );

            fprintf( f, "   </Resource>\n" );
        }break;
    case AE_MOVIE_RESOURCE_PARTICLE:
        {
            const aeMovieResourceParticle * resource_particle = (const aeMovieResourceParticle *)_resource;

#if AE_MOVIE_SDK_MAJOR_VERSION >= 17
            fprintf( f, "   <Resource Name = \"%s\" Type = \"ResourceParticle\" Unique = \"0\">\n"
                , resource_particle->name
            );
#endif

            fprintf( f, "       <File Path = \"Movie2/%s/%s\"/>\n"
                , provider->movie_name
                , resource_particle->path
            );

            fprintf( f, "       <AtlasCount Value = \"%u\"/>\n"
                , resource_particle->image_count
            );
            
            for( uint32_t i = 0; i != resource_particle->image_count; ++i )
            {
                const aeMovieResourceImage * image = resource_particle->images[i];

#if AE_MOVIE_SDK_MAJOR_VERSION >= 17
                fprintf( f, "       <Atlas Index = \"%u\" ResourceName = \"%s\"/>\n"
                    , i
                    , image->name
                );
#endif
            }

            fprintf( f, "   </Resource>\n" );
        }break;
    }

    return AE_NULL;
}
//////////////////////////////////////////////////////////////////////////
static void my_resource_deleter( aeMovieResourceTypeEnum _type, ae_voidptr_t _data, ae_voidptr_t _ud )
{
    (void)_type;
    (void)_data;
    (void)_ud;
}
//////////////////////////////////////////////////////////////////////////
static size_t my_read_stream( void * _data, void * _buff, size_t _carriage, size_t _size )
{
    AE_UNUSED( _carriage );

    FILE * f = (FILE *)_data;

    ae_size_t s = fread( _buff, 1, _size, f );

    return s;
}
//////////////////////////////////////////////////////////////////////////
static void my_copy_stream( void * _data, const void * _src, void * _dst, size_t _size )
{
    AE_UNUSED( _data );

    memcpy( _dst, _src, _size );
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
    std::wstring hash = parse_kwds( lpCmdLine, L"--hash", std::wstring() );
    
	if( in_path.empty() == true )
	{
		message_error("not found 'in' param\n"
			);

		return -1;
	}

    if( out_path.empty() == true )
    {
        message_error( "not found 'out' param\n"
        );

        return -1;
    }

    CHAR utf8_hash[64];
    unicode_to_utf8( utf8_hash, 64, hash.c_str(), hash.size() );

    CHAR utf8_movie_name[128];
    unicode_to_utf8( utf8_movie_name, 128, movie_name.c_str(), movie_name.size() );
    
    const aeMovieInstance *  movie_instance = ae_create_movie_instance( utf8_hash
        , &stdlib_movie_alloc
        , &stdlib_movie_alloc_n
        , &stdlib_movie_free
        , &stdlib_movie_free_n
        , 0
        , &stdlib_movie_logerror, AE_NULL );

    if( movie_instance == nullptr )
    {
        message_error( "invalid create movie instance\n"
        );

        return -1;
    }

    FILE * f_out = _wfopen( out_path.c_str(), L"wb" );

    if( f_out == NULL )
    {
        message_error( "invalid open out path '%ls'\n"
            , out_path.c_str()
        );

        return -1;
    }

    FILE * f_in = _wfopen( in_path.c_str(), L"rb" );

    if( f_in == NULL )
    {
        message_error( "invalid open in path '%ls'\n"
            , in_path.c_str()
        );

        return -1;
    }

    fprintf( f_out, "<DataBlock Name = \"%s\">\n", utf8_movie_name );

    resource_provider_t * provider = new resource_provider_t;
    provider->f = f_out;
    strcpy( provider->movie_name, utf8_movie_name );

    aeMovieData * movie_data = ae_create_movie_data( movie_instance, &my_resource_provider, &my_resource_deleter, provider );

    aeMovieStream * movie_stream = ae_create_movie_stream( movie_instance, &my_read_stream, &my_copy_stream, f_in );

    ae_uint32_t major_version;
    ae_uint32_t minor_version;
    ae_result_t result_load_movie_data = ae_load_movie_data( movie_data, movie_stream, &major_version, &minor_version );

    if( result_load_movie_data != AE_RESULT_SUCCESSFUL )
    {
        message_error( "invalid load movie data '%s'\n"
            , ae_get_result_string_info( result_load_movie_data )
        );

        return -1;
    }

    delete provider;
    	
    ae_delete_movie_stream( movie_stream );

    fclose( f_in );

    ae_delete_movie_data( movie_data );

    ae_delete_movie_instance( movie_instance );

    fprintf( f_out, "</DataBlock>\n" );

    fclose( f_out );

	return 0;
}