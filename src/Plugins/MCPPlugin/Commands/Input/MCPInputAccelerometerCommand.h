#pragma once

#include "../MCPCommandInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPInputAccelerometerCommand
        : public MCPCommandInterface
        , public Factorable
    {
    public:
        MCPInputAccelerometerCommand();

        EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPInputAccelerometerCommand, MCPCommandInterface> MCPInputAccelerometerCommandPtr;
    //////////////////////////////////////////////////////////////////////////
}
