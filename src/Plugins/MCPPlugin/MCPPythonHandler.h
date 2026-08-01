#pragma once

#include "MCPHandlerInterface.h"

#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPPythonHandler
        : public MCPHandlerInterface
        , public PythonCallbackProvider
        , public Factorable
    {
    public:
        MCPPythonHandler( const pybind::object & _callback, const pybind::args & _args );
        ~MCPPythonHandler() override;

    protected:
        bool onMCPCall( const jpp::object & _arguments, jpp::object * const _result ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPPythonHandler, MCPHandlerInterface> MCPPythonHandlerPtr;
    //////////////////////////////////////////////////////////////////////////
}
