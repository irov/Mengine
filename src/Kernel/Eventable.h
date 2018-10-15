#pragma once

#include "Interface/EventationInterface.h"

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
        virtual EventationInterface * getEventation()
        {
            return nullptr;
        };
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Eventable> EventablePtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_EVENTABLE( Type )\
public:\
    typedef Type EventReceiverType;\
    Mengine::EventationInterface * getEventation() override{ return this; }\
protected:
