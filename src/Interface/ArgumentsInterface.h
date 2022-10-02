#pragma once

#include "Interface/Interface.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ArgumentsInterface
        : public Interface
    {
    public:
        virtual void addArgument( const Char * _argument ) = 0;
        virtual void addArguments( const Char * _arguments ) = 0;
        virtual const Char * getArgument( uint32_t _index ) const = 0;
        virtual uint32_t getArgumentCount() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ArgumentsInterface> ArgumentsInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}