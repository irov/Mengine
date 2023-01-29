#include "LoaderResourceCursorSystem.h"

#include "Interface/ContentInterface.h"

#include "Engine/ResourceCursorSystem.h"

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
        ResourceCursorSystem * resource = _loadable.getT<ResourceCursorSystem *>();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorSystem * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorSystem *>(_meta);

        const ContentInterfacePtr & content = resource->getContent();

        metadata->getm_File_Path( content.get(), &ContentInterface::setFilePath );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}