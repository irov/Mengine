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

        virtual UnknownConstPointer getUnknown() const
        {
            return nullptr;
        };

        virtual UnknownDynamicPointer getDynamicUnknown()
        {
            return nullptr;
        }

        virtual UnknownDynamicConstPointer getDynamicUnknown() const
        {
            return nullptr;
        }
    };
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_UNKNOWABLE()\
public:\
    Mengine::UnknownPointer getUnknown() override{ return this; }\
    Mengine::UnknownConstPointer getUnknown() const override{ return this; }\
    Mengine::UnknownDynamicPointer getDynamicUnknown() override{ return this; }\
    Mengine::UnknownDynamicConstPointer getDynamicUnknown() const override{ return this; }\
protected:
//////////////////////////////////////////////////////////////////////////