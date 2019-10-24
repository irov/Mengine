#include "EntityHelper.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/EntityEventable.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Document.h"

namespace Mengine
{
    namespace Helper
    {
        EntityPtr generateEntity( const EntityEventReceiverPtr & _eventReceiver )
        {
            EntityPtr entity = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Entity" ), ConstString::none(), MENGINE_DOCUMENT_FUNCTION );

            EntityEventablePtr eventable = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "EntityEventable" ), ConstString::none(), MENGINE_DOCUMENT_FUNCTION );

            EventationInterface * eventation = eventable->getEventation();

            eventation->setReceiver( EVENTATION_RECEIVER_MASK_FULL, _eventReceiver );

            entity->setBehaviorEventable( eventable );

            entity->onCreate();

            return entity;
        }
    }
}