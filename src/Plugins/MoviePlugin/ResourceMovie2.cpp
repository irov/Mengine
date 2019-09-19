#include "ResourceMovie2.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Movie2Interface.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/Dataflow.h"
#include "Kernel/Stream.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

#include "stdex/memorycopy.h"

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
        if( m_data == nullptr )
        {
            LOGGER_ERROR( "resource '%s' not compile (composition '%s' layer '%s')"
                , this->getName().c_str()
                , _compositionName.c_str()
                , _layerName.c_str()
            );

            return false;
        }

        const aeMovieData * movieData = m_data->getMovieData();

        if( movieData == nullptr )
        {
            LOGGER_ERROR( "resource '%s' not data (composition '%s' layer '%s')"
                , this->getName().c_str()
                , _compositionName.c_str()
                , _layerName.c_str()
            );

            return false;
        }

        const aeMovieCompositionData * compositionData = ae_get_movie_composition_data( movieData, _compositionName.c_str() );

        if( compositionData == nullptr )
        {
            LOGGER_ERROR( "resource '%s' not composition data (composition '%s' layer '%s')"
                , this->getName().c_str()
                , _compositionName.c_str()
                , _layerName.c_str()
            );

            return false;
        }

        const aeMovieInstance * instance = ae_get_movie_data_instance( movieData );

        if( ae_has_movie_composition_data_layer( instance, compositionData, _layerName.c_str() ) == AE_FALSE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceMovie2::getCompositionDuration( const ConstString & _compositionName ) const
    {
        MapCompositions::const_iterator it_found = m_compositions.find( _compositionName );

        if( it_found == m_compositions.end() )
        {
            return 0.f;
        }

        const ResourceMovie2CompositionDesc & composition = it_found->second;

        return AE_TIME_MILLISECOND( composition.duration );
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceMovie2::getCompositionFrameDuration( const ConstString & _compositionName ) const
    {
        MapCompositions::const_iterator it_found = m_compositions.find( _compositionName );

        if( it_found == m_compositions.end() )
        {
            return 0.f;
        }

        const ResourceMovie2CompositionDesc & composition = it_found->second;

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
            return nullptr;
        }

        const aeMovieData * movieData = m_data->getMovieData();

        const aeMovieCompositionData * compositionData = ae_get_movie_composition_data( movieData, _compositionName.c_str() );

        if( compositionData == nullptr )
        {
            LOGGER_ERROR( "resource '%s' file '%s' not found composition data '%s'"
                , this->getName().c_str()
                , this->getFilePath().c_str()
                , _compositionName.c_str()
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
    const ResourceMovie2CompositionDesc * ResourceMovie2::getCompositionDesc( const ConstString & _compositionName ) const
    {
        MapCompositions::const_iterator it_found = m_compositions.find( _compositionName );

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
    bool ResourceMovie2::_compile()
    {
        Content * content = this->getContent();

        const FilePath & filePath = content->getFilePath();

        if( filePath.empty() == true )
        {
            LOGGER_ERROR( "resource movie '%s' group '%s' don`t set file path"
                , this->getName().c_str()
                , this->getGroupName().c_str()
            );

            return false;
        }

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();

        const ConstString & dataflowType = content->getDataflowType();

        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), dataflowType );

        DataInterfacePtr data = Helper::getDataflow( fileGroup, filePath, dataflow, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( data, false, "resource movie '%s' group '%s' invalid compile data"
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
        if( m_data != nullptr )
        {
            m_data->release();
            m_data = nullptr;
        }
    }
}