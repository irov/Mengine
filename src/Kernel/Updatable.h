#pragma once

#include "Interface/UpdationInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Updatable
        : public Mixin
    {
    public:
        virtual UpdationInterfacePtr getUpdation()
        {
            return nullptr;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Updatable> UpdatablePtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_UPDATABLE()\
public:\
    Mengine::UpdationInterfacePtr getUpdation() override{ return this; }\
protected:
