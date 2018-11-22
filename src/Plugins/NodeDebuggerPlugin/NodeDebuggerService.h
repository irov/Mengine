#pragma once

#include "NodeDebuggerInterface.h"

#include "Engine/TextField.h"

#include "Kernel/ServiceBase.h"

#include "Interface/SocketInterface.h"
#include "Interface/ThreadServiceInterface.h"

namespace Mengine
{
    enum class NodeDebuggerServerState : size_t
    {
        Invalid,
        WaitingForClient,
        Connected
    };

    //////////////////////////////////////////////////////////////////////////
    class NodeDebuggerService
        : public ServiceBase<NodeDebuggerServiceInterface>
        , public ThreadWorkerInterface
    {
    public:
        NodeDebuggerService();
        ~NodeDebuggerService() override;

    public:
        void _dependencyService() override;
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool onWork( uint32_t _id ) override;
        void onDone( uint32_t _id ) override;

    public:
        void setScene( const Mengine::ScenePtr & _scene ) override;
        void update() override;

    protected:
        ScenePtr m_scene;
        SocketInterfacePtr m_socketListen;
        SocketInterfacePtr m_socketSpeak;
        ThreadJobPtr m_threadJobListenToSocket;
        NodeDebuggerServerState m_serverState;
    };
}