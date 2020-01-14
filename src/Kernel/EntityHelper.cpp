#include "EntityHelper.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/EntityEventable.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    namespace Helper
    {
        EntityPtr generateEntity( const EntityEventReceiverInterfacePtr & _eventReceiver, const DocumentPtr & _doc )
        {
            EntityPtr entity = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Entity" ), ConstString::none(), _doc );

            EntityEventablePtr eventable = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "EntityEventable" ), ConstString::none(), _doc );

            EventationInterface * eventation = eventable->getEventation();

            eventation->setReceiver( EVENTATION_RECEIVER_MASK_FULL, _eventReceiver );

            entity->setBehaviorEventable( eventable );

            if( entity->onCreate() == false )
            {
                return nullptr;
            }

            return entity;
        }
    }
}