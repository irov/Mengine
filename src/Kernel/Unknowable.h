#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/Pointer.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef PointerT<UnknownInterface> PointerUnknown;
    //////////////////////////////////////////////////////////////////////////
    class Unknowable
        : public Mixin
    {
    public:
        virtual PointerUnknown getUnknown()
        {
            return nullptr;
        };
    };
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_UNKNOWABLE()\
public:\
    Mengine::PointerUnknown getUnknown() override{ return this; }\
protected:
//////////////////////////////////////////////////////////////////////////