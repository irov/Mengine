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
    bool ResourceAstralaxValidator::_validate( const ResourceAstralaxPtr & _resource )
    {
        const FilePath & filePath = _resource->getFilePath();

        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();

        AstralaxEmitterContainerInterfacePtr container = ASTRALAX_SERVICE()
            ->createEmitterContainerFromFile( fileGroup, filePath, MENGINE_DOCUMENT_FACTORABLE );

        if( container == nullptr )
        {
            LOGGER_ERROR( "resource '%s' group '%s' can't create container file '%s'"
                , _resource->getName().c_str()
                , _resource->getGroupName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        if( container->isValid() == false )
        {
            LOGGER_ERROR( "resource '%s' can't valid container '%s'"
                , _resource->getName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}