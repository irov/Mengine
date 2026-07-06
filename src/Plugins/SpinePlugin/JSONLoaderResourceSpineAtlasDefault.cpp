#include "JSONLoaderResourceSpineAtlasDefault.h"

#include "Interface/ResourceBankInterface.h"

#include "ResourceSpineAtlasDefault.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/JSONDataHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceSpineAtlasDefault::JSONLoaderResourceSpineAtlasDefault()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    JSONLoaderResourceSpineAtlasDefault::~JSONLoaderResourceSpineAtlasDefault()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool JSONLoaderResourceSpineAtlasDefault::load( const FactorablePtr & _factorable, const jpp::object & _json )
    {
        ResourceSpineAtlasDefault * resource = _factorable.getT<ResourceSpineAtlasDefault *>();

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

        ResourceBankInterface * resourceBank = resource->getResourceBank();

        jpp::object j_images;
        if( _json.exist( "Image", &j_images ) == true )
        {
            if( j_images.is_type_array() == false )
            {
                return false;
            }

            for( const jpp::object & meta_image : jpp::array( j_images ) )
            {
                ConstString name;
                ConstString resourceName;

                if( Helper::getJSONConstString( meta_image, "Name", &name ) == false ||
                    Helper::getJSONConstString( meta_image, "Resource", &resourceName ) == false )
                {
                    return false;
                }

                const ResourceImagePtr & resourceImage = resourceBank->getResource( groupName, resourceName );

                MENGINE_ASSERTION_MEMORY_PANIC( resourceImage, "'%s' category '%s' group '%s' invalid get image resource '%s'"
                    , resource->getName().c_str()
                    , content->getFileGroup()->getName().c_str()
                    , resource->getGroupName().c_str()
                    , resourceName.c_str()
                );

                resource->addResourceImageDesc( name, resourceImage );
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
