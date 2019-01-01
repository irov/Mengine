#pragma once

#include "Kernel/Mixin.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ArgumentsInterface
        : public Mixin
    {
    public:
        virtual void addArgument( const Char * _argument ) = 0;
        virtual const Char * getArgument( uint32_t _index ) const = 0;
        virtual uint32_t getArgumentCount() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ArgumentsInterface> ArgumentsInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}