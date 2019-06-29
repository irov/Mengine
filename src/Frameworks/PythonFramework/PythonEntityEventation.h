#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/Eventable.h"
#include "Kernel/BaseEventation.h"
#include "Kernel/Entity.h"

namespace Mengine
{
    class PythonEntityEventation
        : public Factorable
        , public Eventable
        , public BaseEventation
    {
        DECLARE_EVENTABLE( EntityEventReceiver );

    public:
        PythonEntityEventation();
        ~PythonEntityEventation() override;
    };
}