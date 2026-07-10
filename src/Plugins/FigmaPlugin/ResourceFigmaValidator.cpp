#include "ResourceFigmaValidator.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceFigmaValidator::ResourceFigmaValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceFigmaValidator::~ResourceFigmaValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceFigmaValidator::_validate( const ResourceFigmaPtr & _resource ) const
    {
        const ContentInterfacePtr & content = _resource->getContent();

        MENGINE_ASSERTION_MEMORY_PANIC( content, "resource '%s' is not contentable"
            , _resource->getType().c_str()
        );

        if( content->exist( true ) == false )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource figma '%s' group '%s' file group '%s' not found file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFileGroup()->getName().c_str()
                , content->getFilePath().c_str()
            );

            return false;
        }

        const FilePath & sidecarPath = _resource->getSidecarPath();

        if( sidecarPath.empty() == false && content->getFileGroup()->existFile( sidecarPath, true ) == false )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource figma '%s' group '%s' file group '%s' not found sidecar '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFileGroup()->getName().c_str()
                , sidecarPath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
