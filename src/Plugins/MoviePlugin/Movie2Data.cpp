#include "Movie2Data.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/MaterialEnumInterface.h"

#include "Kernel/Materialable.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t aeMovieResourceImageAcquire( const Movie2Data * _data, const aeMovieLayerData * _layer, const aeMovieResourceImage * _resourceImage )
        {
            MENGINE_UNUSED( _data );
            MENGINE_UNUSED( _layer );

            const aeMovieResource * resource = reinterpret_cast<const aeMovieResource *>(_resourceImage);

            ae_userdata_t resourceUserdata = ae_get_movie_resource_userdata( resource );

            Movie2Data::ImageDesc * imageDesc = reinterpret_cast<Movie2Data::ImageDesc *>(resourceUserdata);

            ResourceImage * resourceImage = imageDesc->resourceImage;

            if( resourceImage->compile() == false )
            {
                return AE_FALSE;
            }

            if( ++imageDesc->refCount == 1 )
            {
                imageDesc->materials[EMB_NORMAL] = Helper::makeImageMaterial( resourceImage, ConstString::none(), EMB_NORMAL, false, false, MENGINE_DOCUMENT_FORWARD_PTR( _data ) );
                imageDesc->materials[EMB_ADD] = Helper::makeImageMaterial( resourceImage, ConstString::none(), EMB_ADD, false, false, MENGINE_DOCUMENT_FORWARD_PTR( _data ) );
                imageDesc->materials[EMB_SCREEN] = Helper::makeImageMaterial( resourceImage, ConstString::none(), EMB_SCREEN, false, false, MENGINE_DOCUMENT_FORWARD_PTR( _data ) );
                imageDesc->materials[EMB_MULTIPLY] = Helper::makeImageMaterial( resourceImage, ConstString::none(), EMB_MULTIPLY, false, false, MENGINE_DOCUMENT_FORWARD_PTR( _data ) );
            }

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t aeMovieLayerDataVisitorAcquire( const aeMovieCompositionData * _compositionData, const aeMovieLayerData * _layer, ae_userdata_t _userdata )
        {
            AE_UNUSED( _compositionData );

            const Movie2Data * data = reinterpret_cast<const Movie2Data *>(_userdata);

            aeMovieResourceTypeEnum resourceType = ae_get_movie_layer_data_resource_type( _layer );

            switch( resourceType )
            {
            case AE_MOVIE_RESOURCE_SEQUENCE:
                {
                    const aeMovieResource * resource = ae_get_movie_layer_data_resource( _layer );

                    const aeMovieResourceSequence * movieResourceSequence = reinterpret_cast<const aeMovieResourceSequence *>(resource);

                    for( ae_uint32_t index = 0; index != movieResourceSequence->image_count; ++index )
                    {
                        const aeMovieResourceImage * movieResourceImage = movieResourceSequence->images[index];

                        if( Detail::aeMovieResourceImageAcquire( data, _layer, movieResourceImage ) == AE_FALSE )
                        {
                            return AE_FALSE;
                        }
                    }

                    return AE_TRUE;
                }break;
            case AE_MOVIE_RESOURCE_IMAGE:
                {
                    const aeMovieResource * resource = ae_get_movie_layer_data_resource( _layer );

                    const aeMovieResourceImage * movieResourceImage = reinterpret_cast<const aeMovieResourceImage *>(resource);

                    if( Detail::aeMovieResourceImageAcquire( data, _layer, movieResourceImage ) == AE_FALSE )
                    {
                        return AE_FALSE;
                    }

                    return AE_TRUE;
                }break;
            case AE_MOVIE_RESOURCE_VIDEO:
            case AE_MOVIE_RESOURCE_SOUND:
            case AE_MOVIE_RESOURCE_PARTICLE:
                {
                    ae_userdata_t resourceUserdata = ae_get_movie_layer_data_resource_userdata( _layer );

                    if( resourceUserdata == AE_USERDATA_NULL )
                    {
                        return AE_TRUE;
                    }

                    Resource * resource = reinterpret_cast<Resource *>(resourceUserdata);

                    if( resource->compile() == false )
                    {
                        return AE_FALSE;
                    }

                    return AE_TRUE;
                }break;
            default:
                break;
            }

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static void aeMovieResourceImageRelease( const aeMovieResourceImage * _resourceImage )
        {
            const aeMovieResource * resource = reinterpret_cast<const aeMovieResource *>(_resourceImage);

            ae_userdata_t resourceUserdata = ae_get_movie_resource_userdata( resource );

            Movie2Data::ImageDesc * imageDesc = reinterpret_cast<Movie2Data::ImageDesc *>(resourceUserdata);

            ResourceImage * resourceImage = imageDesc->resourceImage;

            resourceImage->release();

            if( --imageDesc->refCount == 0 )
            {
                imageDesc->materials[EMB_NORMAL] = nullptr;
                imageDesc->materials[EMB_ADD] = nullptr;
                imageDesc->materials[EMB_SCREEN] = nullptr;
                imageDesc->materials[EMB_MULTIPLY] = nullptr;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t aeMovieLayerDataVisitorRelease( const aeMovieCompositionData * _compositionData, const aeMovieLayerData * _layer, ae_userdata_t _userdata )
        {
            AE_UNUSED( _compositionData );
            AE_UNUSED( _userdata );

            aeMovieResourceTypeEnum resourceType = ae_get_movie_layer_data_resource_type( _layer );

            switch( resourceType )
            {
            case AE_MOVIE_RESOURCE_SEQUENCE:
                {
                    const aeMovieResource * resource = ae_get_movie_layer_data_resource( _layer );

                    const aeMovieResourceSequence * movieResourceSequence = reinterpret_cast<const aeMovieResourceSequence *>(resource);

                    for( ae_uint32_t index = 0; index != movieResourceSequence->image_count; ++index )
                    {
                        const aeMovieResourceImage * movieResourceImage = movieResourceSequence->images[index];

                        Detail::aeMovieResourceImageRelease( movieResourceImage );
                    }

                    return AE_TRUE;
                }break;
            case AE_MOVIE_RESOURCE_IMAGE:
                {
                    const aeMovieResource * resource = ae_get_movie_layer_data_resource( _layer );

                    const aeMovieResourceImage * movieResourceImage = reinterpret_cast<const aeMovieResourceImage *>(resource);

                    Detail::aeMovieResourceImageRelease( movieResourceImage );

                    return AE_TRUE;
                }break;
            case AE_MOVIE_RESOURCE_VIDEO:
            case AE_MOVIE_RESOURCE_SOUND:
            case AE_MOVIE_RESOURCE_PARTICLE:
                {
                    ae_userdata_t resourceUserdata = ae_get_movie_layer_data_resource_userdata( _layer );

                    if( resourceUserdata == AE_USERDATA_NULL )
                    {
                        return AE_TRUE;
                    }

                    Resource * resource = reinterpret_cast<Resource *>(resourceUserdata);

                    resource->release();

                    return AE_TRUE;
                }break;
            default:
                break;
            }

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t aeMovieLayerDataVisitorGet( const aeMovieCompositionData * _compositionData, const aeMovieLayerData * _layer, ae_userdata_t _userdata )
        {
            AE_UNUSED( _compositionData );

            const Movie2DataInterface::LambdaCompositionDataResource * lambda = reinterpret_cast<const Movie2DataInterface::LambdaCompositionDataResource *>(_userdata);

            ae_uint32_t layerIndex = ae_get_movie_layer_data_index( _layer );
            const ae_char_t * layerName = ae_get_movie_layer_data_name( _layer );
            aeMovieResourceTypeEnum layerResourceType = ae_get_movie_layer_data_resource_type( _layer );

            switch( layerResourceType )
            {
            case AE_MOVIE_RESOURCE_SEQUENCE:
                {
                    const aeMovieResource * resource = ae_get_movie_layer_data_resource( _layer );

                    const aeMovieResourceSequence * movieResourceSequence = reinterpret_cast<const aeMovieResourceSequence *>(resource);

                    for( ae_uint32_t index = 0; index != movieResourceSequence->image_count; ++index )
                    {
                        const aeMovieResourceImage * movieResourceImage = movieResourceSequence->images[index];

                        const aeMovieResource * movieResourceImageBase = reinterpret_cast<const aeMovieResource *>(movieResourceImage);

                        ae_userdata_t resourceUserdata = ae_get_movie_resource_userdata( movieResourceImageBase );

                        Movie2Data::ImageDesc * imageDesc = reinterpret_cast<Movie2Data::ImageDesc *>(resourceUserdata);

                        ResourceImage * resourceImage = imageDesc->resourceImage;

                        (*lambda)(layerIndex, layerName, ResourcePtr::from( resourceImage ));
                    }

                    return AE_TRUE;
                }break;
            case AE_MOVIE_RESOURCE_IMAGE:
                {
                    const aeMovieResource * resource = ae_get_movie_layer_data_resource( _layer );

                    ae_userdata_t resourceUserdata = ae_get_movie_resource_userdata( resource );

                    Movie2Data::ImageDesc * imageDesc = reinterpret_cast<Movie2Data::ImageDesc *>(resourceUserdata);

                    ResourceImage * resourceImage = imageDesc->resourceImage;

                    (*lambda)(layerIndex, layerName, ResourcePtr::from( resourceImage ));

                    return AE_TRUE;
                }break;
            case AE_MOVIE_RESOURCE_VIDEO:
            case AE_MOVIE_RESOURCE_SOUND:
            case AE_MOVIE_RESOURCE_PARTICLE:
                {
                    ae_userdata_t resourceUserdata = ae_get_movie_layer_data_resource_userdata( _layer );

                    if( resourceUserdata == AE_USERDATA_NULL )
                    {
                        return AE_TRUE;
                    }

                    Resource * resource = reinterpret_cast<Resource *>(resourceUserdata);

                    (*lambda)(layerIndex, layerName, ResourcePtr::from( resource ));

                    return AE_TRUE;
                }break;
            default:
                break;
            }

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2Data::Movie2Data()
        : m_movieData( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2Data::~Movie2Data()
    {
        if( m_movieData != nullptr )
        {
            ae_delete_movie_data( m_movieData );
            m_movieData = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Data::setGroupName( const ConstString & _groupName )
    {
        m_groupName = _groupName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Movie2Data::getGroupName() const
    {
        return m_groupName;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Data::setMovieData( const aeMovieData * _movieData )
    {
        m_movieData = _movieData;
    }
    //////////////////////////////////////////////////////////////////////////
    const aeMovieData * Movie2Data::getMovieData() const
    {
        return m_movieData;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2Data::acquire()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Data::release()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2Data::acquireCompositionData( const aeMovieCompositionData * _compositionData )
    {
        if( ae_visit_composition_layer_data( _compositionData, &Detail::aeMovieLayerDataVisitorAcquire, this ) == AE_FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Data::releaseCompositionData( const aeMovieCompositionData * _compositionData )
    {
        ae_visit_composition_layer_data( _compositionData, &Detail::aeMovieLayerDataVisitorRelease, this );
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Data::foreachCompositionDataResources( const aeMovieCompositionData * _compositionData, const LambdaCompositionDataResource & _lambda )
    {
        ae_visit_composition_layer_data( _compositionData, &Detail::aeMovieLayerDataVisitorGet, const_cast<void *>(reinterpret_cast<const void *>(&_lambda)) );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourcePtr & Movie2Data::getResource( const Char * _resourceName )
    {
        const ConstString & resourceName = Helper::stringizeString( _resourceName );

        const ResourcePtr & resource = RESOURCE_SERVICE()
            ->getResourceReference( m_groupName, resourceName );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, "not found resource '%s'"
            , _resourceName
        );

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2Data::ImageDesc * Movie2Data::makeImageDesc( const ResourcePtr & _resource )
    {
        ImageDesc * desc = m_poolImageDesc.createT();

        desc->refCount = 0;
        desc->resourceImage = Helper::staticResourceCast<ResourceImage *>( _resource.get() );

        return desc;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Data::removeImageDesc( ImageDesc * _desc )
    {
        m_poolImageDesc.destroyT( _desc );
    }
    //////////////////////////////////////////////////////////////////////////
}
