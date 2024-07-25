#include "ResourceMovie2.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"

#include "Movie2Interface.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/Dataflow.h"
#include "Kernel/Stream.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ContentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceMovie2::ResourceMovie2()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceMovie2::~ResourceMovie2()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie2::hasComposition( const ConstString & _compositionName ) const
    {
        MapCompositions::const_iterator it_found = m_compositions.find( _compositionName );

        if( it_found == m_compositions.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie2::hasCompositionLayer( const ConstString & _compositionName, const ConstString & _layerName ) const
    {
        MapCompositions::const_iterator it_found = m_compositions.find( _compositionName );

        if( it_found == m_compositions.end() )
        {
            return false;
        }

        const CompositionDesc & composition = it_found->second;

        for( const CompositionLayer & layer : composition.layers )
        {
            if( layer.name != _layerName )
            {
                continue;
            }

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceMovie2::getCompositionDuration( const ConstString & _compositionName ) const
    {
        MapCompositions::const_iterator it_found = m_compositions.find( _compositionName );

        if( it_found == m_compositions.end() )
        {
            return 0.f;
        }

        const CompositionDesc & composition = it_found->second;

        return composition.duration;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceMovie2::getCompositionFrameDuration( const ConstString & _compositionName ) const
    {
        MapCompositions::const_iterator it_found = m_compositions.find( _compositionName );

        if( it_found == m_compositions.end() )
        {
            return 0.f;
        }

        const CompositionDesc & composition = it_found->second;

        return composition.frameDuration;
    }
    //////////////////////////////////////////////////////////////////////////
    const Movie2DataInterfacePtr & ResourceMovie2::getData() const
    {
        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    const aeMovieCompositionData * ResourceMovie2::getCompositionData( const ConstString & _compositionName ) const
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "resource movie2 '%s' file '%s' not compile"
                , this->getName().c_str()
                , Helper::getContentFullPath( this->getContent() )
            );

            return nullptr;
        }

        const aeMovieData * movieData = m_data->getMovieData();

        const aeMovieCompositionData * compositionData = ae_get_movie_composition_data( movieData, _compositionName.c_str() );

        if( compositionData == nullptr )
        {
            LOGGER_ERROR( "resource movie2 '%s' file '%s' not found composition data '%s'"
                , this->getName().c_str()
                , Helper::getContentFullPath( this->getContent() )
                , _compositionName.c_str()
            );

            return nullptr;
        }

        return compositionData;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie2::setCompositionDesc( const ConstString & _name, const CompositionDesc & _composition )
    {
        m_compositions[_name] = _composition;
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceMovie2::CompositionDesc * ResourceMovie2::getCompositionDesc( const ConstString & _compositionName ) const
    {
        MapCompositions::const_iterator it_found = m_compositions.find( _compositionName );

        if( it_found == m_compositions.end() )
        {
            return nullptr;
        }

        const CompositionDesc & composition = it_found->second;

        return &composition;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie2::foreachCompositionLayers( const ConstString & _compositionName, const LambdaCompositionLayers & _lambda ) const
    {
        MapCompositions::const_iterator it_found = m_compositions.find( _compositionName );

        if( it_found == m_compositions.end() )
        {
            LOGGER_ERROR( "resource movie2 '%s' path '%s' invalid get composition name '%s'"
                , this->getName().c_str()
                , Helper::getContentFullPath( this->getContent() )
                , _compositionName.c_str()
            );

            return false;
        }

        const CompositionDesc & compositionDesc = it_found->second;

        const VectorCompositionLayers & layers = compositionDesc.layers;

        for( const CompositionLayer & layer : layers )
        {
            _lambda( layer );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie2::foreachCompositionsDesc( const LambdaCompositionDescs & _lambda ) const
    {
        for( const MapCompositions::value_type & value : m_compositions )
        {
            const ConstString & name = value.first;
            const CompositionDesc & desc = value.second;

            _lambda( name, desc );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie2::_compile()
    {
        const ContentInterfacePtr & content = this->getContent();

        const FilePath & filePath = content->getFilePath();

        if( filePath.empty() == true )
        {
            LOGGER_ERROR( "resource movie2 '%s' group '%s' don`t set file path"
                , this->getName().c_str()
                , this->getGroupName().c_str()
            );

            return false;
        }

        DataflowContext context;
        context.filePath = filePath;
        context.groupName = this->getGroupName();

        Movie2DataInterfacePtr data = Helper::getDataflow( content, &context, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( data, "resource '%s' group '%s' invalid compile data"
            , this->getName().c_str()
            , this->getGroupName().c_str()
        );

        if( data->acquire() == false )
        {
            return false;
        }

        m_data = data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie2::_release()
    {
        m_data->release();
        m_data = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}