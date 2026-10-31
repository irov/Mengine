#include "ResourceMovie2Validator.h"

#include "Interface/ResourceServiceInterface.h"

#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"

#include "Movie2Interface.h"

#include "Kernel/StreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/MemoryCopy.h"

#include "Config/Abs.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        struct MovieLayerDataVisitorDesc
        {
            ResourceMovie2Ptr resource;
            bool successful;
        };
        //////////////////////////////////////////////////////////////////////////
        static ae_size_t movieReadStream( ae_voidptr_t _buffer, ae_size_t _carriage, ae_size_t _size, ae_userdata_t _userdata )
        {
            const MemoryInterface * memory = static_cast<const MemoryInterface *>(_userdata);
            size_t memorySize = memory->getSize();

            if( _carriage >= memorySize )
            {
                return 0;
            }

            if( _size > memorySize - _carriage )
            {
                return 0;
            }

            const void * memoryBuffer = memory->getBuffer();
            Helper::memoryCopy( _buffer, 0, memoryBuffer, _carriage, _size );

            return _size;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t movieCopyStream( ae_constvoidptr_t _src, ae_voidptr_t _dst, ae_size_t _size, ae_userdata_t _userdata )
        {
            MENGINE_UNUSED( _userdata );

            Helper::memoryCopy( _dst, 0, _src, 0, _size );
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t movieResourceProvider( const aeMovieResource * _resource, ae_voidptrptr_t _resourceData, ae_voidptr_t _userdata )
        {
            MENGINE_UNUSED( _resourceData );

            ResourceMovie2 * resourceMovie2 = (ResourceMovie2 *)_userdata;
            MENGINE_UNUSED( resourceMovie2 );

            const ConstString & groupName = resourceMovie2->getGroupName();

            aeMovieResourceTypeEnum resourceType = _resource->type;

            switch( resourceType )
            {
            case AE_MOVIE_RESOURCE_IMAGE:
                {
                    const aeMovieResourceImage * movieResourceImage = reinterpret_cast<const aeMovieResourceImage *>(_resource);

                    const Char * movieResourceName = movieResourceImage->name;
                    const ConstString & resourceName = Helper::stringizeString( movieResourceName );

                    const ResourcePtr & resource = RESOURCE_SERVICE()
                        ->getResourceReference( groupName, resourceName );

                    if( resource == nullptr )
                    {
                        return AE_FALSE;
                    }

                    RESOURCEVALIDATE_SERVICE()
                        ->validateResource( resource );

                    return AE_TRUE;
                }break;
            case AE_MOVIE_RESOURCE_VIDEO:
                {
                    const aeMovieResourceVideo * movieResourceVideo = reinterpret_cast<const aeMovieResourceVideo *>(_resource);

                    const Char * movieResourceName = movieResourceVideo->name;
                    const ConstString & resourceName = Helper::stringizeString( movieResourceName );

                    const ResourcePtr & resource = RESOURCE_SERVICE()
                        ->getResourceReference( groupName, resourceName );

                    if( resource == nullptr )
                    {
                        return AE_FALSE;
                    }

                    RESOURCEVALIDATE_SERVICE()
                        ->validateResource( resource );

                    return AE_TRUE;
                }break;
            case AE_MOVIE_RESOURCE_SOUND:
                {
                    const aeMovieResourceSound * movieResourceSound = reinterpret_cast<const aeMovieResourceSound *>(_resource);

                    const Char * movieResourceName = movieResourceSound->name;
                    const ConstString & resourceName = Helper::stringizeString( movieResourceName );

                    const ResourcePtr & resource = RESOURCE_SERVICE()
                        ->getResourceReference( groupName, resourceName );

                    if( resource == nullptr )
                    {
                        return AE_FALSE;
                    }

                    RESOURCEVALIDATE_SERVICE()
                        ->validateResource( resource );

                    return AE_TRUE;
                }break;
            case AE_MOVIE_RESOURCE_PARTICLE:
                {
                    const aeMovieResourceParticle * movieResourceParticle = reinterpret_cast<const aeMovieResourceParticle *>(_resource);

                    const Char * movieResourceName = movieResourceParticle->name;
                    const ConstString & resourceName = Helper::stringizeString( movieResourceName );

                    const ResourcePtr & resource = RESOURCE_SERVICE()
                        ->getResourceReference( groupName, resourceName );

                    if( resource == nullptr )
                    {
                        return AE_FALSE;
                    }

                    RESOURCEVALIDATE_SERVICE()
                        ->validateResource( resource );

                    return AE_TRUE;
                }break;
            default:
                break;
            }

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool isMovieLayerScaleValid( float _x, float _y, float _z, float _minimum )
        {
            if( (MENGINE_ABS( _x ) >= _minimum) == false )
            {
                return false;
            }

            if( (MENGINE_ABS( _y ) >= _minimum) == false )
            {
                return false;
            }

            if( (MENGINE_ABS( _z ) >= _minimum) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t movieLayerDataVisitor( const aeMovieCompositionData * _compositionData, const aeMovieLayerData * _layerData, ae_voidptr_t _userdata )
        {
            MovieLayerDataVisitorDesc * desc = (MovieLayerDataVisitorDesc *)_userdata;

            const ae_char_t * compositionDataName = ae_get_movie_composition_data_name( _compositionData );
            const ae_char_t * layerDataName = ae_get_movie_layer_data_name( _layerData );

            aeMovieLayerTypeEnum layerType = ae_get_movie_layer_data_type( _layerData );

            switch( layerType )
            {
            case AE_MOVIE_LAYER_TYPE_SLOT:
                {
                    ae_float_t immutableOpacity;
                    if( ae_get_movie_layer_data_immutable_opacity( _layerData, &immutableOpacity ) == AE_FALSE )
                    {
                        break;
                    }

                    if( immutableOpacity >= MENGINE_COLOR_MINIMAL_ALPHA )
                    {
                        break;
                    }

                    LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' composition '%s' slot layer '%s' opacity transparent"
                        , desc->resource->getName().c_str()
                        , compositionDataName
                        , layerDataName
                    );

                    desc->successful = false;
                }break;
            case AE_MOVIE_LAYER_TYPE_TEXT:
                {
                    ae_float_t immutableOpacity;
                    if( ae_get_movie_layer_data_immutable_opacity( _layerData, &immutableOpacity ) == AE_FALSE )
                    {
                        break;
                    }

                    if( immutableOpacity >= MENGINE_COLOR_MINIMAL_ALPHA )
                    {
                        break;
                    }

                    LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' composition '%s' text layer '%s' opacity transparent"
                        , desc->resource->getName().c_str()
                        , compositionDataName
                        , layerDataName
                    );

                    desc->successful = false;
                }break;
            case AE_MOVIE_LAYER_TYPE_IMAGE:
                {
                    ae_float_t immutableScaleX;
                    ae_float_t immutableScaleY;
                    ae_float_t immutableScaleZ;
                    if( ae_get_movie_layer_data_immutable_scale( _layerData, &immutableScaleX, &immutableScaleY, &immutableScaleZ ) == AE_FALSE )
                    {
                        break;
                    }

                    float imageImmutableScaleLimit = CONFIG_VALUE_FLOAT( "Limit", "Movie2ImageImmutableScale", 0.8f );

                    if( Detail::isMovieLayerScaleValid( immutableScaleX, immutableScaleY, immutableScaleZ, imageImmutableScaleLimit ) == true )
                    {
                        break;
                    }

                    LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' composition '%s' image layer '%s' scale [%f - %f - %f] limit %f (Limit.Movie2ImageImmutableScale)"
                        , desc->resource->getName().c_str()
                        , compositionDataName
                        , layerDataName
                        , immutableScaleX
                        , immutableScaleY
                        , immutableScaleZ
                        , imageImmutableScaleLimit
                    );

                    desc->successful = false;
                }break;
            case AE_MOVIE_LAYER_TYPE_VIDEO:
                {
                    ae_float_t immutableScaleX;
                    ae_float_t immutableScaleY;
                    ae_float_t immutableScaleZ;
                    if( ae_get_movie_layer_data_immutable_scale( _layerData, &immutableScaleX, &immutableScaleY, &immutableScaleZ ) == AE_FALSE )
                    {
                        break;
                    }

                    float videoImmutableScaleLimit = CONFIG_VALUE_FLOAT( "Limit", "Movie2VideoImmutableScale", 0.9f );

                    if( Detail::isMovieLayerScaleValid( immutableScaleX, immutableScaleY, immutableScaleZ, videoImmutableScaleLimit ) == true )
                    {
                        break;
                    }

                    LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' composition '%s' video layer '%s' scale [%f - %f - %f] limit %f"
                        , desc->resource->getName().c_str()
                        , compositionDataName
                        , layerDataName
                        , immutableScaleX
                        , immutableScaleY
                        , immutableScaleZ
                        , videoImmutableScaleLimit
                    );

                    desc->successful = false;
                }break;
            default:
                {
                }break;
            }

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t movieCompositionNodeProvider( const aeMovieNodeProviderCallbackData * _callbackData, ae_voidptrptr_t _node, ae_voidptr_t _userdata )
        {
            MovieLayerDataVisitorDesc * desc = (MovieLayerDataVisitorDesc *)_userdata;

            const aeMovieLayerData * layerData = _callbackData->layer_data;

            if( _callbackData->track_matte_layer_data != AE_NULLPTR )
            {
                aeMovieLayerTypeEnum layerType = ae_get_movie_layer_data_type( layerData );

                switch( layerType )
                {
                case AE_MOVIE_LAYER_TYPE_IMAGE:
                    {
                        aeMovieLayerTypeEnum trackMatteLayerType = ae_get_movie_layer_data_type( _callbackData->track_matte_layer_data );

                        if( trackMatteLayerType != AE_MOVIE_LAYER_TYPE_IMAGE )
                        {
                            const ae_char_t * compositionDataName = ae_get_movie_composition_data_name( _callbackData->composition_data );
                            const ae_char_t * layerDataName = ae_get_movie_layer_data_name( _callbackData->layer_data );

                            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' composition '%s' layer '%s' has track matte non-image type"
                                , desc->resource->getName().c_str()
                                , compositionDataName
                                , layerDataName
                            );

                            desc->successful = false;
                        }

                        return AE_TRUE;
                    }break;
                case AE_MOVIE_LAYER_TYPE_SEQUENCE:
                    {
                        aeMovieLayerTypeEnum trackMatteLayerType = ae_get_movie_layer_data_type( _callbackData->track_matte_layer_data );

                        if( trackMatteLayerType != AE_MOVIE_LAYER_TYPE_IMAGE )
                        {
                            const ae_char_t * compositionDataName = ae_get_movie_composition_data_name( _callbackData->composition_data );
                            const ae_char_t * layerDataName = ae_get_movie_layer_data_name( _callbackData->layer_data );

                            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' composition '%s' text layer '%s' has track matte non-image type"
                                , desc->resource->getName().c_str()
                                , compositionDataName
                                , layerDataName
                            );

                            desc->successful = false;
                        }

                        return AE_TRUE;
                    }break;
                default:
                    {
                    }break;
                }
            }

            *_node = AE_NULLPTR;

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t movieCompositionDataVisitor( const aeMovieData * _movieData, const aeMovieCompositionData * _compositionData, ae_userdata_t _userdata )
        {
            AE_UNUSED( _movieData );

            aeMovieCompositionProviders compositionProviders;
            ae_initialize_movie_composition_providers( &compositionProviders );

            compositionProviders.node_provider = &movieCompositionNodeProvider;

            const aeMovieComposition * composition = ae_create_movie_composition( _compositionData, AE_TRUE, &compositionProviders, _userdata );

            if( composition == AE_NULLPTR )
            {
                return AE_FALSE;
            }

            ae_delete_movie_composition( composition );

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
    }
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
    bool ResourceMovie2Validator::_validate( const ResourceMovie2Ptr & _resource ) const
    {
        const ContentInterfacePtr & content = _resource->getContent();

        const FilePath & filePath = content->getFilePath();

        if( filePath.empty() == true )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "movie2 '%s' group '%s' don`t have Key Frames Pack Path"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
            );

            return false;
        }

        InputStreamInterfacePtr stream = content->openInputStreamFile( false, false, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "movie2 '%s' group '%s' can`t open file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFilePath().c_str()
            );

            return false;
        }

        MemoryInterfacePtr memory = Helper::readStreamArchiveMagic( stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_AEZ ), GET_MAGIC_VERSION( MAGIC_AEZ ), MENGINE_DOCUMENT_FACTORABLE );

        content->closeInputStreamFile( stream );

        if( memory == nullptr )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "movie2 '%s' group '%s' can`t load stream archive file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFilePath().c_str()
            );

            return false;
        }

        aeMovieDataProviders dataProviders;
        ae_clear_movie_data_providers( &dataProviders );

        dataProviders.resource_provider = &Detail::movieResourceProvider;

        aeMovieData * movieData = ae_create_movie_data( m_movieInstance, &dataProviders, (ae_voidptr_t)_resource.get() );

        if( movieData == nullptr )
        {
            return false;
        }

        aeMovieStream * movieStream = ae_create_movie_stream( m_movieInstance, &Detail::movieReadStream, &Detail::movieCopyStream, memory.get() );

        if( movieStream == nullptr )
        {
            return false;
        }

        ae_uint32_t majorVersion;
        ae_uint32_t minorVersion;
        ae_result_t resultLoadMovieData = ae_load_movie_data( movieData, movieStream, &majorVersion, &minorVersion );

        if( resultLoadMovieData != AE_RESULT_SUCCESSFUL )
        {
            const ae_char_t * resultStringInfo = ae_get_movie_result_string_info( resultLoadMovieData );

            LOGGER_MESSAGE_RELEASE_ERROR( "movie2 '%s' group '%s' file '%s' check movie data invalid '%s'\ncurrent version '%u.%u'\nload version '%u.%u'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFilePath().c_str()
                , resultStringInfo
                , AE_MOVIE_SDK_MAJOR_VERSION
                , AE_MOVIE_SDK_MINOR_VERSION
                , majorVersion
                , minorVersion
            );

            return false;
        }

        bool successful = true;

        Detail::MovieLayerDataVisitorDesc desc;
        desc.resource = _resource;
        desc.successful = true;

        if( ae_visit_movie_layer_data( movieData, &Detail::movieLayerDataVisitor, &desc ) == AE_FALSE )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "movie2 '%s' group '%s' file '%s' check movie data invalid"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFilePath().c_str()
            );

            successful = false;
        }

        if( desc.successful == false )
        {
            successful = false;
        }

        if( ae_visit_movie_composition_data( movieData, &Detail::movieCompositionDataVisitor, &desc ) == AE_FALSE )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "movie2 '%s' group '%s' file '%s' check movie data invalid"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFilePath().c_str()
            );

            successful = false;
        }

        ae_delete_movie_data( movieData );
        ae_delete_movie_stream( movieStream );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}
