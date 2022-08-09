#include "LoaderResourceSpineAtlasTexturepacker.h"

#include "Interface/ResourceBankInterface.h"

#include "ResourceSpineAtlasTexturepacker.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceSpineAtlasTexturepacker::LoaderResourceSpineAtlasTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceSpineAtlasTexturepacker::~LoaderResourceSpineAtlasTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceSpineAtlasTexturepacker::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceSpineAtlasTexturepacker * resource = stdex::intrusive_get<ResourceSpineAtlasTexturepacker *>( _loadable );

        const ConstString & groupName = resource->getGroupName();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSpineAtlasTexturepacker * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceSpineAtlasTexturepacker *>(_meta);

        ConstString resourceTexturepackerName;
        metadata->get_Texturepacker_Name( &resourceTexturepackerName );

        ResourceBankInterface * resourceBank = resource->getResourceBank();

        const ResourcePtr & resourceTextupacker = resourceBank->getResource( groupName, resourceTexturepackerName );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceTextupacker, "'%s' group '%s' invalid get resource '%s'"
            , resource->getName().c_str()
            , resource->getGroupName().c_str()
            , resourceTexturepackerName.c_str()
        );

        resource->addResourceTexturepacker( resourceTextupacker );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
