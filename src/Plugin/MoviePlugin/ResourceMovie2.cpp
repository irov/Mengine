#include "ResourceMovie2.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/OptionsInterface.h"
#include "Interface/FileSystemInterface.h"

#include "Plugin/ResourceValidatePlugin/ResourceValidateInterface.h"

#include "Movie2Interface.h"

#include "Engine/ResourceImageDefault.h"
#include "Engine/ResourceSound.h"

#include "Metacode/Metacode.h"

#include "Kernel/Stream.h"
#include "Kernel/Logger.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static ae_size_t __movie_read_stream( ae_voidptr_t _data, ae_voidptr_t _buff, ae_size_t _carriage, ae_size_t _size )
    {
        stdex::memorycopy( _buff, 0U, (ae_uint8_t *)_data + _carriage, _size );

        return _size;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_copy_stream( ae_voidptr_t _data, ae_constvoidptr_t _src, ae_voidptr_t _dst, ae_size_t _size )
    {
        (void)_data;

        stdex::memorycopy( _dst, 0U, _src, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_bool_t __movie_resource_provider( const aeMovieResource * _resource, ae_voidptrptr_t _rd, ae_voidptr_t _ud )
    {
        ResourceMovie2 * resourceMovie2 = (ResourceMovie2 *)_ud;

        aeMovieResourceTypeEnum resource_type = _resource->type;

        switch( resource_type )
        {
        case AE_MOVIE_RESOURCE_IMAGE:
            {
                const aeMovieResourceImage * resource_image = (const aeMovieResourceImage *)_resource;

                Resource * data_resource = resourceMovie2->getResource_( resource_image->name );

                if( data_resource == AE_NULL )
                {
                    return AE_FALSE;
                }

                *_rd = data_resource;

                return AE_TRUE;
            }break;
        case AE_MOVIE_RESOURCE_VIDEO:
            {
                const aeMovieResourceVideo * resource_video = (const aeMovieResourceVideo *)_resource;

                Resource * data_resource = resourceMovie2->getResource_( resource_video->name );
                
                if( data_resource == AE_NULL )
                {
                    return AE_FALSE;
                }

                *_rd = data_resource;

                return AE_TRUE;
            }break;
        case AE_MOVIE_RESOURCE_SOUND:
            {
                const aeMovieResourceSound * resource_sound = (const aeMovieResourceSound *)_resource;

                Resource * data_resource = resourceMovie2->getResource_( resource_sound->name );

                if( data_resource == AE_NULL )
                {
                    return AE_FALSE;
                }

                *_rd = data_resource;

                return AE_TRUE;
            }break;
        case AE_MOVIE_RESOURCE_PARTICLE:
            {
                const aeMovieResourceParticle * resource_particle = (const aeMovieResourceParticle *)_resource;

                Resource * data_resource = resourceMovie2->getResource_( resource_particle->name );

                if( data_resource == AE_NULL )
                {
                    return AE_FALSE;
                }

                *_rd = data_resource;

                return AE_TRUE;
            }break;
        default:
            break;
        }

        *_rd = AE_NULL;

        return AE_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_resource_deleter( aeMovieResourceTypeEnum _type, ae_voidptr_t _data, ae_voidptr_t _ud )
    {
        (void)_type;
        (void)_data;
        (void)_ud;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_bool_t __movie_cache_uv_provider( const aeMovieDataCacheUVProviderCallbackData * _callbackData, ae_voidptrptr_t _cud, ae_voidptr_t _ud )
    {
        (void)_ud;

        const aeMovieResource * movie_resource = _callbackData->resource;

        Resource * resource = reinterpret_cast<Resource *>(movie_resource->data);

        aeMovieResourceTypeEnum resource_type = movie_resource->type;

        switch( resource_type )
        {
        case AE_MOVIE_RESOURCE_IMAGE:
            {
                const aeMovieResourceImage * movie_resource_image = reinterpret_cast<const aeMovieResourceImage *>(movie_resource);

                if( movie_resource_image->atlas_image != AE_NULL )
                {
                    break;
                }

                ResourceImage * resource_image = static_cast<ResourceImage *>(resource);

                ae_uint32_t vertex_count = _callbackData->vertex_count;

                mt::vec2f * uvs = Helper::allocateArrayT<mt::vec2f>( vertex_count );

                for( uint32_t index = 0; index != vertex_count; ++index )
                {
                    mt::vec2f uv;
                    const float * uv2 = _callbackData->uvs[index];
                    uv.from_f2( uv2 );

                    resource_image->correctUVImage( uvs[index], uv );
                }

                *_cud = uvs;
            }break;
        default:
            {
            }break;
        }

        return AE_TRUE;
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_void_t __movie_cache_uv_deleter( const aeMovieDataCacheUVDeleterCallbackData * _callbackData, ae_voidptr_t _ud )
    {
        (void)_ud;

        if( _callbackData->uv_cache_data == AE_NULL )
        {
            return;
        }

        Helper::freeArrayT( reinterpret_cast<mt::vec2f *>(_callbackData->uv_cache_data) );
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceMovie2::ResourceMovie2()
        : m_movieInstance( nullptr )
        , m_movieData( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceMovie2::~ResourceMovie2()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie2::setFilePath( const FilePath & _filePath )
    {
        m_filePath = _filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & ResourceMovie2::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie2::hasComposition( const ConstString & _name ) const
    {
        MapCompositions::const_iterator it_found = m_compositions.find( _name );

        if( it_found == m_compositions.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceMovie2::getCompositionDuration( const ConstString & _name ) const
    {
        MapCompositions::const_iterator it_found = m_compositions.find( _name );

        if( it_found == m_compositions.end() )
        {
            return 0.f;
        }

        const ResourceMovie2CompositionDesc & composition = it_found->second;

        return composition.duration * 1000.f;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceMovie2::getCompositionFrameDuration( const ConstString & _name ) const
    {
        MapCompositions::const_iterator it_found = m_compositions.find( _name );

        if( it_found == m_compositions.end() )
        {
            return 0.f;
        }

        const ResourceMovie2CompositionDesc & composition = it_found->second;

        return composition.frameDuration;
    }
    //////////////////////////////////////////////////////////////////////////
    const aeMovieData * ResourceMovie2::getMovieData() const
    {
        return m_movieData;
    }
    //////////////////////////////////////////////////////////////////////////
    const aeMovieCompositionData * ResourceMovie2::getCompositionData( const ConstString & _name ) const
    {
        if( this->isCompile() == false )
        {
            return nullptr;
        }

        const aeMovieCompositionData * compositionData = ae_get_movie_composition_data( m_movieData, _name.c_str() );

        if( compositionData == nullptr )
        {
            LOGGER_ERROR( "ResourceMovie2::getCompositionData resource '%s' file '%s' not found composition data '%s'"
                , this->getName().c_str()
                , this->getFilePath().c_str()
                , _name.c_str()
            );

            return nullptr;
        }

        return compositionData;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie2::setCompositionDesc( const ConstString & _name, const ResourceMovie2CompositionDesc & _composition )
    {
        m_compositions[_name] = _composition;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceMovie2CompositionDesc * ResourceMovie2::getCompositionDesc( const ConstString & _name ) const
    {
        MapCompositions::const_iterator it_found = m_compositions.find( _name );

        if( it_found == m_compositions.end() )
        {
            return nullptr;
        }

        const ResourceMovie2CompositionDesc & composition = it_found->second;

        return &composition;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie2::foreachCompositionDesc( const LambdaCompositionDescs & _lambda )
    {
        for( const MapCompositions::value_type & value : m_compositions )
        {
            const ConstString & name = value.first;
            const ResourceMovie2CompositionDesc & desc = value.second;

            _lambda( name, desc );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie2::setMovieInstance( const aeMovieInstance * _movieInstance )
    {
        m_movieInstance = _movieInstance;
    }
    //////////////////////////////////////////////////////////////////////////
    const aeMovieInstance * ResourceMovie2::getMovieInstance() const
    {
        return m_movieInstance;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie2::setMovieArchivator( const ArchivatorInterfacePtr & _movieArchivator )
    {
        m_movieArchivator = _movieArchivator;
    }
    //////////////////////////////////////////////////////////////////////////
    const ArchivatorInterfacePtr & ResourceMovie2::getMovieArchivator() const
    {
        return m_movieArchivator;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie2::_compile()
    {
        if( Resource::_compile() == false )
        {
            return false;
        }

        if( m_filePath.empty() == true )
        {
            LOGGER_ERROR( "ResourceMovie::_compile: '%s' group '%s' don`t have Key Frames Pack Path"
                , this->getName().c_str()
                , this->getGroupName().c_str()
            );

            return false;
        }

        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( fileGroup, m_filePath, false );
        
        if( stream == nullptr )
        {
            LOGGER_ERROR( "ResourceMovie2::_compile: '%s' group '%s' can`t open file '%s'"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        MemoryInterfacePtr memory = Helper::loadStreamArchiveData( stream, m_movieArchivator, GET_MAGIC_NUMBER( MAGIC_AEZ ), GET_MAGIC_VERSION( MAGIC_AEZ ), "ResourceMovie2::_compile", __FILE__, __LINE__ );

        if( memory == nullptr )
        {
            return false;
        }

        void * memory_buffer = memory->getBuffer();

        aeMovieDataProviders data_providers;
        ae_clear_movie_data_providers( &data_providers );

        data_providers.resource_provider = &__movie_resource_provider;
        data_providers.resource_deleter = &__movie_resource_deleter;
        data_providers.cache_uv_provider = &__movie_cache_uv_provider;
        data_providers.cache_uv_deleter = &__movie_cache_uv_deleter;

        aeMovieData * movieData = ae_create_movie_data( m_movieInstance, &data_providers, this );

        aeMovieStream * movie_stream = ae_create_movie_stream( m_movieInstance, &__movie_read_stream, &__movie_copy_stream, memory_buffer );

        ae_uint32_t major_version;
        ae_uint32_t minor_version;
        ae_result_t result_load_movie_data = ae_load_movie_data( movieData, movie_stream, &major_version, &minor_version );

        if( result_load_movie_data != AE_RESULT_SUCCESSFUL )
        {
            const ae_char_t * result_string_info = ae_get_result_string_info( result_load_movie_data );

            LOGGER_ERROR( "ResourceMovie2::_compile: '%s' group '%s' invalid load data from file '%s' result '%s'\ncurrent version '%u.%u'\nload version '%u.%u'"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , this->getFilePath().c_str()
                , result_string_info
                , AE_MOVIE_SDK_MAJOR_VERSION
                , AE_MOVIE_SDK_MINOR_VERSION
                , major_version
                , minor_version
            );

            return 0;
        }

        ae_delete_movie_stream( movie_stream );

        m_movieData = movieData;
        stream = nullptr;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie2::_release()
    {
        for( const ResourcePtr & resource : m_resources )
        {
            resource->decrementReference();
        }

        m_resources.clear();

        ae_delete_movie_data( m_movieData );
        m_movieData = nullptr;

        Resource::_release();
    }    
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie2::storeResource_( const ResourcePtr & _resource )
    {
        m_resources.emplace_back( _resource );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    Resource * ResourceMovie2::getResource_( const ae_string_t _name )
    {
        const ResourcePtr & resource = RESOURCE_SERVICE()
            ->getResource( Helper::stringizeString( _name ) );

        if( resource == nullptr )
        {
            LOGGER_ERROR( "ResourceMovie2::storeResource_ resource nullptr name '%s' group '%s' path '%s'"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , this->getFilePath().c_str()
            );

            return nullptr;
        }

        if( this->storeResource_( resource ) == false )
        {
            return nullptr;
        }

        return resource.get();
    }
}