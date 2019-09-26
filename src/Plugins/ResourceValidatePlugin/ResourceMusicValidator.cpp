#include "ResourceMusicValidator.h"

#include "Interface/FileGroupInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceMusicValidator::ResourceMusicValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceMusicValidator::~ResourceMusicValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMusicValidator::_validate( const ResourceMusicPtr & _resource )
    {
        bool external = _resource->isExternal();

        if( external == true )
        {
            return true;
        }

        const ContentInterface * content = _resource->getContent();

        const FilePath & filePath = content->getFilePath();
        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();

        if( fileGroup->existFile( filePath, true ) == false )
        {
            LOGGER_ERROR( "resource '%s' group '%s' music '%s' not exist"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFilePath().c_str()
            );

            return false;
        }

        return true;
    }
}