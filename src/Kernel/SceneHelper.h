#pragma once

#include "Interface/DocumentInterface.h"

#include "Kernel/Scene.h"
#include "Kernel/SceneEventReceiverInterface.h"

namespace Mengine
{
    namespace Helper
    {
        ScenePtr makeScene( const SceneEventReceiverInterfacePtr & _eventReceiver, const DocumentInterfacePtr & _doc );
    }
}