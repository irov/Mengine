#include "ResourceMovie2.h"

#include "Interface/ResourceInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/OptionsInterface.h"
#include "Interface/FileSystemInterface.h"

#include "Engine/ResourceImageDefault.h"
#include "Engine/ResourceVideo.h"
#include "Engine/ResourceSound.h"

#include "Metacode/Metacode.h"

#include "Kernel/Logger.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static ae_size_t __movie_read_stream( ae_voidptr_t _data, ae_voidptr_t _buff, ae_size_t _carriage, ae_size_t _size )
    {
        (void)_carriage;

        InputStreamInterface * stream = (InputStreamInterface *)_data;

        size_t bytes = stream->read( _buff, _size );

        return bytes;
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

        Helper::freeMemoryT( reinterpret_cast<mt::vec2f *>(const_cast<void *>(_callbackData->uv_cache_data)) );
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

        const ResourceMovie2Composition & composition = it_found->second;

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

        const ResourceMovie2Composition & composition = it_found->second;

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
    const ResourceMovie2Composition * ResourceMovie2::getCompositionDesc( const ConstString & _name ) const
    {
        MapCompositions::const_iterator it_found = m_compositions.find( _name );

        if( it_found == m_compositions.end() )
        {
            return nullptr;
        }

        const ResourceMovie2Composition & composition = it_found->second;

        return &composition;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie2::setMovieInstance( const aeMovieInstance * _instance )
    {
        m_movieInstance = _instance;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie2::_loader( const Metabuf::Metadata * _meta )
    {
        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2 * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2 *>(_meta);

        m_filePath = metadata->get_File_Path();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2::VectorMeta_Composition & includes_composition = metadata->get_Includes_Composition();

        for( const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition & meta_composition : includes_composition )
        {
            const ConstString & compositionName = meta_composition.get_Name();

            ResourceMovie2Composition & composition = m_compositions[compositionName];

            composition.duration = meta_composition.get_Duration();
            composition.frameDuration = meta_composition.get_FrameDuration();

            const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::VectorMeta_Layer & includes_layer = meta_composition.get_Includes_Layer();

            for( const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_Layer & meta_layer : includes_layer )
            {
                uint32_t layerIndex = meta_layer.get_Index();
                const ConstString & layerName = meta_layer.get_Name();
                const ConstString & layerType = meta_layer.get_Type();
                const mt::mat4f & layerMatrix = meta_layer.get_Matrix();
                const ColourValue & layerColor = meta_layer.get_Color();

                ResourceMovie2CompositionLayer layer;
                layer.index = layerIndex;
                layer.name = layerName;
                layer.type = layerType;
                layer.matrix = layerMatrix;
                layer.color = layerColor;

                composition.layers.emplace_back( layer );
            }

            const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::VectorMeta_SubComposition & includes_subcomposition = meta_composition.get_Includes_SubComposition();

            for( const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceMovie2::Meta_Composition::Meta_SubComposition & meta_subcomposition : includes_subcomposition )
            {
                uint32_t subcompositionIndex = meta_subcomposition.get_Index();
                const ConstString & subcompositionName = meta_subcomposition.get_Name();

                ResourceMovie2CompositionSubComposition subcomposition;
                subcomposition.index = subcompositionIndex;
                subcomposition.name = subcompositionName;

                composition.subcompositions.emplace_back( subcomposition );
            }
        }

        return true;
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

        aeMovieDataProviders data_providers;
        ae_clear_movie_data_providers( &data_providers );

        data_providers.resource_provider = &__movie_resource_provider;
        data_providers.resource_deleter = &__movie_resource_deleter;
        data_providers.cache_uv_provider = &__movie_cache_uv_provider;
        data_providers.cache_uv_deleter = &__movie_cache_uv_deleter;

        aeMovieData * movieData = ae_create_movie_data( m_movieInstance, &data_providers, this );

        aeMovieStream * movie_stream = ae_create_movie_stream( m_movieInstance, &__movie_read_stream, &__movie_copy_stream, stream.get() );

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

#ifndef MENGINE_MASTER_RELEASE
        bool developmentMode = HAS_OPTION( "dev" );
        bool noresourceCheck = HAS_OPTION( "noresourcecheck" );

        if( developmentMode == true && noresourceCheck == false )
        {
            for( const ResourcePtr & resource : m_resources )
            {
                if( resource->isValid() == false )
                {
                    LOGGER_ERROR( "ResourceMovie2::_compile resource name '%s' group '%s' path '%s' invalid store resource '%s' type '%s'"
                        , this->getName().c_str()
                        , this->getGroupName().c_str()
                        , this->getFilePath().c_str()
                        , resource->getName().c_str()
                        , resource->getType().c_str()
                    );

                    return false;
                }
            }
        }
#endif

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
    static ae_bool_t Mengine_movie_layer_data_visitor( const aeMovieCompositionData * _compositionData, const aeMovieLayerData * _layerData, ae_voidptr_t _ud )
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
                    LOGGER_ERROR( "Mengine_movie_layer_data_visitor '%s' composition '%s' text layer '%s' opacity transparent"
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
    bool ResourceMovie2::_isValid() const
    {
        if( m_filePath.empty() == true )
        {
            LOGGER_ERROR( "ResourceMovie::_isValid: '%s' group '%s' don`t have Key Frames Pack Path"
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
            LOGGER_ERROR( "ResourceMovie2::_isValid: '%s' group '%s' can`t open file '%s'"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , this->getFilePath().c_str()
            );

            return false;
        }

        aeMovieDataProviders data_providers;
        ae_clear_movie_data_providers( &data_providers );

        aeMovieData * movieData = ae_create_movie_data( m_movieInstance, &data_providers, AE_NULL );

        aeMovieStream * movieStream = ae_create_movie_stream( m_movieInstance, &__movie_read_stream, &__movie_copy_stream, stream.get() );

        ae_uint32_t major_version;
        ae_uint32_t minor_version;
        ae_result_t result_load_movie_data = ae_load_movie_data( movieData, movieStream, &major_version, &minor_version );

        if( result_load_movie_data != AE_RESULT_SUCCESSFUL )
        {
            const ae_char_t * result_string_info = ae_get_result_string_info( result_load_movie_data );

            LOGGER_ERROR( "ResourceMovie2::_isValid: '%s' group '%s' file '%s' check movie data invalid '%s'\ncurrent version '%u.%u'\nload version '%u.%u'"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , this->getFilePath().c_str()
                , result_string_info
                , AE_MOVIE_SDK_MAJOR_VERSION
                , AE_MOVIE_SDK_MINOR_VERSION
                , major_version
                , minor_version
            );

            return false;
        }

        ae_visit_movie_layer_data( movieData, &Mengine_movie_layer_data_visitor, (ae_voidptr_t)this );

        ae_delete_movie_data( movieData );
        ae_delete_movie_stream( movieStream );

        return true;
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
        ResourcePtr resource = RESOURCE_SERVICE()
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