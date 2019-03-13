#include "ResourceMovie2.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/OptionsInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/DataServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Movie2Interface.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/Dataflow.h"
#include "Kernel/Stream.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

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

        return AE_TIME_MILLISECOND( composition.duration );
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
    const Movie2DataInterfacePtr & ResourceMovie2::getMovieData() const
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

        const aeMovieData * movieData = m_movieData->getMovieData();

        const aeMovieCompositionData * compositionData = ae_get_movie_composition_data( movieData, _name.c_str() );

        if( compositionData == nullptr )
        {
            LOGGER_ERROR( "resource '%s' file '%s' not found composition data '%s'"
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
    bool ResourceMovie2::_compile()
    {
        const FilePath & filePath = this->getFilePath();

        if( filePath.empty() == true )
        {
            LOGGER_ERROR( "resource movie '%s' group '%s' don`t set file path"
                , this->getName().c_str()
                , this->getGroupName().c_str()
            );

            return false;
        }

        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        const ConstString & dataflowType = this->getDataflowType();

        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), dataflowType );

        DataInterfacePtr data = Helper::getDataflow( fileGroup, filePath, dataflow, MENGINE_DOCUMENT_FUNCTION );

        if( data == nullptr )
        {
            LOGGER_ERROR( "resource movie '%s' group '%s' invalid compile data"
                , this->getName().c_str()
                , this->getGroupName().c_str()
            );

            return false;
        }

        if( data->acquire() == false )
        {
            return false;
        }

        m_movieData = data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie2::_release()
    {
        if( m_movieData != nullptr )
        {
            m_movieData->release();
            m_movieData = nullptr;
        }
    }
}