#include "DataflowAEZ.h"

#include "Movie2Interface.h"

#include "ResourceMovie2.h"
#include "Movie2Data.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/StreamHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/MemoryCopy.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
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
            Movie2Data * data = static_cast<Movie2Data *>(_userdata);

            aeMovieResourceTypeEnum resourceType = _resource->type;

            switch( resourceType )
            {
            case AE_MOVIE_RESOURCE_IMAGE:
                {
                    const aeMovieResourceImage * movieResourceImage = reinterpret_cast<const aeMovieResourceImage *>(_resource);

                    const ResourcePtr & dataResource = data->getResource( movieResourceImage->name );

                    if( dataResource == AE_NULLPTR )
                    {
                        return AE_FALSE;
                    }

                    Movie2Data::ImageDesc * desc = data->makeImageDesc( dataResource );

                    *_resourceData = desc;

                    return AE_TRUE;
                }break;
            case AE_MOVIE_RESOURCE_VIDEO:
                {
                    const aeMovieResourceVideo * movieResourceVideo = reinterpret_cast<const aeMovieResourceVideo *>(_resource);

                    const ResourcePtr & dataResource = data->getResource( movieResourceVideo->name );

                    if( dataResource == AE_NULLPTR )
                    {
                        return AE_FALSE;
                    }

                    *_resourceData = dataResource.get();

                    return AE_TRUE;
                }break;
            case AE_MOVIE_RESOURCE_SOUND:
                {
                    const aeMovieResourceSound * movieResourceSound = reinterpret_cast<const aeMovieResourceSound *>(_resource);

                    const ResourcePtr & dataResource = data->getResource( movieResourceSound->name );

                    if( dataResource == AE_NULLPTR )
                    {
                        return AE_FALSE;
                    }

                    *_resourceData = dataResource.get();

                    return AE_TRUE;
                }break;
            case AE_MOVIE_RESOURCE_PARTICLE:
                {
                    const aeMovieResourceParticle * movieResourceParticle = reinterpret_cast<const aeMovieResourceParticle *>(_resource);

                    const ResourcePtr & dataResource = data->getResource( movieResourceParticle->name );

                    if( dataResource == AE_NULLPTR )
                    {
                        return AE_FALSE;
                    }

                    *_resourceData = dataResource.get();

                    return AE_TRUE;
                }break;
            default:
                break;
            }

            *_resourceData = AE_NULLPTR;

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t movieResourceDeleter( aeMovieResourceTypeEnum _type, ae_userdata_t _data, ae_userdata_t _userdata )
        {
            Movie2Data * data = reinterpret_cast<Movie2Data *>(_userdata);

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
        static ae_bool_t movieCacheUvAvailable( const aeMovieDataCacheUVAvailableCallbackData * _callbackData, ae_voidptr_t _userdata )
        {
            AE_UNUSED( _callbackData );
            AE_UNUSED( _userdata );

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t movieCacheUvProvider( const aeMovieDataCacheUVProviderCallbackData * _callbackData, ae_voidptrptr_t _resourceData, ae_voidptr_t _userdata )
        {
            AE_UNUSED( _userdata );

            const aeMovieResource * movieResource = _callbackData->resource;

            aeMovieResourceTypeEnum resourceType = movieResource->type;

            switch( resourceType )
            {
            case AE_MOVIE_RESOURCE_IMAGE:
                {
                    const aeMovieResourceImage * movieResourceImage = reinterpret_cast<const aeMovieResourceImage *>(movieResource);

                    if( movieResourceImage->atlas_image != AE_NULLPTR )
                    {
                        break;
                    }

                    const Movie2Data::ImageDesc * imageDesc = reinterpret_cast<const Movie2Data::ImageDesc *>(movieResource->userdata);

                    ResourceImage * resourceImage = imageDesc->resourceImage;

                    ae_uint32_t vertexCount = _callbackData->vertex_count;

                    mt::vec2f * uvs = Helper::allocateMemoryNT<mt::vec2f>( vertexCount, "movie" );

                    mt::vec2f * uvsIterator = uvs;

                    for( const ae_vector2_t * uvIterator = _callbackData->uvs, * uvEnd = _callbackData->uvs + vertexCount; uvIterator != uvEnd; ++uvIterator, ++uvsIterator )
                    {
                        mt::vec2f uv;
                        const float * uv2 = *uvIterator;
                        uv.from_f2( uv2 );

                        resourceImage->correctUV( 0, uv, uvsIterator );
                    }

                    *_resourceData = uvs;
                }break;
            default:
                {
                }break;
            }

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t movieCacheUvDeleter( const aeMovieDataCacheUVDeleterCallbackData * _callbackData, ae_userdata_t _userdata )
        {
            MENGINE_UNUSED( _userdata );

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
    DataInterfacePtr DataflowAEZ::create( const DocumentInterfacePtr & _doc )
    {
        Movie2DataPtr data = m_factoryMovieData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data, "invalid create movie data" );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DataflowAEZ::load( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc )
    {
        MemoryInterfacePtr memory = Helper::readStreamArchiveMagic( _stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_AEZ ), GET_MAGIC_VERSION( MAGIC_AEZ ), _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid read magic" );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowAEZ::flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const DataflowContext * _context, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _context );
        MENGINE_UNUSED( _doc );

        Movie2Data * data = _data.getT<Movie2Data *>();

        data->setGroupName( _context->groupName );

        aeMovieDataProviders dataProviders;
        ae_clear_movie_data_providers( &dataProviders );

        dataProviders.resource_provider = &Detail::movieResourceProvider;
        dataProviders.resource_deleter = &Detail::movieResourceDeleter;
        dataProviders.cache_uv_available = &Detail::movieCacheUvAvailable;
        dataProviders.cache_uv_provider = &Detail::movieCacheUvProvider;
        dataProviders.cache_uv_deleter = &Detail::movieCacheUvDeleter;

        aeMovieData * movieData = ae_create_movie_data( m_movieInstance, &dataProviders, data );

        aeMovieStream * movieStream = ae_create_movie_stream( m_movieInstance, &Detail::movieReadStream, &Detail::movieCopyStream, _memory.get() );

        ae_uint32_t majorVersion;
        ae_uint32_t minorVersion;
        ae_result_t resultLoadMovieData = ae_load_movie_data( movieData, movieStream, &majorVersion, &minorVersion );

        ae_delete_movie_stream( movieStream );

        if( resultLoadMovieData != AE_RESULT_SUCCESSFUL )
        {
            const ae_char_t * resultStringInfo = ae_get_movie_result_string_info( resultLoadMovieData );

            LOGGER_ERROR( "invalid load data result '%s'\ncurrent version '%u.%u'\nload version '%u.%u'"
                , resultStringInfo
                , AE_MOVIE_SDK_MAJOR_VERSION
                , AE_MOVIE_SDK_MINOR_VERSION
                , majorVersion
                , minorVersion
            );

            return false;
        }

        data->setMovieData( movieData );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
