#include "SceneHelper.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PrototypeHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        ScenePtr makeScene( const SceneEventReceiverInterfacePtr & _eventReceiver, const DocumentInterfacePtr & _doc )
        {
            ScenePtr scene = Helper::generatePrototype<Scene>( ConstString::none(), _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( scene, "invalid create scene (doc %s)"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            EntityEventablePtr eventable = Helper::generatePrototype<EntityEventable>( ConstString::none(), _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( eventable, "invalid create eventable (doc %s)"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            EventationInterface * eventation = eventable->getEventation();

            MENGINE_ASSERTION_MEMORY_PANIC( eventation, "invalid get eventation (doc %s)"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            eventation->setReceiver( EVENTATION_RECEIVER_MASK_FULL, _eventReceiver );

            scene->setBehaviorEventable( eventable );

            if( scene->create() == false )
            {
                LOGGER_ERROR( "invalid create scene (doc %s)"
                    , MENGINE_DOCUMENT_STR( _doc )
                );

                return nullptr;
            }

            return scene;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}