#pragma once

#include "MCPInputSequenceStepInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPKeyboardInputSequenceStep
        : public MCPInputSequenceStepInterface
        , public Factorable
    {
    public:
        explicit MCPKeyboardInputSequenceStep( const MCPCommandInterfacePtr & _command );

        bool execute( const MCPInputSequenceStepRequest & _request, MCPInputSequenceStepResponse * const _response ) override;

    protected:
        MCPCommandInterfacePtr m_command;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPKeyboardInputSequenceStep, MCPInputSequenceStepInterface> MCPKeyboardInputSequenceStepPtr;
    //////////////////////////////////////////////////////////////////////////
}
