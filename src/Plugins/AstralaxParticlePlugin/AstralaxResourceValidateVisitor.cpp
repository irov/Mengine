#include "AstralaxResourceValidateVisitor.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/RenderTextureInterface.h"
#include "Interface/RenderImageInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AstralaxResourceValidateVisitor::AstralaxResourceValidateVisitor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxResourceValidateVisitor::~AstralaxResourceValidateVisitor()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxResourceValidateVisitor::accept( ResourceAstralax * _resource )
    {
        const FilePath & filePath = _resource->getFilePath();
        const ConstString & name = _resource->getName();
        
        const FileGroupInterfacePtr & fileGroup = _resource->getFileGroup();

        AstralaxEmitterContainerInterfacePtr container = ASTRALAX_SERVICE()
            ->createEmitterContainerFromFile( fileGroup, filePath, name );

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
            LOGGER_ERROR( "resource %s can't valid container '%s'"
                , _resource->getName().c_str()
                , _resource->getFilePath().c_str()
            );

            return false;
        }

        return true;
    }    
}