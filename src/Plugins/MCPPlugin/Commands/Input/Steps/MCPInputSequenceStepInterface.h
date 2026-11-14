#pragma once

#include "../../MCPCommandInterface.h"

#include "Kernel/Map.h"

#include "Config/Timestamp.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct MCPInputSequenceStepRequest
    {
        const jpp::object & step;
        uint64_t startFrame;
        uint64_t currentFrame;
    };
    //////////////////////////////////////////////////////////////////////////
    struct MCPInputSequenceStepResponse
    {
        MCPInputSequenceStepResponse();

        bool completed;
        uint64_t resumeFrame;
        Timestamp resumeTimestamp;
        String errorCode;
        String errorMessage;
    };
    //////////////////////////////////////////////////////////////////////////
    class MCPInputSequenceStepInterface
        : public Mixin
    {
    public:
        virtual bool execute( const MCPInputSequenceStepRequest & _request, MCPInputSequenceStepResponse * const _response ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPInputSequenceStepInterface> MCPInputSequenceStepInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class MCPInputSequenceStepRegistry
    {
    public:
        bool addStep( const ConstString & _name, const MCPInputSequenceStepInterfacePtr & _step );
        MCPInputSequenceStepInterfacePtr findStep( const ConstString & _name ) const;
        void clear();

    protected:
        typedef Map<ConstString, MCPInputSequenceStepInterfacePtr> MapSteps;

        MapSteps m_steps;
    };
    //////////////////////////////////////////////////////////////////////////
}
