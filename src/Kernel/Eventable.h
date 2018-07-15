#pragma once

#include "Interface/EventInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/Mixin.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Eventable
        : public Mixin
    {
    public:
        virtual EventInterfacePtr getEvent()
        {
            return nullptr;
        };
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Eventable> EventablePtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#   define DECLARE_EVENTABLE( Type )\
public:\
    typedef Type EventReceiverType;\
    Mengine::EventInterfacePtr getEvent() override{ return this; }\
protected:
