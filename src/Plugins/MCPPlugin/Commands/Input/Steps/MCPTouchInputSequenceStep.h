#pragma once

#include "MCPInputSequenceStepInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPTouchInputSequenceStep
        : public MCPInputSequenceStepInterface
        , public Factorable
    {
    public:
        explicit MCPTouchInputSequenceStep( const MCPCommandInterfacePtr & _command );

        bool execute( const MCPInputSequenceStepRequest & _request, MCPInputSequenceStepResponse * const _response ) override;

    protected:
        MCPCommandInterfacePtr m_command;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPTouchInputSequenceStep, MCPInputSequenceStepInterface> MCPTouchInputSequenceStepPtr;
    //////////////////////////////////////////////////////////////////////////
}
