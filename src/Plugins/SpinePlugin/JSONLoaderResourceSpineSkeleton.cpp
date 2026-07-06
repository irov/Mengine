#include "JSONLoaderResourceSpineSkeleton.h"

#include "Interface/ResourceBankInterface.h"

#include "ResourceSpineSkeleton.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/JSONDataHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceSpineSkeleton::JSONLoaderResourceSpineSkeleton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceSpineSkeleton::~JSONLoaderResourceSpineSkeleton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceSpineSkeleton::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceSpineSkeleton * resource = _factorable.getT<ResourceSpineSkeleton *>();

        const ConstString & groupName = resource->getGroupName();

        const ContentInterfacePtr & content = resource->getContent();

        FilePath filePath;
        if( Helper::getJSONMemberFilePath( _json, "File", "Path", &filePath ) == true )
        {
            content->setFilePath( filePath );
        }

        ConstString converterType;
        if( Helper::getJSONMemberConstString( _json, "File", "Converter", &converterType ) == true )
        {
            content->setConverterType( converterType );
        }

        bool noExist = false;
        if( Helper::getJSONMemberBool( _json, "File", "NoExist", &noExist ) == true )
        {
            content->setValidNoExist( noExist );
        }

        ConstString resourceSpineAtlasName;
        Helper::getJSONMemberConstString( _json, "Atlas", "Name", &resourceSpineAtlasName );

        ResourceBankInterface * resourceBank = resource->getResourceBank();

        const ResourcePtr & resourceSpineAtlas = resourceBank->getResource( groupName, resourceSpineAtlasName );

        MENGINE_ASSERTION_MEMORY_PANIC( resourceSpineAtlas, "'%s' group '%s' invalid get resource '%s'"
            , resource->getName().c_str()
            , resource->getGroupName().c_str()
            , resourceSpineAtlasName.c_str()
        );

        resource->setResourceSpineAtlas( resourceSpineAtlas );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
