#pragma once

#include "Kernel/Factorable.h"

#include "Config/Char.h"
#include "Config/StdInt.h"

#ifndef MENGINE_OPTIONS_VALUES_MAX
#define MENGINE_OPTIONS_VALUES_MAX 16
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OptionInterface
        : public Factorable
    {
    public:
        virtual const Char * getKey() const = 0;
        virtual const Char * getValue( uint32_t _index ) const = 0;
        virtual uint32_t getValueCount() const = 0;
        virtual bool isProtected() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OptionInterface> OptionInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
