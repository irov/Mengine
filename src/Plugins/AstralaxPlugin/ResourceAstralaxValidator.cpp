#include "ResourceAstralaxValidator.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"

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

        AstralaxEmitterContainerInterfacePtr container = ASTRALAX_SERVICE()
            ->createEmitterContainerFromFile( content, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( container, "resource '%s' group '%s' can't create container file '%s'"
            , _resource->getName().c_str()
            , _resource->getGroupName().c_str()
            , Helper::getContentFullPath( _resource->getContent() ).c_str()
        );

        if( container->isValid() == false )
        {
            LOGGER_MESSAGE_RELEASE_ERROR( "resource '%s' can't valid container '%s'"
                , _resource->getName().c_str()
                , Helper::getContentFullPath( _resource->getContent() ).c_str()
            );

            successful = false;
        }

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}