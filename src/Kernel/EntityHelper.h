#pragma once

#include "Interface/DocumentInterface.h"

#include "Kernel/Entity.h"
#include "Kernel/EntityEventReceiverInterface.h"

namespace Mengine
{
    namespace Helper
    {
        EntityPtr generateEntity( const EntityEventReceiverInterfacePtr & _eventReceiver, const DocumentInterfacePtr & _doc );
    }
}