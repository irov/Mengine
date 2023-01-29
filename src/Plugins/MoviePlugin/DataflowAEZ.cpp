#include "DataflowAEZ.h"

#include "Movie2Interface.h"

#include "ResourceMovie2.h"
#include "Movie2Data.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Stream.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/MemoryCopy.h"

#include "Config/StdString.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static ae_size_t __movie_read_stream( ae_voidptr_t _buff, ae_size_t _carriage, ae_size_t _size, ae_userdata_t _ud )
        {
            Helper::memoryCopy( _buff, 0U, (ae_uint8_t *)_ud + _carriage, _size );

            return _size;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t __movie_copy_stream( ae_constvoidptr_t _src, ae_voidptr_t _dst, ae_size_t _size, ae_userdata_t _ud )
        {
            MENGINE_UNUSED( _ud );

            Helper::memoryCopy( _dst, 0U, _src, _size );
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t __movie_resource_provider( const aeMovieResource * _resource, ae_voidptrptr_t _rd, ae_voidptr_t _ud )
        {
            Movie2Data * data = static_cast<Movie2Data *>(_ud);

            aeMovieResourceTypeEnum resource_type = _resource->type;

            switch( resource_type )
            {
            case AE_MOVIE_RESOURCE_IMAGE:
                {
                    const aeMovieResourceImage * resource_image = reinterpret_cast<const aeMovieResourceImage *>(_resource);

                    const ResourcePtr & data_resource = data->getResource( resource_image->name );

                    if( data_resource == AE_NULLPTR )
                    {
                        return AE_FALSE;
                    }

                    Movie2Data::ImageDesc * desc = data->makeImageDesc( data_resource );

                    *_rd = desc;

                    return AE_TRUE;
                }break;
            case AE_MOVIE_RESOURCE_VIDEO:
                {
                    const aeMovieResourceVideo * resource_video = reinterpret_cast<const aeMovieResourceVideo *>(_resource);

                    const ResourcePtr & data_resource = data->getResource( resource_video->name );

                    if( data_resource == AE_NULLPTR )
                    {
                        return AE_FALSE;
                    }

                    *_rd = data_resource.get();

                    return AE_TRUE;
                }break;
            case AE_MOVIE_RESOURCE_SOUND:
                {
                    const aeMovieResourceSound * resource_sound = reinterpret_cast<const aeMovieResourceSound *>(_resource);

                    const ResourcePtr & data_resource = data->getResource( resource_sound->name );

                    if( data_resource == AE_NULLPTR )
                    {
                        return AE_FALSE;
                    }

                    *_rd = data_resource.get();

                    return AE_TRUE;
                }break;
            case AE_MOVIE_RESOURCE_PARTICLE:
                {
                    const aeMovieResourceParticle * resource_particle = reinterpret_cast<const aeMovieResourceParticle *>(_resource);

                    const ResourcePtr & data_resource = data->getResource( resource_particle->name );

                    if( data_resource == AE_NULLPTR )
                    {
                        return AE_FALSE;
                    }

                    *_rd = data_resource.get();

                    return AE_TRUE;
                }break;
            default:
                break;
            }

            *_rd = AE_NULLPTR;

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t __movie_resource_deleter( aeMovieResourceTypeEnum _type, ae_userdata_t _data, ae_userdata_t _ud )
        {
            Movie2Data * data = reinterpret_cast<Movie2Data *>(_ud);

            switch( _type )
            {
            case AE_MOVIE_RESOURCE_IMAGE:
                {
                    Movie2Data::ImageDesc * desc = reinterpret_cast<Movie2Data::ImageDesc *>(_data);

                    data->removeImageDesc( desc );
                }break;
            default:
                break;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t __movie_cache_uv_available( const aeMovieDataCacheUVAvailableCallbackData * _callbackData, ae_voidptr_t _ud )
        {
            AE_UNUSED( _callbackData );
            AE_UNUSED( _ud );

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t __movie_cache_uv_provider( const aeMovieDataCacheUVProviderCallbackData * _callbackData, ae_voidptrptr_t _rd, ae_voidptr_t _ud )
        {
            AE_UNUSED( _ud );

            const aeMovieResource * movie_resource = _callbackData->resource;

            aeMovieResourceTypeEnum resource_type = movie_resource->type;

            switch( resource_type )
            {
            case AE_MOVIE_RESOURCE_IMAGE:
                {
                    const aeMovieResourceImage * movie_resource_image = reinterpret_cast<const aeMovieResourceImage *>(movie_resource);

                    if( movie_resource_image->atlas_image != AE_NULLPTR )
                    {
                        break;
                    }

                    const Movie2Data::ImageDesc * image_desc = reinterpret_cast<const Movie2Data::ImageDesc *>(movie_resource->userdata);

                    ResourceImage * resourceImage = image_desc->resourceImage;

                    ae_uint32_t vertex_count = _callbackData->vertex_count;

                    mt::vec2f * uvs = Helper::allocateMemoryNT<mt::vec2f>( vertex_count, "movie" );

                    mt::vec2f * uvs_iterator = uvs;

                    for( const ae_vector2_t
                        * it = _callbackData->uvs,
                        *it_end = _callbackData->uvs + vertex_count;
                        it != it_end;
                        ++it, ++uvs_iterator )
                    {
                        mt::vec2f uv;
                        const float * uv2 = *it;
                        uv.from_f2( uv2 );

                        resourceImage->correctUVImage( uv, uvs_iterator );
                    }

                    *_rd = uvs;
                }break;
            default:
                {
                }break;
            }

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t __movie_cache_uv_deleter( const aeMovieDataCacheUVDeleterCallbackData * _callbackData, ae_userdata_t _ud )
        {
            MENGINE_UNUSED( _ud );

            if( _callbackData->uv_cache_userdata == AE_NULLPTR )
            {
                return;
            }

            Helper::deallocateMemory( _callbackData->uv_cache_userdata, "movie" );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    DataflowAEZ::DataflowAEZ()
        : m_movieInstance( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DataflowAEZ::~DataflowAEZ()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowAEZ::setMovieInstance( const aeMovieInstance * _movieInstance )
    {
        m_movieInstance = _movieInstance;
    }
    //////////////////////////////////////////////////////////////////////////
    const aeMovieInstance * DataflowAEZ::getMovieInstance() const
    {
        return m_movieInstance;
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowAEZ::setArchivator( const ArchivatorInterfacePtr & _archivator )
    {
        m_archivator = _archivator;
    }
    //////////////////////////////////////////////////////////////////////////
    const ArchivatorInterfacePtr & DataflowAEZ::getArchivator() const
    {
        return m_archivator;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowAEZ::initialize()
    {
        m_factoryMovieData = Helper::makeFactoryPool<Movie2Data, 128>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowAEZ::finalize()
    {
        m_archivator = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMovieData );
        m_factoryMovieData = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowAEZ::isThreadFlow() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    DataInterfacePtr DataflowAEZ::create( const DocumentPtr & _doc )
    {
        Movie2DataPtr data = m_factoryMovieData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DataflowAEZ::load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc )
    {
        MemoryInterfacePtr memory = Helper::loadStreamArchiveData( _stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_AEZ ), GET_MAGIC_VERSION( MAGIC_AEZ ), _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowAEZ::flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const DataflowContext * _context, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _doc );

        Movie2Data * data = _data.getT<Movie2Data *>();

        data->setGroupName( _context->groupName );

        void * memory_buffer = _memory->getBuffer();

        aeMovieDataProviders data_providers;
        ae_clear_movie_data_providers( &data_providers );

        data_providers.resource_provider = &Detail::__movie_resource_provider;
        data_providers.resource_deleter = &Detail::__movie_resource_deleter;
        data_providers.cache_uv_available = &Detail::__movie_cache_uv_available;
        data_providers.cache_uv_provider = &Detail::__movie_cache_uv_provider;
        data_providers.cache_uv_deleter = &Detail::__movie_cache_uv_deleter;

        aeMovieData * movieData = ae_create_movie_data( m_movieInstance, &data_providers, data );

        aeMovieStream * movie_stream = ae_create_movie_stream( m_movieInstance, &Detail::__movie_read_stream, &Detail::__movie_copy_stream, memory_buffer );

        ae_uint32_t major_version;
        ae_uint32_t minor_version;
        ae_result_t result_load_movie_data = ae_load_movie_data( movieData, movie_stream, &major_version, &minor_version );

        ae_delete_movie_stream( movie_stream );

        if( result_load_movie_data != AE_RESULT_SUCCESSFUL )
        {
            const ae_char_t * result_string_info = ae_get_movie_result_string_info( result_load_movie_data );

            LOGGER_ERROR( "invalid load data result '%s'\ncurrent version '%u.%u'\nload version '%u.%u'"
                , result_string_info
                , AE_MOVIE_SDK_MAJOR_VERSION
                , AE_MOVIE_SDK_MINOR_VERSION
                , major_version
                , minor_version
            );

            return false;
        }

        data->setMovieData( movieData );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}