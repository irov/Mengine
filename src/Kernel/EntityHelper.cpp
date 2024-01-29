#include "EntityHelper.h"

#include "Kernel/Logger.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/PrototypeHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        EntityPtr generateEntity( const EntityEventReceiverInterfacePtr & _eventReceiver, const DocumentInterfacePtr & _doc )
        {
            EntityPtr entity = Helper::generatePrototype( Entity::getFactorableType(), ConstString::none(), _doc );

            EntityEventablePtr eventable = Helper::generatePrototype( STRINGIZE_STRING_LOCAL( "EntityEventable" ), ConstString::none(), _doc );

            EventationInterface * eventation = eventable->getEventation();

            eventation->setReceiver( EVENTATION_RECEIVER_MASK_FULL, _eventReceiver );

            entity->setBehaviorEventable( eventable );

            if( entity->create() == false )
            {
                LOGGER_ERROR( "invalid create entity (doc %s)"
                    , MENGINE_DOCUMENT_STR( _doc )
                );

                return nullptr;
            }

            return entity;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}