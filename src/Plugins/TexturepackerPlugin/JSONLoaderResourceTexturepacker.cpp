#include "JSONLoaderResourceTexturepacker.h"

#include "Interface/ResourceBankInterface.h"

#include "ResourceTexturepacker.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/JSONDataHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceTexturepacker::JSONLoaderResourceTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceTexturepacker::~JSONLoaderResourceTexturepacker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceTexturepacker::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceTexturepacker * resource = _factorable.getT<ResourceTexturepacker *>();

        const ConstString & groupName = resource->getGroupName();

        ConstString resourceJSONName;
        ConstString resourceImageName;

        if( Helper::getJSONMemberConstString( _json, "JSON", "Name", &resourceJSONName ) == false ||
            Helper::getJSONMemberConstString( _json, "Image", "Name", &resourceImageName ) == false )
        {
            return false;
        }

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
