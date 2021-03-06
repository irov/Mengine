#include "ResourceHelper.h"

#include "Interface/ContentInterface.h"
#include "Interface/ResourceServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        const FilePath & getResourceFilePath( const ResourcePtr & _resource )
        {
            const ContentInterfacePtr & content = _resource->getContent();

            if( content == nullptr )
            {
                return FilePath::none();
            }

            const FilePath & filePath = content->getFilePath();

            return filePath;
        }
        //////////////////////////////////////////////////////////////////////////
        const FilePath & getResourceFilePathByName( const ConstString & _resourceName )
        {
            ResourcePtr resource;
            if( RESOURCE_SERVICE()
                ->hasResource( ConstString::none(), _resourceName, false, &resource ) == false )
            {
                return FilePath::none();
            }

            const FilePath & filePath = Helper::getResourceFilePath( resource );

            return filePath;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}