#include "SceneHelper.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        ScenePtr makeScene( const SceneEventReceiverInterfacePtr & _eventReceiver, const DocumentInterfacePtr & _doc )
        {
            ScenePtr scene = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Scene" ), ConstString::none(), _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( scene );

            EntityEventablePtr eventable = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "EntityEventable" ), ConstString::none(), _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( eventable );

            EventationInterface * eventation = eventable->getEventation();

            MENGINE_ASSERTION_MEMORY_PANIC( eventation );

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