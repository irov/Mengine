#include "ResourceFileValidator.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceFileValidator::ResourceFileValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceFileValidator::~ResourceFileValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceFileValidator::_validate( const ResourceFilePtr & _resource )
    {
        const FilePath & filePath = _resource->getFilePath();
        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();

        if( fileGroup->existFile( filePath, true ) == false )
        {
            LOGGER_ERROR( "resource '%s' group '%s' file group '%s' not found file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFileGroup()->getName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        return true;
    }
}