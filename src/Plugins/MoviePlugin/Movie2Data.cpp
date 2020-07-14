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
    static ae_bool_t __ae_movie_resource_image_acquire( const aeMovieResourceImage * _resource, const Movie2Data * _data )
    {
        MENGINE_UNUSED( _data );

        ae_userdata_t resource_ud = ae_get_movie_resource_userdata( (const aeMovieResource * )_resource );

        Movie2Data::ImageDesc * image_desc = reinterpret_cast<Movie2Data::ImageDesc *>(resource_ud);

        const ResourceImagePtr & resource_image = image_desc->resourceImage;

        if( resource_image->compile() == false )
        {
            return AE_FALSE;
        }

        if( ++image_desc->refcount == 1 )
        {
            image_desc->materials[EMB_NORMAL] = Helper::makeImageMaterial( resource_image, ConstString::none(), EMB_NORMAL, false, false, MENGINE_DOCUMENT_FACTORABLE_PTR( _data ) );
            image_desc->materials[EMB_ADD] = Helper::makeImageMaterial( resource_image, ConstString::none(), EMB_ADD, false, false, MENGINE_DOCUMENT_FACTORABLE_PTR( _data ) );
            image_desc->materials[EMB_SCREEN] = Helper::makeImageMaterial( resource_image, ConstString::none(), EMB_SCREEN, false, false, MENGINE_DOCUMENT_FACTORABLE_PTR( _data ) );
            image_desc->materials[EMB_MULTIPLY] = Helper::makeImageMaterial( resource_image, ConstString::none(), EMB_MULTIPLY, false, false, MENGINE_DOCUMENT_FACTORABLE_PTR( _data ) );
        }

        return AE_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_bool_t __ae_movie_layer_data_visitor_acquire( const aeMovieCompositionData * _compositionData, const aeMovieLayerData * _layer, ae_userdata_t _ud )
    {
        AE_UNUSED( _compositionData );

        const Movie2Data * data = reinterpret_cast<const Movie2Data *>(_ud);

        aeMovieResourceTypeEnum resource_type = ae_get_movie_layer_data_resource_type( _layer );

        switch( resource_type )
        {
        case AE_MOVIE_RESOURCE_SEQUENCE:
            {
                const aeMovieResource * resource = ae_get_movie_layer_data_resource( _layer );

                const aeMovieResourceSequence * resource_sequence = (const aeMovieResourceSequence *)resource;

                for( ae_uint32_t index = 0; index != resource_sequence->image_count; ++index )
                {
                    const aeMovieResourceImage * resource_image = resource_sequence->images[index];

                    if( __ae_movie_resource_image_acquire( resource_image, data ) == AE_FALSE )
                    {
                        return AE_FALSE;
                    }
                }

                return AE_TRUE;
            }break;
        case AE_MOVIE_RESOURCE_IMAGE:
            {
                const aeMovieResource * resource = ae_get_movie_layer_data_resource( _layer );

                const aeMovieResourceImage * resource_image = reinterpret_cast<const aeMovieResourceImage *>(resource);

                if( __ae_movie_resource_image_acquire( resource_image, data ) == AE_FALSE )
                {
                    return AE_FALSE;
                }

                return AE_TRUE;
            }break;
        case AE_MOVIE_RESOURCE_VIDEO:
        case AE_MOVIE_RESOURCE_SOUND:
        case AE_MOVIE_RESOURCE_PARTICLE:
            {
                ae_userdata_t resource_ud = ae_get_movie_layer_data_resource_userdata( _layer );

                if( resource_ud == AE_USERDATA_NULL )
                {
                    return AE_TRUE;
                }

                Resource * resource = reinterpret_cast<Resource *>(resource_ud);

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
    bool Movie2Data::acquireComposition( const aeMovieComposition * _composition )
    {
        if( ae_visit_nodes_layer_data( _composition, &__ae_movie_layer_data_visitor_acquire, this ) == AE_FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static void __ae_movie_resource_image_release( const aeMovieResourceImage * _resource )
    {
        ae_userdata_t resource_ud = ae_get_movie_resource_userdata( (const aeMovieResource *)_resource );

        Movie2Data::ImageDesc * image_desc = reinterpret_cast<Movie2Data::ImageDesc *>(resource_ud);

        const ResourceImagePtr & resource_image = image_desc->resourceImage;

        resource_image->release();

        if( --image_desc->refcount == 0 )
        {
            image_desc->materials[EMB_NORMAL] = nullptr;
            image_desc->materials[EMB_ADD] = nullptr;
            image_desc->materials[EMB_SCREEN] = nullptr;
            image_desc->materials[EMB_MULTIPLY] = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_bool_t __ae_movie_layer_data_visitor_release( const aeMovieCompositionData * _compositionData, const aeMovieLayerData * _layer, ae_userdata_t _ud )
    {
        AE_UNUSED( _compositionData );
        AE_UNUSED( _ud );

        aeMovieResourceTypeEnum resource_type = ae_get_movie_layer_data_resource_type( _layer );

        switch( resource_type )
        {
        case AE_MOVIE_RESOURCE_SEQUENCE:
            {
                const aeMovieResource * resource = ae_get_movie_layer_data_resource( _layer );

                const aeMovieResourceSequence * resource_sequence = (const aeMovieResourceSequence *)resource;

                for( ae_uint32_t index = 0; index != resource_sequence->image_count; ++index )
                {
                    const aeMovieResourceImage * resource_image = resource_sequence->images[index];

                    __ae_movie_resource_image_release( resource_image );
                }

                return AE_TRUE;
            }break;
        case AE_MOVIE_RESOURCE_IMAGE:
            {
                const aeMovieResource * resource = ae_get_movie_layer_data_resource( _layer );

                const aeMovieResourceImage * resource_image = reinterpret_cast<const aeMovieResourceImage *>(resource);

                __ae_movie_resource_image_release( resource_image );

                return AE_TRUE;
            }break;
        case AE_MOVIE_RESOURCE_VIDEO:
        case AE_MOVIE_RESOURCE_SOUND:
        case AE_MOVIE_RESOURCE_PARTICLE:
            {
                ae_userdata_t resource_ud = ae_get_movie_layer_data_resource_userdata( _layer );

                if( resource_ud == AE_USERDATA_NULL )
                {
                    return AE_TRUE;
                }

                Resource * resource = reinterpret_cast<Resource *>(resource_ud);

                resource->release();

                return AE_TRUE;
            }break;
        default:
            break;
        }

        return AE_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Data::releaseComposition( const aeMovieComposition * _composition )
    {
        ae_visit_nodes_layer_data( _composition, &__ae_movie_layer_data_visitor_release, this );
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
    const ResourcePtr & Movie2Data::getResource( const ae_string_t _resourceName )
    {
        const ResourcePtr & resource = RESOURCE_SERVICE()
            ->getResourceReference( Helper::stringizeString( _resourceName ) );

        MENGINE_ASSERTION_MEMORY_PANIC( resource, "not found resource '%s'"
            , _resourceName
        );

        m_resources.emplace_back( resource );

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2Data::ImageDesc * Movie2Data::makeImageDesc( const ResourceImagePtr & _resource )
    {
        ImageDesc * desc = m_poolImageDesc.createT();

        desc->refcount = 0;
        desc->resourceImage = _resource;

        m_images.push_back( desc );

        return desc;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2Data::removeImageDesc( ImageDesc * _desc )
    {
        m_poolImageDesc.destroyT( _desc );
    }
}