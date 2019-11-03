#include "SceneHelper.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Document.h"

namespace Mengine
{
    namespace Helper
    {
        ScenePtr makeScene( const SceneEventReceiverPtr & _eventReceiver, const Char * _doc )
        {
            ScenePtr scene = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Scene" ), ConstString::none(), _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( scene, nullptr );

            EntityEventablePtr eventable = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "EntityEventable" ), ConstString::none(), _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( eventable, nullptr );

            EventationInterface * eventation = eventable->getEventation();

            MENGINE_ASSERTION_MEMORY_PANIC( eventation, nullptr );

            eventation->setReceiver( EVENTATION_RECEIVER_MASK_FULL, _eventReceiver );

            scene->setBehaviorEventable( eventable );

            if( scene->onCreate() == false )
            {
                return nullptr;
            }

            return scene;
        }
    }
}