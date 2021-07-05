#include "ResourceAstralaxValidator.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceAstralaxValidator::ResourceAstralaxValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceAstralaxValidator::~ResourceAstralaxValidator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceAstralaxValidator::_validate( const ResourceAstralaxPtr & _resource ) const
    {
        bool successful = true;

        const ContentInterfacePtr & content = _resource->getContent();

        const FilePath & filePath = content->getFilePath();
        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();

        AstralaxEmitterContainerInterfacePtr container = ASTRALAX_SERVICE()
            ->createEmitterContainerFromFile( fileGroup, filePath, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( container, "resource '%s' group '%s' can't create container file '%s'"
            , _resource->getName().c_str()
            , _resource->getGroupName().c_str()
            , _resource->getContent()->getFilePath().c_str()
        );

        if( container->isValid() == false )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' can't valid container '%s'"
                , _resource->getName().c_str()
                , _resource->getContent()->getFilePath().c_str()
            );

            successful = false;
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}