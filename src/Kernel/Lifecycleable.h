#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class LifecycleInterface;
    //////////////////////////////////////////////////////////////////////////
    class Lifecycleable
        : public Mixin
    {
    public:
        virtual LifecycleInterface * getLifecycleable()
        {
            return nullptr;
        }

        virtual const LifecycleInterface * getLifecycleable() const
        {
            return nullptr;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Lifecycleable> LifecycleablePtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_LIFECYCLEABLE()\
public:\
    Mengine::LifecycleInterface * getLifecycleable() override{ return this; }\
    const Mengine::LifecycleInterface * getLifecycleable() const override{ return this; }\
protected:
//////////////////////////////////////////////////////////////////////////
