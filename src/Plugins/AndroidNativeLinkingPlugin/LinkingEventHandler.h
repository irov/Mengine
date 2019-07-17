#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    enum EnumLinkingEventHandler
    {
    };

    class LinkingEventHandler
        : public Mixin
    {
    public:
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LinkingEventHandler> LinkingEventHandlerPtr;
    //////////////////////////////////////////////////////////////////////////
}
