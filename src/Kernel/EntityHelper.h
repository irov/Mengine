#pragma once

#include "Kernel/Entity.h"

namespace Mengine
{
    namespace Helper
    {
        EntityPtr makeEntity( const EntityEventReceiverPtr & _eventReceiver );
    }
}