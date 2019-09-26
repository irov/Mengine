#include "ResourceMovie2Validator.h"

#include "Interface/ResourceServiceInterface.h"

#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"

#include "Movie2Interface.h"

#include "Kernel/Stream.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceMovie2Validator::ResourceMovie2Validator()
        : m_movieInstance( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceMovie2Validator::~ResourceMovie2Validator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie2Validator::setMovieInstance( const aeMovieInstance * _movieInstance )
    {
        m_movieInstance = _movieInstance;
    }
    //////////////////////////////////////////////////////////////////////////
    const aeMovieInstance * ResourceMovie2Validator::getMovieInstance() const
    {
        return m_movieInstance;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie2Validator::setArchivator( const ArchivatorInterfacePtr & _archivator )
    {
        m_archivator = _archivator;
    }
    //////////////////////////////////////////////////////////////////////////
    const ArchivatorInterfacePtr & ResourceMovie2Validator::getArchivator() const
    {
        return m_archivator;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_size_t __movie_read_stream( ae_voidptr_t _buff, ae_size_t _carriage, ae_size_t _size, ae_userdata_t _userdata )
    {
        ae_uint8_t * memory_userdata = (ae_uint8_t *)_userdata;

        stdex::memorycopy( _buff, 0U, memory_userdata + _carriage, _size );

        return _size;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_copy_stream( ae_constvoidptr_t _src, ae_voidptr_t _dst, ae_size_t _size, ae_userdata_t _userdata )
    {
        MENGINE_UNUSED( _userdata );

        stdex::memorycopy( _dst, 0U, _src, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_bool_t __movie_resource_provider( const aeMovieResource * _resource, ae_voidptrptr_t _rd, ae_voidptr_t _ud )
    {
        MENGINE_UNUSED( _rd );

        ResourceMovie2 * resourceMovie2 = (ResourceMovie2 *)_ud;
        MENGINE_UNUSED( resourceMovie2 );

        aeMovieResourceTypeEnum resource_type = _resource->type;

        switch( resource_type )
        {
        case AE_MOVIE_RESOURCE_IMAGE:
            {
                const aeMovieResourceImage * resource_image = (const aeMovieResourceImage *)_resource;

                const ResourcePtr & resource = RESOURCE_SERVICE()
                    ->getResourceReference( Helper::stringizeString( resource_image->name ) );

                if( resource == nullptr )
                {
                    return AE_FALSE;
                }

                RESOURCEVALIDATE_SERVICE()
                    ->validResource( resource );

                return AE_TRUE;
            }break;
        case AE_MOVIE_RESOURCE_VIDEO:
            {
                const aeMovieResourceVideo * resource_video = (const aeMovieResourceVideo *)_resource;

                const ResourcePtr & resource = RESOURCE_SERVICE()
                    ->getResourceReference( Helper::stringizeString( resource_video->name ) );

                if( resource == nullptr )
                {
                    return AE_FALSE;
                }

                RESOURCEVALIDATE_SERVICE()
                    ->validResource( resource );

                return AE_TRUE;
            }break;
        case AE_MOVIE_RESOURCE_SOUND:
            {
                const aeMovieResourceSound * resource_sound = (const aeMovieResourceSound *)_resource;

                const ResourcePtr & resource = RESOURCE_SERVICE()
                    ->getResourceReference( Helper::stringizeString( resource_sound->name ) );

                if( resource == nullptr )
                {
                    return AE_FALSE;
                }

                RESOURCEVALIDATE_SERVICE()
                    ->validResource( resource );

                return AE_TRUE;
            }break;
        case AE_MOVIE_RESOURCE_PARTICLE:
            {
                const aeMovieResourceParticle * resource_particle = (const aeMovieResourceParticle *)_resource;

                const ResourcePtr & resource = RESOURCE_SERVICE()
                    ->getResourceReference( Helper::stringizeString( resource_particle->name ) );

                if( resource == nullptr )
                {
                    return AE_FALSE;
                }

                RESOURCEVALIDATE_SERVICE()
                    ->validResource( resource );

                return AE_TRUE;
            }break;
        default:
            break;
        }

        return AE_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_bool_t __movie_layer_data_visitor( const aeMovieCompositionData * _compositionData, const aeMovieLayerData * _layerData, ae_voidptr_t _ud )
    {
        ResourceMovie2 * resourceMovie2 = (ResourceMovie2 *)_ud;

        const ae_char_t * compositionDataName = ae_get_movie_composition_data_name( _compositionData );
        const ae_char_t * layerDataName = ae_get_movie_layer_data_name( _layerData );

        aeMovieLayerTypeEnum layerType = ae_get_movie_layer_data_type( _layerData );

        switch( layerType )
        {
        case AE_MOVIE_LAYER_TYPE_TEXT:
            {
                if( ae_test_movie_layer_data_opacity_transparent( _layerData ) == AE_TRUE )
                {
                    LOGGER_ERROR( "'%s' composition '%s' text layer '%s' opacity transparent"
                        , resourceMovie2->getName().c_str()
                        , compositionDataName
                        , layerDataName
                    );
                }
            }break;
        default:
            {
            }break;
        }

        return AE_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie2Validator::_validate( const ResourceMovie2Ptr & _resource )
    {
        const ContentInterface * content = _resource->getContent();

        const FilePath & filePath = content->getFilePath();

        if( filePath.empty() == true )
        {
            LOGGER_ERROR( "movie2 '%s' group '%s' don`t have Key Frames Pack Path"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
            );

            return false;
        }

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( fileGroup, filePath, false, MENGINE_DOCUMENT_FUNCTION );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "movie2 '%s' group '%s' can`t open file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFilePath().c_str()
            );

            return false;
        }

        MemoryInterfacePtr memory = Helper::loadStreamArchiveData( stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_AEZ ), GET_MAGIC_VERSION( MAGIC_AEZ ), MENGINE_DOCUMENT_FUNCTION );

        if( memory == nullptr )
        {
            LOGGER_ERROR( "movie2 '%s' group '%s' can`t load stream archive file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFilePath().c_str()
            );

            return false;
        }

        void * memory_buffer = memory->getBuffer();

        aeMovieDataProviders data_providers;
        ae_clear_movie_data_providers( &data_providers );

        data_providers.resource_provider = &__movie_resource_provider;

        aeMovieData * movieData = ae_create_movie_data( m_movieInstance, &data_providers, (ae_voidptr_t)_resource.get() );

        if( movieData == nullptr )
        {
            return false;
        }

        aeMovieStream * movieStream = ae_create_movie_stream( m_movieInstance, &__movie_read_stream, &__movie_copy_stream, memory_buffer );

        if( movieStream == nullptr )
        {
            return false;
        }

        ae_uint32_t major_version;
        ae_uint32_t minor_version;
        ae_result_t result_load_movie_data = ae_load_movie_data( movieData, movieStream, &major_version, &minor_version );

        if( result_load_movie_data != AE_RESULT_SUCCESSFUL )
        {
            const ae_char_t * result_string_info = ae_get_result_string_info( result_load_movie_data );

            LOGGER_ERROR( "movie2 '%s' group '%s' file '%s' check movie data invalid '%s'\ncurrent version '%u.%u'\nload version '%u.%u'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFilePath().c_str()
                , result_string_info
                , AE_MOVIE_SDK_MAJOR_VERSION
                , AE_MOVIE_SDK_MINOR_VERSION
                , major_version
                , minor_version
            );

            return false;
        }

        ae_visit_movie_layer_data( movieData, &__movie_layer_data_visitor, (ae_voidptr_t)_resource.get() );

        ae_delete_movie_data( movieData );
        ae_delete_movie_stream( movieStream );

        return true;
    }
}