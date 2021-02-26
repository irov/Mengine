#include "ResourceFileValidator.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/MemoryStreamHelper.h"
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
        bool successful = true;

        const ContentInterfacePtr & content = _resource->getContent();

        MENGINE_ASSERTION_MEMORY_PANIC( content, "resource '%s' is not contentable"
            , _resource->getType().c_str()
        );

        const FilePath & filePath = content->getFilePath();
        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();

        if( fileGroup->existFile( filePath, true ) == false )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' file group '%s' not found file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
            );

            return false;
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}