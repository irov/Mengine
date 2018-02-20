#	include "ToolUtils/ToolUtils.h"

#	include <vector>
#	include <string>
#	include <sstream>

#   include "movie/movie.hpp"

//////////////////////////////////////////////////////////////////////////
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
int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)nShowCmd;

    Sleep( 10000 );

	std::wstring in_path = parse_kwds( lpCmdLine, L"--in_path", std::wstring() );
	std::wstring out_path = parse_kwds( lpCmdLine, L"--out_path", std::wstring() );
    std::wstring hash = parse_kwds( lpCmdLine, L"--hash", std::wstring() );
    
	if( in_path.empty() == true )
	{
		message_error("not found 'in' param\n"
			);

		return 0;
	}

    if( out_path.empty() == true )
    {
        message_error( "not found 'out' param\n"
        );

        return 0;
    }

    CHAR utf8_hash[64];

    DWORD dwConversionFlags = WC_ERR_INVALID_CHARS;

    int utf8_size = ::WideCharToMultiByte(
        CP_UTF8
        , dwConversionFlags
        , hash.c_str()
        , hash.size()
        , utf8_hash
        , 64
        , NULL
        , NULL
    );

    const aeMovieInstance *  movie_instance = ae_create_movie_instance( utf8_hash
        , &stdlib_movie_alloc
        , &stdlib_movie_alloc_n
        , &stdlib_movie_free
        , &stdlib_movie_free_n
        , 0
        , &stdlib_movie_logerror, AE_NULL );

    	
	return 0;
}