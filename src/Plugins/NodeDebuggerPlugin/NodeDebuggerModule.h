#pragma once

#include "NodeDebuggerBoundingBoxInterface.h"

#include "Engine/TextField.h"

#include "Kernel/Vector.h"
#include "Kernel/Map.h"
#include "Kernel/Deque.h"
#include "Kernel/ModuleBase.h"
#include "Kernel/NodeUniqueFinder.h"
#include "Kernel/Scene.h"

#include "Interface/SocketInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ArchivatorInterface.h"

#include "Plugins/MoviePlugin/UnknownMovie2Interface.h"

#include "NodeDebuggerSerialization.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum class ENodeDebuggerServerState : uint32_t
    {
        Invalid,
        WaitingForClient,
        Connected
    };
    //////////////////////////////////////////////////////////////////////////
    enum class ENodeDebuggerPacketType : uint32_t
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
    class NodeDebuggerModule
        : public ModuleBase
        , public ThreadWorkerInterface
    {
    public:
        NodeDebuggerModule();
        ~NodeDebuggerModule() override;

    protected:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        bool _availableModule() const override;

    public:
        void onUpdate( uint32_t _id ) override;
        bool onWork( uint32_t _id ) override;
        void onDone( uint32_t _id ) override;

    public:
        void setScene( const ScenePtr & _scene );
        void updateScene();

    public:
        void _update( bool _focus ) override;
        void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) override;

    protected:
        bool privateInit();
        void recreateServer();
        void compressPacket( NodeDebuggerPacket & _packet, PacketHeader & _hdr );
        void uncompressPacket( NodeDebuggerPacket & _packet, PacketHeader & _hdr, const uint8_t * _receivedData );
        void sendPacket( NodeDebuggerPacket & _packet );
        void sendScene( const ScenePtr & _scene );
        void sendPickerable( const ScenePtr & _scene );
        void sendRenderable( const ScenePtr & _scene );
        void sendSettings();
        void sendObjectsLeak();

        void serializeNode( const NodePtr & _node, pugi::xml_node & _xmlParentNode );
        void serializeNodeSingle( const NodePtr & _node, pugi::xml_node & _xmlNode );
        void serializePickerable( PickerInterface * _picker, pugi::xml_node & _xmlParentNode );
        void serializeRenderable( RenderInterface * _render, pugi::xml_node & _xmlParentNode );

        void serializeTransformation( const TransformationPtr & _transformation, pugi::xml_node & _xmlParentNode );
        void serializeRender( const RenderInterface * _render, pugi::xml_node & _xmlParentNode );
        void serializeAnimation( const AnimationInterface * _animation, pugi::xml_node & _xmlParentNode );
        void serializeTextField( const TextFieldPtr & _textField, pugi::xml_node & _xmlParentNode );
        void serializeMovie2( UnknownMovie2Interface * _unknownMovie2, pugi::xml_node & _xmlParentNode );
        void processPacket( NodeDebuggerPacket & _packet );
        void receiveChangedNode( const pugi::xml_node & _xmlNode );
        void receiveGameControlCommand( const String & _command );
        void stringToPath( const String & _str, VectorNodePath & _path ) const;

    protected:
        void notifyChangeScene( const ScenePtr & _scene );
        void notifyRemoveSceneDestroy();
        void notifyIncrefFactoryGeneration( uint32_t _generator );

    protected:
        uint32_t m_globalKeyHandlerF2;

        ScenePtr m_scene;
        SocketInterfacePtr m_socket;
        AtomicBool m_shouldRecreateServer;
        AtomicBool m_shouldUpdateScene;
        ThreadJobPtr m_threadJob;
        uint32_t m_workerId;
        ThreadMutexInterfacePtr m_dataMutex;
        ENodeDebuggerServerState m_serverState;
        Deque<NodeDebuggerPacket> m_incomingPackets;
        Deque<NodeDebuggerPacket> m_outgoingPackets;
        Vector<uint8_t> m_receivedData;
        VectorNodePath m_selectedNodePath;
        ArchivatorInterfacePtr m_archivator;
    };
}