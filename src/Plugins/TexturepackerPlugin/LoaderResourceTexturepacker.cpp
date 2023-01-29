#include "LoaderResourceTexturepacker.h"

#include "Interface/ResourceServiceInterface.h"

#include "ResourceTexturepacker.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceTexturepacker::LoaderResourceTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LoaderResourceTexturepacker::~LoaderResourceTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LoaderResourceTexturepacker::load( const LoadableInterfacePtr & _loadable, const Metabuf::Metadata * _meta )
    {
        ResourceTexturepacker * resource = _loadable.getT<ResourceTexturepacker *>();

        const ConstString & groupName = resource->getGroupName();

        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceTexturepacker * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceTexturepacker *>(_meta);

        const ConstString & resourceJSONName = metadata->get_JSON_Name();
        const ConstString & resourceImageName = metadata->get_Image_Name();

        ResourceBankInterface * resourceBank = resource->getResourceBank();

        const ResourcePtr & resourceJSON = resourceBank->getResource( groupName, resourceJSONName );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceJSON, "'%s' group '%s' invalid get resource '%s'"
            , resource->getName().c_str()
            , resource->getGroupName().c_str()
            , resourceJSONName.c_str()
        );

        resource->setResourceJSON( resourceJSON );

        const ResourceImagePtr & resourceImage = resourceBank->getResource( groupName, resourceImageName );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, "'%s' group '%s' invalid get resource '%s'"
            , resource->getName().c_str()
            , resource->getGroupName().c_str()
            , resourceImageName.c_str()
        );

        resource->setResourceImage( resourceImage );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
