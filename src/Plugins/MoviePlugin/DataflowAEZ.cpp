#include "DataflowAEZ.h"

#include "Interface/StringizeServiceInterface.h"

#include "Movie2Interface.h"

#include "Movie2Data.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Stream.h"
#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static ae_size_t __movie_read_stream( ae_voidptr_t _buff, ae_size_t _carriage, ae_size_t _size, ae_userdata_t _ud )
        {
            stdex::memorycopy( _buff, 0U, (ae_uint8_t *)_ud + _carriage, _size );

            return _size;
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t __movie_copy_stream( ae_constvoidptr_t _src, ae_voidptr_t _dst, ae_size_t _size, ae_userdata_t _ud )
        {
            MENGINE_UNUSED( _ud );

            stdex::memorycopy( _dst, 0U, _src, _size );
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t __movie_resource_provider( const aeMovieResource * _resource, ae_voidptrptr_t _rd, ae_voidptr_t _ud )
        {
            Movie2Data * data = (Movie2Data *)_ud;

            aeMovieResourceTypeEnum resource_type = _resource->type;

            switch( resource_type )
            {
            case AE_MOVIE_RESOURCE_IMAGE:
                {
                    const aeMovieResourceImage * resource_image = (const aeMovieResourceImage *)_resource;

                    const ResourcePtr & data_resource = data->getResource( resource_image->name );

                    if( data_resource == AE_NULLPTR )
                    {
                        return AE_FALSE;
                    }

                    Movie2DataImageDesc * desc = data->makeImageDesc( data_resource );

                    *_rd = desc;

                    return AE_TRUE;
                }break;
            case AE_MOVIE_RESOURCE_VIDEO:
                {
                    const aeMovieResourceVideo * resource_video = (const aeMovieResourceVideo *)_resource;

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
                    const aeMovieResourceSound * resource_sound = (const aeMovieResourceSound *)_resource;

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
                    const aeMovieResourceParticle * resource_particle = (const aeMovieResourceParticle *)_resource;

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
                    Movie2DataImageDesc * desc = reinterpret_cast<Movie2DataImageDesc *>(_data);

                    data->removeImageDesc( desc );
                }break;
            default:
                break;
            }
        }        
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
        m_factoryMovieData = new FactoryPool<Movie2Data, 128>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DataflowAEZ::finalize()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryMovieData );
        m_factoryMovieData = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowAEZ::isThreadFlow() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    DataInterfacePtr DataflowAEZ::create( const Char * _doc )
    {
        Movie2DataPtr data = m_factoryMovieData->createObject( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( data, nullptr );

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DataflowAEZ::load( const InputStreamInterfacePtr & _stream, const Char * _doc )
    {
        MemoryInterfacePtr memory = Helper::loadStreamArchiveData( _stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_AEZ ), GET_MAGIC_VERSION( MAGIC_AEZ ), _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DataflowAEZ::flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const Char * _doc )
    {
        MENGINE_UNUSED( _doc );

        if( strcmp( _doc, "Movies2/StartMenu/StartMenu.aez" ) == 0 )
        {
            int i = 0;
            (void)i;
        }

        Movie2Data * data = stdex::intrusive_get<Movie2Data *>( _data );

        void * memory_buffer = _memory->getBuffer();

        aeMovieDataProviders data_providers;
        ae_clear_movie_data_providers( &data_providers );

        data_providers.resource_provider = &Detail::__movie_resource_provider;
        data_providers.resource_deleter = &Detail::__movie_resource_deleter;

        aeMovieData * movieData = ae_create_movie_data( m_movieInstance, &data_providers, data );

        aeMovieStream * movie_stream = ae_create_movie_stream( m_movieInstance, &Detail::__movie_read_stream, &Detail::__movie_copy_stream, memory_buffer );

        ae_uint32_t major_version;
        ae_uint32_t minor_version;
        ae_result_t result_load_movie_data = ae_load_movie_data( movieData, movie_stream, &major_version, &minor_version );

        if( result_load_movie_data != AE_RESULT_SUCCESSFUL )
        {
            const ae_char_t * result_string_info = ae_get_result_string_info( result_load_movie_data );

            LOGGER_ERROR( "invalid load data result '%s'\ncurrent version '%u.%u'\nload version '%u.%u'"
                , result_string_info
                , AE_MOVIE_SDK_MAJOR_VERSION
                , AE_MOVIE_SDK_MINOR_VERSION
                , major_version
                , minor_version
            );

            return false;
        }

        ae_delete_movie_stream( movie_stream );

        data->setMovieData( movieData );

        return true;
    }
}