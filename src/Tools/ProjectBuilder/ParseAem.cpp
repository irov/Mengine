#   include "ParseAem.h"

#   include "Interface/UnicodeInterface.h"
#   include "Interface/StreamInterface.h"
#   include "Interface/FileSystemInterface.h"

#   include "Config/String.h"

#   include "Logger/Logger.h"

#   include "movie/movie.hpp"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    static void * my_movie_alloc( void * _data, size_t _size )
    {
        (void)_data;

        void * ptr = malloc( _size );

        return ptr;
    }
    //////////////////////////////////////////////////////////////////////////
    static void * my_movie_alloc_n( void * _data, size_t _size, size_t _count )
    {
        (void)_data;

        size_t total = _size * _count;

        void * ptr = malloc( total );

        return ptr;
    }
    //////////////////////////////////////////////////////////////////////////
    static void my_movie_free( void * _data, const void * _ptr )
    {
        (void)_data;

        free( (void *)_ptr );
    }
    //////////////////////////////////////////////////////////////////////////
    static void my_movie_free_n( void * _data, const void * _ptr )
    {
        (void)_data;

        free( (void *)_ptr );
    }
    //////////////////////////////////////////////////////////////////////////
    static void my_movie_logerror( void * _data, aeMovieErrorCode _code, const char * _format, ... )
    {
        (void)_data;

        if( _code == AE_ERROR_STREAM )
        {
            return;
        }

        va_list argList;

        va_start( argList, _format );
        char msg[4096];
        vsprintf( msg, _format, argList );
        va_end( argList );

        printf( "MoviePlugin error '%s' code '%d'\n"
            , msg
            , _code
            );
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_voidptr_t my_resource_provider( const aeMovieResource * _resource, ae_voidptr_t _data )
    {
        PyObject * pylist = (PyObject *)_data;

        aeMovieResourceTypeEnum resource_type = _resource->type;

        switch( resource_type )
        {
        case AE_MOVIE_RESOURCE_IMAGE:
            {
                const aeMovieResourceImage * resource_image = (const aeMovieResourceImage *)_resource;
                
                PyObject * pypath = pybind::unicode_from_utf8( resource_image->path );
                pybind::list_appenditem( pylist, pypath );
                pybind::decref( pypath );

                return AE_NULL;
            }break;
        case AE_MOVIE_RESOURCE_VIDEO:
            {
                const aeMovieResourceVideo * resource_video = (const aeMovieResourceVideo *)_resource;

                PyObject * pypath = pybind::unicode_from_utf8( resource_video->path );
                pybind::list_appenditem( pylist, pypath );
                pybind::decref( pypath );

                return AE_NULL;
            }break;
        case AE_MOVIE_RESOURCE_SOUND:
            {
                const aeMovieResourceSound * resource_sound = (const aeMovieResourceSound *)_resource;

                PyObject * pypath = pybind::unicode_from_utf8( resource_sound->path );
                pybind::list_appenditem( pylist, pypath );
                pybind::decref( pypath );

                return AE_NULL;
            }break;
        case AE_MOVIE_RESOURCE_PARTICLE:
            {
                const aeMovieResourceParticle * resource_particle = (const aeMovieResourceParticle *)_resource;

                PyObject * pypath = pybind::unicode_from_utf8( resource_particle->path );
                pybind::list_appenditem( pylist, pypath );
                pybind::decref( pypath );

                return AE_NULL;
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
        (void)_carriage;

        InputStreamInterface * stream = (InputStreamInterface *)_data;

        size_t bytes = stream->read( _buff, _size );

        return bytes;
    }
    //////////////////////////////////////////////////////////////////////////
    static void my_copy_stream( void * _data, const void * _src, void * _dst, size_t _size )
    {
        (void)_data;

        memcpy( _dst, _src, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    PyObject * parseAem( const wchar_t * hash, const wchar_t * aemPath )
    {
        String utf8_aemPath;
        Helper::unicodeToUtf8( aemPath, utf8_aemPath );

        FilePath inputFileName = Helper::stringizeFilePath( utf8_aemPath );
    
        InputStreamInterfacePtr input_stream = FILE_SERVICE()
            ->openInputFile( ConstString::none(), inputFileName, false );

        if( input_stream == nullptr )
        {
            LOGGER_ERROR( "spreadingPngAlpha invalid PNG file '%s' not found"
                , inputFileName.c_str()
                );

            return pybind::ret_none();
        }

        String utf8_hash;
        Helper::unicodeToUtf8( hash, utf8_hash );

        const aeMovieInstance * movieInstance = ae_create_movie_instance( utf8_hash.c_str()
            , &my_movie_alloc
            , &my_movie_alloc_n
            , &my_movie_free
            , &my_movie_free_n
            , 0
            , &my_movie_logerror
            , NULL );

        PyObject * pylist = pybind::list_new( 0 );

        aeMovieData * movieData = ae_create_movie_data( movieInstance, &my_resource_provider, &my_resource_deleter, pylist );

        aeMovieStream * movie_stream = ae_create_movie_stream( movieInstance, &my_read_stream, &my_copy_stream, input_stream.get() );

        ae_uint32_t load_version;
        ae_result_t successful = ae_load_movie_data( movieData, movie_stream, &load_version );

        ae_delete_movie_stream( movie_stream );

        ae_delete_movie_data( movieData );

        ae_delete_movie_instance( movieInstance );

        if( successful != AE_RESULT_SUCCESSFUL )
        {
            return pybind::ret_none();
        }

        return pylist;
    }
}