#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class EventationInterface;
    //////////////////////////////////////////////////////////////////////////
    class Eventable
        : public Mixin
    {
    public:
        virtual EventationInterface * getEventation()
        {
            return nullptr;
        };

        virtual const EventationInterface * getEventation() const
        {
            return nullptr;
        };
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Eventable> EventablePtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_EVENTABLE()\
public:\
    Mengine::EventationInterface * getEventation() override{ return this; }\
    const Mengine::EventationInterface * getEventation() const override{ return this; }\
    Mengine::Eventable * getEventable() override{ return this; }\
protected:
//////////////////////////////////////////////////////////////////////////