#include "JSONLoaderResourceSpineAtlasTexturepacker.h"

#include "Interface/ResourceBankInterface.h"

#include "ResourceSpineAtlasTexturepacker.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/JSONDataHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceSpineAtlasTexturepacker::JSONLoaderResourceSpineAtlasTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceSpineAtlasTexturepacker::~JSONLoaderResourceSpineAtlasTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceSpineAtlasTexturepacker::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceSpineAtlasTexturepacker * resource = _factorable.getT<ResourceSpineAtlasTexturepacker *>();

        const ConstString & groupName = resource->getGroupName();

        ConstString resourceTexturepackerName;
        Helper::getJSONMemberConstString( _json, "Texturepacker", "Name", &resourceTexturepackerName );

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
