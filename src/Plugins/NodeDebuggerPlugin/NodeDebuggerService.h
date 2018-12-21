#pragma once

#include "NodeDebuggerInterface.h"

#include "Engine/TextField.h"

#include "Config/Vector.h"
#include "Config/Deque.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/NodeUniqueFinder.h"

#include "Interface/SocketInterface.h"
#include "Interface/ThreadServiceInterface.h"

#define PUGIXML_NO_STL
#define PUGIXML_HEADER_ONLY
#include "pugixml.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum class NodeDebuggerServerState : uint32_t
    {
        Invalid,
        WaitingForClient,
        Connected
    };
    //////////////////////////////////////////////////////////////////////////
    enum class NodeDebuggerPacketType : uint32_t
    {
        // asking packets
        Ask_Scene,

        // answering packets
        Answer_Scene
    };
    //////////////////////////////////////////////////////////////////////////
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
        void setScene( const ScenePtr & _scene ) override;
        void update() override;
        void render( const RenderContext * _context ) override;

    private:
        void privateInit();
        void recreateServer();
        void sendPacket( NodeDebuggerPacket & _packet );
        void sendScene( const ScenePtr & _scene );
        void serializeNode( const NodePtr & _node, pugi::xml_node & _xmlParentNode );
        void serializeRender( const RenderInterface * _render, pugi::xml_node & _xmlParentNode );
        void serializeAnimation( const AnimationInterface * _animation, pugi::xml_node & _xmlParentNode );
        void processPacket( NodeDebuggerPacket & _packet );
        void receiveChangedNode( const pugi::xml_node & _xmlNode );
        VectorNodePath stringToPath( const String & _str );

    protected:
        ScenePtr m_scene;
        SocketInterfacePtr m_socket;
        volatile bool m_shouldRecreateServer;
        ThreadJobPtr m_threadJob;
        ThreadMutexInterfacePtr m_dataMutex;
        NodeDebuggerServerState m_serverState;
        Deque<NodeDebuggerPacket> m_incomingPackets;
        Deque<NodeDebuggerPacket> m_outgoingPackets;
        Vector<uint8_t> m_receivedData;
        VectorNodePath m_selectedNodePath;
    };
}