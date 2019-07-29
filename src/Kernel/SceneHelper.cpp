#include "SceneHelper.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/EntityEventable.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Document.h"

namespace Mengine
{
    namespace Helper
    {
        ScenePtr makeScene( const SceneEventReceiverPtr & _eventReceiver )
        {
            ScenePtr scene = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "Scene" ), ConstString::none(), MENGINE_DOCUMENT_FUNCTION );

            EntityEventablePtr eventable = PROTOTYPE_SERVICE()
                ->generatePrototype( STRINGIZE_STRING_LOCAL( "EntityEventable" ), ConstString::none(), MENGINE_DOCUMENT_FUNCTION );

            EventationInterface * eventation = eventable->getEventation();

            eventation->setReceiver( RECEIVER_MASK_FULL, _eventReceiver );

            scene->setBehaviorEventable( eventable );

            scene->onCreate();

            return scene;
        }
    }
}