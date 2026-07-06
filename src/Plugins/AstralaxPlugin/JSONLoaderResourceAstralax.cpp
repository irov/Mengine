#include "JSONLoaderResourceAstralax.h"

#include "Interface/ResourceServiceInterface.h"

#include "ResourceAstralax.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/JSONDataHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceAstralax::JSONLoaderResourceAstralax()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceAstralax::~JSONLoaderResourceAstralax()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceAstralax::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceAstralax * resource = _factorable.getT<ResourceAstralax *>();

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

        uint32_t atlasCount;
        if( Helper::getJSONMemberUInt32( _json, "AtlasCount", "Value", &atlasCount ) == false )
        {
            return false;
        }

        VectorResourceImages resourceImages( atlasCount );

        jpp::object j_atlas;
        if( _json.exist( "Atlas", &j_atlas ) == true )
        {
            if( j_atlas.is_type_array() == false )
            {
                return false;
            }

            for( const jpp::object & atlas : jpp::array( j_atlas ) )
            {
                uint32_t index;
                ConstString resourceImageName;

                if( Helper::getJSONUInt32( atlas, "Index", &index ) == false ||
                    Helper::getJSONConstString( atlas, "ResourceName", &resourceImageName ) == false )
                {
                    return false;
                }

                ResourceImagePtr resourceImage = RESOURCE_SERVICE()
                    ->getResourceReference( groupName, resourceImageName );

                MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, "resource '%s' container '%s' can't get atlas image '%s'"
                    , resource->getName().c_str()
                    , Helper::getContentFullPath( resource->getContent() ).c_str()
                    , resourceImageName.c_str()
                );

                resourceImages[index] = resourceImage;
            }
        }

        resource->setResourceImages( resourceImages );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
