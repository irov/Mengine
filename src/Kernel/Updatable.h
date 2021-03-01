#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UpdationInterface;
    //////////////////////////////////////////////////////////////////////////
    class Updatable
        : public Mixin
    {
    public:
        virtual UpdationInterface * getUpdation()
        {
            return nullptr;
        }

        virtual const UpdationInterface * getUpdation() const
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
    Mengine::UpdationInterface * getUpdation() override{ return this; }\
    const Mengine::UpdationInterface * getUpdation() const override{ return this; }\
    Mengine::Updatable * getUpdatable() override{ return this; }\
protected:
//////////////////////////////////////////////////////////////////////////