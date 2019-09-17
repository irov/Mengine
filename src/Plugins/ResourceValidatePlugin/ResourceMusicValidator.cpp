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

        const FilePath & filePath = _resource->getFilePath();

        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();

        if( fileGroup->existFile( filePath, true ) == false )
        {
            LOGGER_ERROR( "resource '%s' group '%s' music '%s' not exist"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        return true;
    }
}