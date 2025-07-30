#pragma once

#include "Interface/SocketInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/DebuggerBoundingBoxInterface.h"
#include "Interface/HttpSystemInterface.h"

#include "Plugins/MoviePlugin/Movie2Interface.h"
#include "Plugins/SpinePlugin/SpineInterface.h"

#include "Engine/TextField.h"

#include "Kernel/SurfaceImage.h"
#include "Kernel/SurfaceImageSequence.h"
#include "Kernel/Vector.h"
#include "Kernel/Map.h"
#include "Kernel/Deque.h"
#include "Kernel/ModuleBase.h"
#include "Kernel/NodeUniqueFinder.h"
#include "Kernel/Shape.h"
#include "Kernel/Scene.h"
#include "Kernel/ResourceImageSequence.h"
#include "Kernel/List.h"

#include "NodeDebuggerSerialization.h"
#include "SceneDataProviderInterface.h"

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
        Data payload;
    };
    //////////////////////////////////////////////////////////////////////////
    struct NodeDebuggerRequestData
    {
        ConstString type;
        HttpRequestId id;
        String url;
    };
    //////////////////////////////////////////////////////////////////////////
    class ModuleNodeDebugger
        : public ModuleBase
        , public ThreadWorkerInterface
        , public SceneDataProviderInterface
    {
        DECLARE_FACTORABLE( ModuleNodeDebugger );

    public:
        ModuleNodeDebugger();
        ~ModuleNodeDebugger() override;

    protected:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        bool _availableModule() const override;

    public:
        void onThreadWorkerUpdate( UniqueId _id ) override;
        bool onThreadWorkerWork( UniqueId _id ) override;
        void onThreadWorkerDone( UniqueId _id ) override;

    public:
        void setScene( const ScenePtr & _scene );
        void updateScene();

    public:
        void _beginUpdate( bool _focus ) override;
        void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) override;

    protected:
        bool privateInit();
        void recreateServer();
        void compressPacket( NodeDebuggerPacket & _packet, PacketHeader & _hdr );
        void uncompressPacket( NodeDebuggerPacket & _packet, PacketHeader & _hdr, const uint8_t * _receivedData );
        void sendPacket( NodeDebuggerPacket & _packet );
        void sendArrow( const NodePtr & _arrow );
        void sendScene( const ScenePtr & _scene );
        void sendPickerable( const ScenePtr & _scene );
        void sendRenderable( const ScenePtr & _scene );
        void sendSounds();
        void sendSettings();
        void sendMemory();
        void sendObjectsLeak();
        void sendNetwork();
        void sendSelectedNode();

    protected:
        void serializeNode( const NodePtr & _node, pugi::xml_node & _xmlParentNode );
        void serializeNodeSingle( const NodePtr & _node, pugi::xml_node & _xmlNode );
        void serializePickerable( PickerInterface * _picker, pugi::xml_node & _xmlParentNode );
        void serializeRenderable( RenderInterface * _render, pugi::xml_node & _xmlParentNode );
        void serializeTransformation( const TransformablePtr & _transformable, pugi::xml_node & _xmlParentNode );
        void serializeRender( const RenderInterface * _render, pugi::xml_node & _xmlParentNode );
        void serializeAnimation( const Compilable * _compilable, const AnimationInterface * _animation, pugi::xml_node & _xmlParentNode );
        void serializeTextField( const TextFieldPtr & _textField, pugi::xml_node & _xmlParentNode );
        void serializeMovie2( const Compilable * _compilable, const UnknownMovie2Interface * _unknownMovie2, pugi::xml_node & _xmlParentNode );
        void serializeSpine( const UnknownSpineInterface * _unknownSpine, pugi::xml_node & _xmlParentNode );
        void serializeShape( const ShapePtr & _shape, pugi::xml_node & _xmlParentNode );
        void serializeSurfaceImage( const SurfaceImagePtr & _surfaceImage, pugi::xml_node & _xmlParentNode );
        void serializeSurfaceImageSequence( const SurfaceImageSequencePtr & _surfaceImageSequence, pugi::xml_node & _xmlParentNode );
        void serializeContent( const ContentInterfacePtr & _content, pugi::xml_node & _xmlParentNode );

    protected:
        void processPacket( NodeDebuggerPacket & _packet );
        void receiveChangedNode( const pugi::xml_node & _xmlNode );
        void receiveGameControlCommand( const String & _command );
        void receiveResolutins( uint32_t _width, uint32_t _height );
        void receiveSetting( const Char * _setting, const Char * _key, const Char * _value );
        void stringToPath( const String & _str, VectorNodePath * const _path ) const;
        void pathToString( const VectorNodePath & _path, String * const _outStr ) const;

    protected:
        void notifyChangeSceneComplete( const ScenePtr & _scene );
        void notifyChangeSceneDestroy( const ScenePtr & _scene );
        void notifyRemoveSceneDestroy();
        void notifyIncrefFactoryGeneration( uint32_t _generator );
        void notifyHttpRequest( HttpRequestId _id, const String & _url );
        void notifyHttpResponse( const HttpResponseInterfacePtr & _response );

    public:
        typedef Lambda<void( const NodeDebuggerRequestData & data )> LambdaNodeDebuggerRequestData;
        void foreachRequestData( const LambdaNodeDebuggerRequestData & _lambda );

    protected:
        void clearRequestDatas_();

    public:
        void setUpdateSceneFlag( bool _flag ) override;

    protected:
        void findChildRecursive( const NodePtr & _currentNode, const mt::vec2f & _point );
        bool checkHit( const ShapePtr & _currentNode, const mt::vec2f & _point );
        bool checkIsTransparencePoint( const ShapePtr & _currentNode
            , const mt::vec2f & _point
            , const RenderImageLoaderInterfacePtr & _imageLoader
            , const RenderTextureInterfacePtr & _renderTexture
            , const RenderImageDesc & _imageDesc
            , const mt::uv4f & uv );

        void getScreenBoundingBox( const ShapePtr & _node, const RenderImageDesc & _imageDesc, mt::box2f * const _bb ) const;
        void getWorldBoundingBox( const ShapePtr & _node, const RenderImageDesc & _imageDesc, mt::box2f * const _bb ) const;

    protected:
        template<typename T>
        bool deserializeNodeProp( const Char * _propName, const pugi::xml_node & _xmlParentNode, const Lambda<void( const T & )> & _lambda )
        { 
            if( Detail::deserializeNodeProp( _propName, _xmlParentNode, _lambda ) == false )
            {
                return false;
            }

            m_shouldUpdateScene = true;

            return true;
        }

    protected:
        UniqueId m_globalKeyHandlerF2;
        UniqueId m_globalKeyHandlerForSendingSelectedNode;

        ScenePtr m_scene;
        NodePtr m_selectedNode;

        SocketInterfacePtr m_socket;
        AtomicBool m_shouldRecreateServer;
        AtomicBool m_shouldUpdateScene;
        ThreadJobPtr m_threadJob;
        UniqueId m_workerId;
        ThreadMutexInterfacePtr m_dataMutex;
        Atomic<ENodeDebuggerServerState> m_serverState;
        Deque<NodeDebuggerPacket> m_incomingPackets;
        Deque<NodeDebuggerPacket> m_outgoingPackets;
        Data m_receivedData;
        Data m_receivedDataAux;
        VectorNodePath m_selectedNodePath;
        ArchivatorInterfacePtr m_archivator;
        String m_currentTab;

        typedef List<NodeDebuggerRequestData> ListNodeDebuggerRequestData;
        ListNodeDebuggerRequestData m_requestDatas;

        mt::vec2f m_cursorAdaptScreenPosition;
        mt::vec2f m_cursorWorldPosition;
    };
}