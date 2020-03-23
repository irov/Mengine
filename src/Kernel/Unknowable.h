#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/UnknownPointer.h"

namespace Mengine
{
    class Unknowable
        : public Mixin
    {
    public:
        virtual UnknownPointer getUnknown()
        {
            return nullptr;
        };
    };
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_UNKNOWABLE()\
public:\
    Mengine::UnknownPointer getUnknown() override{ return this; }\
protected:
//////////////////////////////////////////////////////////////////////////