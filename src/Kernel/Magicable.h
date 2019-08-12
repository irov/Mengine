#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/Magic.h"

namespace Mengine
{
    class Magicable
        : public Mixin
    {
    public:
        virtual uint32_t getMagicNumber() const
        {
            return 0;
        };

        virtual uint32_t getMagicVersion() const
        {
            return 0;
        };
    };
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_MAGICABLE(Magic)\
public:\
    uint32_t getMagicNumber() const override{ return GET_MAGIC_NUMBER( Magic ); }\
    uint32_t getMagicVersion() const override{ return GET_MAGIC_VERSION( Magic ); }\
protected:
//////////////////////////////////////////////////////////////////////////