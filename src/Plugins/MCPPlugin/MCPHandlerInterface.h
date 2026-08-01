#pragma once

#include "Kernel/Mixin.h"

#include "jpp/jpp.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPHandlerInterface
        : public Mixin
    {
    public:
        virtual bool onMCPCall( const jpp::object & _arguments, jpp::object * const _result ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPHandlerInterface> MCPHandlerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
