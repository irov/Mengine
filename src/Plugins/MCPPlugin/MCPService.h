#pragma once

#include "MCPInterface.h"
#include "MCPProtocol.h"

#include "Commands/MCPCommandInterface.h"
#include "Commands/MCPCommandRegistry.h"
#include "Commands/Wait/MCPWaitConditionInterface.h"
#include "Commands/Input/Steps/MCPInputSequenceStepInterface.h"
#include "Contexts/MCPRuntimeContext.h"
#include "Contexts/MCPSceneContext.h"
#include "Contexts/MCPInputContext.h"
#include "Contexts/MCPHandlerRegistry.h"
#include "Contexts/MCPResourceContext.h"
#include "Contexts/MCPScriptContext.h"
#include "Contexts/MCPDebuggerContext.h"

#include "Interface/LoggerInterface.h"
#include "Interface/SocketInterface.h"
#include "Interface/ThreadIdentityInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/Deque.h"
#include "Kernel/LoggerMessage.h"
#include "Kernel/Map.h"
#include "Kernel/ServiceBase.h"
#include "Kernel/Set.h"

#include "Config/Atomic.h"

namespace Mengine
{
    class MCPLogger;

    //////////////////////////////////////////////////////////////////////////
    class MCPService
        : public ServiceBase<MCPServiceInterface>
        , public MCPCommandHostInterface
    {
        DECLARE_FACTORABLE( MCPService );
        DECLARE_LIFECYCLEABLE();

    public:
        MCPService();
        ~MCPService() override;

    public:
        static void setLaunchConfig( const Char * _host, const Char * _port, const Char * _token, const Char * _mode );
        static void clearLaunchConfig();

    public:
        const ServiceRequiredList & requiredServices() const override;
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        bool addHandler( const ConstString & _name, const MCPHandlerInterfacePtr & _handler ) override;
        void removeHandler( const MCPHandlerInterfacePtr & _handler ) override;

    public:
        bool isConnected() const override;
        bool isDebuggerPaused() const override;
        void captureRuntimeLog( const LoggerMessage & _message );

    public:
        const String & getMCPMode() const override;
        uint64_t getMCPUpdateGeneration() const override;
        bool isMCPConnected() const override;
        bool isMCPDebuggerPaused() const override;
        void setMCPDebuggerPaused( bool _paused ) override;
        void processMCPDebuggerRequests() override;
        void sendMCPCommandResponse( uint32_t _requestId, const jpp::object & _result ) override;
        void sendMCPCommandError( uint32_t _requestId, const Char * _code, const Char * _message ) override;
        void sendMCPCommandBinary( uint32_t _requestId, const Data & _data ) override;

    protected:
        void _update() override;
        void processRequests_();

    protected:
        struct IncomingRequest
        {
            uint32_t requestId;
            Data payload;
            Data attachment;
        };

        struct IncomingAssembly
        {
            Data payload;
            Data attachment;
            Set<uint32_t> chunks;
            uint32_t chunkCount;
            size_t expectedSize;
        };

        typedef Deque<IncomingRequest> DequeIncomingRequests;
        typedef Deque<Data> DequeOutgoingFrames;
        typedef Map<uint32_t, IncomingAssembly> MapIncomingAssemblies;

    protected:
        bool processSocket_( const ThreadIdentityRunnerInterfacePtr & _runner );
        bool connectSocket_();
        bool flushOutgoing_();
        bool receiveIncoming_();
        bool processFrame_( MCPFrame & _frame );
        void disconnectSocket_();

        void queueHandshake_();
        void queueResponse_( uint32_t _requestId, const jpp::object & _response );
        void queueError_( uint32_t _requestId, const Char * _code, const Char * _message );
        void queueFrame_( Data & _frame );
        void queueBinary_( uint32_t _requestId, const Data & _data );

        void processRequest_( IncomingRequest & _request );

    protected:
        ThreadMutexInterfacePtr m_incomingMutex;
        ThreadMutexInterfacePtr m_outgoingMutex;
        ThreadIdentityInterfacePtr m_thread;
        SocketInterfacePtr m_socket;
        MCPFrameDecoder m_decoder;

        DequeIncomingRequests m_incomingRequests;
        DequeOutgoingFrames m_outgoingFrames;
        Set<uint32_t> m_cancelledRequests;
        MapIncomingAssemblies m_incomingAssemblies;

        AtomicBool m_socketConnected;
        AtomicBool m_connected;
        AtomicBool m_debuggerPaused;
        AtomicBool m_revertOverlays;
        AtomicBool m_disconnectCommands;

        String m_host;
        String m_port;
        String m_token;
        String m_mode;
        uint64_t m_updateGeneration;

        MCPCommandRegistry m_commandRegistry;
        MCPWaitConditionRegistry m_waitConditionRegistry;
        MCPInputSequenceStepRegistry m_inputSequenceStepRegistry;
        MCPRuntimeContext m_runtimeContext;
        MCPSceneContext m_sceneContext;
        MCPInputContext m_inputContext;
        MCPHandlerRegistry m_handlerRegistry;
        MCPResourceContext m_resourceContext;
        MCPScriptContext m_scriptContext;
        MCPDebuggerContext m_debuggerContext;

        LoggerInterfacePtr m_runtimeLogger;
    };
    //////////////////////////////////////////////////////////////////////////
}
