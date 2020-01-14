#pragma once

#include "Kernel/Entity.h"
#include "Kernel/EntityEventReceiverInterface.h"
#include "Kernel/Document.h"

namespace Mengine
{
    namespace Helper
    {
        EntityPtr generateEntity( const EntityEventReceiverInterfacePtr & _eventReceiver, const DocumentPtr & _doc );
    }
}