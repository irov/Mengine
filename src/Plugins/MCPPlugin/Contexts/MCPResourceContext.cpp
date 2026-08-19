#include "MCPResourceContext.h"

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Resource.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPFileGroupPtr MCPResourceContext::getOverlayFileGroup( const FileGroupInterfacePtr & _baseFileGroup )
    {
        FileGroupInterface * baseFileGroup = _baseFileGroup.get();
        MapOverlayGroups::const_iterator it = m_overlayGroups.find( baseFileGroup );

        if( it != m_overlayGroups.end() )
        {
            MCPFileGroupPtr overlay = it->second;

            return overlay;
        }

        DocumentInterfacePtr document = MENGINE_DOCUMENT_FUNCTION;
        MCPFileGroupPtr overlay = Helper::makeFactorableUnique<MCPFileGroup>( document );
        ConstString name = _baseFileGroup->getName();
        const FileGroupInterfacePtr & parentFileGroup = _baseFileGroup->getParentFileGroup();
        const FilePath & relationPath = _baseFileGroup->getRelationPath();
        const FilePath & folderPath = _baseFileGroup->getFolderPath();
        overlay->setName( name );
        overlay->setBaseFileGroup( _baseFileGroup );
        overlay->setParentFileGroup( parentFileGroup );
        overlay->setRelationPath( relationPath );
        overlay->setFolderPath( folderPath );

        if( overlay->initialize() == false )
        {
            return nullptr;
        }

        m_overlayGroups.emplace( baseFileGroup, overlay );

        return overlay;
    }
    //////////////////////////////////////////////////////////////////////////
    MCPFileGroupPtr MCPResourceContext::findOverlayFileGroup( const FileGroupInterfacePtr & _baseFileGroup ) const
    {
        FileGroupInterface * baseFileGroup = _baseFileGroup.get();
        MapOverlayGroups::const_iterator it = m_overlayGroups.find( baseFileGroup );

        if( it == m_overlayGroups.end() )
        {
            return nullptr;
        }

        MCPFileGroupPtr overlay = it->second;

        return overlay;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPResourceContext::recompileOverlayResources( const ConstString & _fileGroup, const FilePath & _filePath, const MCPFileGroupPtr & _overlay, jpp::array * const _resources, String * const _error )
    {
        Vector<ResourcePtr> affected;
        RESOURCE_SERVICE()->foreachResources( [&affected, &_fileGroup, &_filePath, &_overlay]( const ResourcePtr & _resource )
        {
            const ContentInterfacePtr & content = _resource->getContent();

            if( content == nullptr || content->getFilePath() != _filePath )
            {
                return;
            }

            const FileGroupInterfacePtr & fileGroup = content->getFileGroup();

            if( fileGroup == nullptr || (fileGroup != _overlay && fileGroup->getName() != _fileGroup) )
            {
                return;
            }

            affected.emplace_back( _resource );
        } );

        for( const ResourcePtr & resource : affected )
        {
            if( resource->isMapping() == true )
            {
                *_error = "mapped resources require application restart";

                return false;
            }
        }

        for( const ResourcePtr & resource : affected )
        {
            const ContentInterfacePtr & content = resource->getContent();
            MapOriginalGroups::const_iterator originalIt = m_overlayOriginalGroups.find( content );

            if( originalIt == m_overlayOriginalGroups.end() )
            {
                const FileGroupInterfacePtr & originalFileGroup = content->getFileGroup();
                m_overlayOriginalGroups.emplace( content, originalFileGroup );
            }

            bool successful;

            if( resource->isCompile() == false && resource->getCompileReferenceCount() != 0 )
            {
                content->setFileGroup( _overlay );
                successful = resource->Compilable::compile();
            }
            else
            {
                successful = resource->recompile( [content, _overlay]()
                {
                    content->setFileGroup( _overlay );

                    return true;
                } );
            }

            if( successful == false )
            {
                *_error = "resource compilation failed";

                return false;
            }

            ConstString name = resource->getName();
            ConstString group = resource->getGroupName();
            ConstString type = Helper::getFactorableType( resource );
            bool compiled = resource->isCompile();
            jpp::object item = jpp::make_object();
            item.set( "name", name );
            item.set( "group", group );
            item.set( "type", type );
            item.set( "compiled", compiled );
            _resources->push_back( item );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPResourceContext::revertAllOverlays()
    {
        for( MapOverlayGroups::value_type & value : m_overlayGroups )
        {
            value.second->clearOverlays();
        }

        for( MapOriginalGroups::value_type & value : m_overlayOriginalGroups )
        {
            const ContentInterfacePtr & content = value.first;
            const FileGroupInterfacePtr & original = value.second;

            RESOURCE_SERVICE()->foreachResources( [&content, &original]( const ResourcePtr & _resource )
            {
                const ContentInterfacePtr & resourceContent = _resource->getContent();

                if( resourceContent != content )
                {
                    return;
                }

                if( _resource->isCompile() == false && _resource->getCompileReferenceCount() != 0 )
                {
                    content->setFileGroup( original );
                    _resource->Compilable::compile();

                    return;
                }

                _resource->recompile( [content, original]()
                {
                    content->setFileGroup( original );

                    return true;
                } );
            } );
        }

        m_overlayOriginalGroups.clear();

        for( MapOverlayGroups::value_type & value : m_overlayGroups )
        {
            value.second->finalize();
        }

        m_overlayGroups.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MCPResourceContext::getOverlayGroupCount() const
    {
        size_t count = m_overlayGroups.size();

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
}
