#pragma once

#include "Kernel/Scene.h"
#include "Kernel/SceneEventReceiverInterface.h"
#include "Kernel/Document.h"

namespace Mengine
{
    namespace Helper
    {
        ScenePtr makeScene( const SceneEventReceiverInterfacePtr & _eventReceiver, const DocumentPtr & _doc );
    }
}