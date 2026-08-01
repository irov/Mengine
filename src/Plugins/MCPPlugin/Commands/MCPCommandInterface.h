#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/Data.h"
#include "Kernel/Mixin.h"
#include "Kernel/String.h"

#include "jpp/jpp.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum class EMCPCommandStatus
    {
        SUCCESS,
        FAILURE,
        DEFERRED
    };
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
        String errorCode;
        String errorMessage;
    };
    //////////////////////////////////////////////////////////////////////////
    class MCPCommandHostInterface
    {
    public:
        virtual ~MCPCommandHostInterface() = default;

    public:
        virtual const String & getMCPMode() const = 0;
        virtual uint64_t getMCPUpdateGeneration() const = 0;
        virtual bool isMCPConnected() const = 0;
        virtual bool isMCPDebuggerPaused() const = 0;
        virtual void setMCPDebuggerPaused( bool _paused ) = 0;
        virtual void processMCPDebuggerRequests() = 0;

        virtual void sendMCPCommandResponse( uint32_t _requestId, const jpp::object & _result ) = 0;
        virtual void sendMCPCommandError( uint32_t _requestId, const Char * _code, const Char * _message ) = 0;
        virtual void sendMCPCommandBinary( uint32_t _requestId, const Data & _data ) = 0;
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
        virtual void update();
        virtual void cancel( uint32_t _requestId );
        virtual void disconnect();

    protected:
        bool setName_( const ConstString & _name );

    protected:
        ConstString m_name;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPCommandInterface> MCPCommandInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
