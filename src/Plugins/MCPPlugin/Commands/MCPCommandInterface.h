#pragma once

#include "MCPOperationInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Data.h"
#include "Kernel/Mixin.h"
#include "Kernel/String.h"

#include "jpp/jpp.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct MCPCommandRequest
    {
        uint32_t requestId;
        const jpp::object & params;
        const Data & attachment;
    };
    //////////////////////////////////////////////////////////////////////////
    struct MCPCommandResponse
    {
        MCPCommandResponse();

        jpp::object result;
        Data attachment;
        MCPOperationInterfacePtr operation;
        String errorCode;
        String errorMessage;
    };
    //////////////////////////////////////////////////////////////////////////
    class MCPCommandHostInterface
    {
    public:
        virtual ~MCPCommandHostInterface();

    public:
        virtual const String & getMCPMode() const = 0;
        virtual uint64_t getMCPUpdateFrame() const = 0;
        virtual uint64_t getMCPRenderFrame() const = 0;
        virtual bool isMCPConnected() const = 0;
        virtual bool isMCPDebuggerPaused() const = 0;
        virtual void setMCPDebuggerPaused( bool _paused ) = 0;
        virtual void processMCPDebuggerRequests() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class MCPCommandInterface
        : public Mixin
    {
        friend class MCPCommandRegistry;

    public:
        const ConstString & getName() const;

        virtual EMCPCommandStatus execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response ) = 0;
        virtual bool availableWhilePaused( const jpp::object & _params ) const;

    protected:
        bool setName_( const ConstString & _name );

    protected:
        ConstString m_name;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPCommandInterface> MCPCommandInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
