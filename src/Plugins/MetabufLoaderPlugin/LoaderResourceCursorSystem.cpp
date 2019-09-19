#include "LoaderResourceCursorSystem.h"

#include "Engine/ResourceCursorSystem.h"

#include "Kernel/Content.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceCursorSystem::LoaderResourceCursorSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceCursorSystem::~LoaderResourceCursorSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceCursorSystem::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceCursorSystem * resource = stdex::intrusive_get<ResourceCursorSystem *>( _loadable );

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorSystem * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorSystem *>(_meta);

        Content * content = resource->getContent();

        metadata->getm_File_Path( content, &Content::setFilePath );

        return true;
    }
}