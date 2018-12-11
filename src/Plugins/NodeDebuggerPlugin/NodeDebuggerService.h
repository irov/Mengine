#pragma once

#include "NodeDebuggerInterface.h"

#include "Engine/TextField.h"

#include "Config/Vector.h"
#include "Config/Deque.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/NodeUniqueFinder.h"

#include "Interface/SocketInterface.h"
#include "Interface/ThreadServiceInterface.h"

namespace pugi { class xml_node; }

namespace Mengine
{
    enum class NodeDebuggerServerState : size_t
    {
        Invalid,
        WaitingForClient,
        Connected
    };

    enum class NodeDebuggerPacketType : size_t
    {
        // asking packets
        Ask_Scene,

        // answering packets
        Answer_Scene
    };

    struct NodeDebuggerPacket
    {
        Vector<uint8_t> payload;
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

    private:
        void privateInit();
        void sendPacket( NodeDebuggerPacket & _packet );
        void sendScene( const Mengine::ScenePtr & _scene );
        void serializeNode( const Mengine::NodePtr & _node, pugi::xml_node & _xmlParentNode );
        void serializeRender( const RenderInterface * _render, pugi::xml_node & _xmlParentNode );
        void serializeAnimation( const AnimationInterface * _animation, pugi::xml_node & _xmlParentNode );
        void processPacket( NodeDebuggerPacket & _packet );
        void receiveChangedNode( const pugi::xml_node& _xmlNode );
        VectorNodePath stringToPath( const String & _str );

    protected:
        ScenePtr m_scene;
        SocketInterfacePtr m_socket;
        ThreadJobPtr m_threadJob;
        ThreadMutexInterfacePtr m_dataMutex;
        NodeDebuggerServerState m_serverState;
        Deque<NodeDebuggerPacket> m_incomingPackets;
        Deque<NodeDebuggerPacket> m_outgoingPackets;
        Vector<uint8_t> m_receivedData;
    };
}