#include "ResourceMusicValidator.h"

#include "Interface/FileGroupInterface.h"

#include "Kernel/MemoryStreamHelper.h"
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
    bool ResourceMusicValidator::_validate( const ResourceMusicPtr & _resource ) const
    {
        bool external = _resource->isExternal();

        if( external == true )
        {
            return true;
        }

        const ContentInterfacePtr & content = _resource->getContent();

        if( content->exist( true ) == false )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' group '%s' music '%s' not exist"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , content->getFilePath().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}