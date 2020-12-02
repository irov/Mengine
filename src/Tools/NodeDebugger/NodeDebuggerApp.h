#pragma once

#include "Kernel/Vector.h"
#include "Kernel/Map.h"
#include "Kernel/String.h"
#include "Kernel/VectorString.h"
#include "Kernel/Deque.h"
#include "Kernel/Blobject.h"
#include "Kernel/StringHelper.h"

#include "Plugins/NodeDebuggerPlugin/NodeDebuggerSerialization.h"

#include "jpp/jpp.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "zed_net.h"

#include <thread>
#include <mutex>
#include <deque>
#include <functional>

namespace Mengine
{
    enum class ConnectionStatus : uint32_t
    {
        Disconnected,
        ConnectionRequested,
        DisconnectionRequested,
        Connected,
        ConnectionFailed
    };

    struct NodeDebuggerPacket
    {
        Blobject payload;
    };

#define DESERIALIZE_PROP(NAME)\
    Detail::deserializeNodeProp<decltype(this->NAME)>( #NAME, _xmlNode, [this]( decltype(this->NAME) _value ) { this->NAME = _value; } )

#define SERIALIZE_PROP(NAME)\
    Detail::serializeNodeProp( this->NAME, #NAME, _xmlNode )

    struct NodeTransformation
    {
        mt::vec3f position;
        mt::vec3f origin;
        mt::vec2f skew;
        mt::vec3f scale;
        mt::vec3f orientation;
        mt::vec3f worldPosition;

        void serialize( pugi::xml_node & _xmlNode ) const
        {
            SERIALIZE_PROP( position );
            SERIALIZE_PROP( origin );
            SERIALIZE_PROP( skew );
            SERIALIZE_PROP( scale );
            SERIALIZE_PROP( orientation );
        }

        void deserialize( const pugi::xml_node & _xmlNode )
        {
            DESERIALIZE_PROP( position );
            DESERIALIZE_PROP( origin );
            DESERIALIZE_PROP( skew );
            DESERIALIZE_PROP( scale );
            DESERIALIZE_PROP( orientation );
            DESERIALIZE_PROP( worldPosition );
        }
    };

    struct NodeRender
    {
        bool    enable;
        bool    hide;
        int32_t z_index;
        int32_t z_order;
        Color   local_color;
        Color   personal_color;

        void serialize( pugi::xml_node & _xmlNode ) const
        {
            SERIALIZE_PROP( hide );
            SERIALIZE_PROP( z_index );
            SERIALIZE_PROP( z_order );
            SERIALIZE_PROP( local_color );
            SERIALIZE_PROP( personal_color );
        }

        void deserialize( const pugi::xml_node & _xmlNode )
        {
            DESERIALIZE_PROP( enable );
            DESERIALIZE_PROP( hide );
            DESERIALIZE_PROP( z_index );
            DESERIALIZE_PROP( z_order );
            DESERIALIZE_PROP( local_color );
            DESERIALIZE_PROP( personal_color );
        }
    };

    struct NodeAnimation
    {
        bool loop;
        bool play;
        bool pause;
        float duration;
        float time;

        void serialize( pugi::xml_node & _xmlNode ) const
        {
            SERIALIZE_PROP( loop );
            SERIALIZE_PROP( time );
        }

        void deserialize( const pugi::xml_node & _xmlNode )
        {
            DESERIALIZE_PROP( loop );
            DESERIALIZE_PROP( duration );
            DESERIALIZE_PROP( time );
        }
    };

    struct NodeContent
    {
        String FileGroup;
        String FilePath;
        String CodecType;
        String ConverterType;

        void serialize( pugi::xml_node & _xmlNode ) const
        {
            MENGINE_UNUSED( _xmlNode );
        }

        void deserialize( const pugi::xml_node & _xmlNode )
        {
            DESERIALIZE_PROP( FileGroup );
            DESERIALIZE_PROP( FilePath );
            DESERIALIZE_PROP( CodecType );
            DESERIALIZE_PROP( ConverterType );
        }
    };

    struct NodeSurfaceImage
    {
        String ResourceName;
        String ResourceType;

        bool isContent;
        NodeContent content;

        void serialize( pugi::xml_node & _xmlNode ) const
        {
            MENGINE_UNUSED( _xmlNode );
        }

        void deserialize( const pugi::xml_node & _xmlNode )
        {
            DESERIALIZE_PROP( ResourceName );
            DESERIALIZE_PROP( ResourceType );
        }
    };

    struct NodeComponentSurface
    {
        String Name;
        String Type;

        bool Compile;
        mt::vec2f MaxSize;
        mt::vec2f Size;
        mt::vec2f Offset;

        bool hasAnimation;
        NodeAnimation animation;

        bool isTypeSurfaceImage;
        NodeSurfaceImage surfaceImage;

        void serialize( pugi::xml_node & _xmlNode ) const
        {
            MENGINE_UNUSED( _xmlNode );
        }

        void deserialize( const pugi::xml_node & _xmlNode )
        {
            DESERIALIZE_PROP( Name );
            DESERIALIZE_PROP( Type );

            DESERIALIZE_PROP( Compile );

            DESERIALIZE_PROP( MaxSize );
            DESERIALIZE_PROP( Size );
            DESERIALIZE_PROP( Offset );
        }
    };

    struct NodeTextField
    {
        float MaxLength;

        bool Wrap;

        String TextID;
        String TextAliasEnvironment;

        bool HasText;
        String Format;
        String Text;

        String FontName;
        Color FontColor;
        float LineOffset;
        float CharOffset;
        float CharScale;
        uint32_t HorizontAlign;
        uint32_t VerticalAlign;

        String TotalFontName;
        Color TotalFontColor;
        float TotalLineOffset;
        float TotalCharOffset;
        float TotalCharScale;
        uint32_t TotalHorizontAlign;
        uint32_t TotalVerticalAlign;

        uint32_t MaxCharCount;

        bool Pixelsnap;

        void serialize( pugi::xml_node & _xmlNode ) const
        {
            SERIALIZE_PROP( MaxLength );
            SERIALIZE_PROP( Wrap );

            SERIALIZE_PROP( TextID );
            SERIALIZE_PROP( TextAliasEnvironment );

            SERIALIZE_PROP( FontName );
            SERIALIZE_PROP( FontColor );
            SERIALIZE_PROP( LineOffset );
            SERIALIZE_PROP( CharOffset );
            SERIALIZE_PROP( CharScale );
            SERIALIZE_PROP( HorizontAlign );
            SERIALIZE_PROP( VerticalAlign );

            SERIALIZE_PROP( MaxCharCount );
            SERIALIZE_PROP( Pixelsnap );
        }

        void deserialize( const pugi::xml_node & _xmlNode )
        {
            DESERIALIZE_PROP( MaxLength );
            DESERIALIZE_PROP( Wrap );

            DESERIALIZE_PROP( TextID );
            DESERIALIZE_PROP( TextAliasEnvironment );

            DESERIALIZE_PROP( HasText );
            DESERIALIZE_PROP( Format );
            DESERIALIZE_PROP( Text );

            DESERIALIZE_PROP( FontName );
            DESERIALIZE_PROP( FontColor );
            DESERIALIZE_PROP( LineOffset );
            DESERIALIZE_PROP( CharOffset );
            DESERIALIZE_PROP( CharScale );
            DESERIALIZE_PROP( HorizontAlign );
            DESERIALIZE_PROP( VerticalAlign );

            DESERIALIZE_PROP( TotalFontName );
            DESERIALIZE_PROP( TotalFontColor );
            DESERIALIZE_PROP( TotalLineOffset );
            DESERIALIZE_PROP( TotalCharOffset );
            DESERIALIZE_PROP( TotalCharScale );
            DESERIALIZE_PROP( TotalHorizontAlign );
            DESERIALIZE_PROP( TotalVerticalAlign );

            DESERIALIZE_PROP( MaxCharCount );
            DESERIALIZE_PROP( Pixelsnap );
        }
    };

    struct NodeMovie2
    {
        String CompositionName;
        String TextAliasEnvironment;

        void serialize( pugi::xml_node & _xmlNode ) const
        {
            SERIALIZE_PROP( TextAliasEnvironment );
        }

        void deserialize( const pugi::xml_node & _xmlNode )
        {
            DESERIALIZE_PROP( CompositionName );
            DESERIALIZE_PROP( TextAliasEnvironment );
        }
    };

    struct NodeSpine
    {
        String ResourceName;
        String ResourceType;

        bool isContent;
        NodeContent content;

        void serialize( pugi::xml_node & _xmlNode ) const
        {
            MENGINE_UNUSED( _xmlNode );
        }

        void deserialize( const pugi::xml_node & _xmlNode )
        {
            DESERIALIZE_PROP( ResourceName );
            DESERIALIZE_PROP( ResourceType );
        }
    };

    struct CachedImage
    {
        String      name;
        uintptr_t   image;
        size_t      width;
        size_t      height;
    };

    struct NodeIcon
    {
        String      name;
        uintptr_t   image;
        float       uv0_X;
        float       uv0_Y;
        float       uv1_X;
        float       uv1_Y;
    };

    struct DebuggerNode
    {
        enum class Icon : size_t
        {
            Icon_Unknown    = 0,
            Icon_Movie2     = 1,
            Icon_Scene      = 2,
            Icon_Layer2D    = 3,
            Icon_Entity     = 4,
        };

        uint32_t                uid;
        String                  name;
        String                  type;
        bool                    enable;

        bool                    transformationProxy;
        NodeTransformation      transformation;

        bool                    hasRender;
        NodeRender              render;

        bool                    hasAnimation;
        NodeAnimation           animation;

        bool                    hasComponentSurface;
        NodeComponentSurface    componentSurface;

        bool                    isTypeTextField;
        NodeTextField           textField;

        bool                    isTypeMovie2;
        NodeMovie2              movie2;

        bool                    isTypeSpine;
        NodeSpine               spine;

        Vector<DebuggerNode*>   children;
        DebuggerNode*           parent;

        bool                    dirty;
        const NodeIcon*         icon;

        void serialize( pugi::xml_node & _xmlNode ) const
        {
            SERIALIZE_PROP( enable );
        }

        void deserialize( const pugi::xml_node & _xmlNode )
        {
            DESERIALIZE_PROP( uid );
            DESERIALIZE_PROP( name );
            DESERIALIZE_PROP( type );
            DESERIALIZE_PROP( enable );
        }
    };

    struct TabDescriptor
    {
        using TabFunctor = std::function<void()>;

        String      name;
        String      title;
        bool        enabled;
        TabFunctor  functor;
    };

    struct SettingDesc
    {
        String name;
        String file;

        mutable jpp::object json;
    };

    class NodeDebuggerApp
    {
    public:
        NodeDebuggerApp();
        ~NodeDebuggerApp();

        bool Initialize( const String & _address, const uint16_t _port );
        void Loop();
        void Shutdown();

    protected:
        void Resize( const int _width, const int _height );
        void Update( const double _dt );
        void CompressPacket( NodeDebuggerPacket & _packet, PacketHeader & _hdr );
        void UncompressPacket( NodeDebuggerPacket & _packet, PacketHeader & _hdr, const uint8_t * _receivedData );
        void ProcessPacket( const NodeDebuggerPacket & _packet );
        void ReceiveArrow( const pugi::xml_node & _xmlContainer );
        void ReceiveScene( const pugi::xml_node & _xmlContainer );
        void ReceivePickerable( const pugi::xml_node & _xmlContainer );
        void ReceiveRenderable( const pugi::xml_node & _xmlContainer );
        void ReceiveMemory( const pugi::xml_node & _xmlContainer );
        void ReceiveObjectsLeak( const pugi::xml_node & _xmlContainer );
        void ReceiveNetwork( const pugi::xml_node & _xmlContainer );
        void ReceiveSettings( const pugi::xml_node & _xmlContainer );

    protected:
        void DeserializeNode( const pugi::xml_node & _xmlNode, DebuggerNode * _node );
        Vector<uint32_t> CollectNodePath( const DebuggerNode * _node );
        String PathToString( const Vector<uint32_t> & _path );
        Vector<uint32_t> StringToPath( String & _pathStr );
        DebuggerNode * PathToNode( const Vector<uint32_t> & _path );
        void DestroyNode( DebuggerNode * _node );

        // UI
        const CachedImage * GetIconImage( const String & _name );
        void LoadIconsAtlas();
        const NodeIcon * GetIconForNodeType( const String & _nodeType );
        void DoUI();
        void DoUISceneDebuggerTab();
        void DoUIMemoryTab();
        void DoUIObjectsLeakTab();
        void DoUINetwork();
        void DoUISettingsTab();
        String DoIPInput( const String & _title, const String & _inIP );
        void DoNodeElement( DebuggerNode * _node, const String & _tag );
        void DoNodeProperties( DebuggerNode * _node );
        void OnConnectButton();
        void OnDisconnectButton();
        void OnSelectNode( DebuggerNode * _node );
        void OnPauseButton();

        // network
        void NetworkLoop();
        void ConnectToServer();
        void DisconnectFromServer();
        void SendNetworkData();
        void ReceiveNetworkData();
        void SendXML( const pugi::xml_document & _doc );
        void SendChangedTab( const String & _tab );
        void SendChangedNode( const DebuggerNode & _node );
        void SendNodeSelection( const String & _path );

        void SendGameControlCommand( const String & _command );
        void SendSceneRequest();
        void SendPauseRequest();

        void ShowResponseDataForId( uint32_t _id );
        void addSpacesWithMultiplier( String * _out, int _spacesCount, int _multiplier );
        void ShowResponseJpp( const jpp::object & _object, int _spaceCounter );
        void GetValueStringForJppType( const jpp::object & _object, jpp::e_type _jppType, String * _out );

    private:
        GLFWwindow * m_window;
        bool m_shutdown;
        int m_width;
        int m_height;

        // UI
        DebuggerNode * m_selectedNode;
        const NodeIcon * m_defaultIcon;
        Vector<NodeIcon> m_icons;
        Vector<CachedImage> m_imagesCache;
        Vector<TabDescriptor> m_tabs;
        Vector<SettingDesc> m_settings;

        struct LeakDesc
        {
            String file;
            String function;
            String line;
            String message;

            typedef Vector<LeakDesc> VectorLeakDesc;
            VectorLeakDesc parents;
        };

        typedef Vector<LeakDesc> VectorLeaks;
        Map<String, VectorLeaks> m_objectLeaks;
        uint32_t m_memoryTotal;
        uint32_t m_AvailableTextureMemory;
        uint32_t m_TextureMemoryUse;
        uint32_t m_TextureCount;
        uint32_t m_SoundSourcesCount;
        uint32_t m_SoundBuffersCount;
        Map<String, uint32_t> m_memory;
        String m_objectLeakGeneration;
        size_t m_currentTab;

        // Server connection
        String m_serverAddress;
        uint16_t m_serverPort;
        String m_serverAddressCopy;
        uint16_t m_serverPortCopy;
        volatile ConnectionStatus m_connectionStatus;
        zed_net_socket_t m_socket;
        Deque<NodeDebuggerPacket> m_incomingPackets;
        Deque<NodeDebuggerPacket> m_outgoingPackets;
        Mengine::Blobject m_receivedData;

        std::thread m_networkThread;
        std::mutex m_dataMutex;

        String m_selectedTab;
        String m_cacheSelectedTab;
        bool m_invalidateSelectedTab;

        DebuggerNode * m_arrow;
        DebuggerNode * m_scene;
        DebuggerNode * m_scenePickerable;
        DebuggerNode * m_sceneRenderable;
        String m_selectedNodePath;
        String m_lastSelectedNodePath;

        int m_sceneUpdateFreq;
        double m_sceneUpdateTimer;
        bool m_updateSceneOnChange;
        bool m_pauseRequested;

        String m_selectedSetting;

        struct NetworkDesk
        {
            String type;
            uint32_t id;
            String url;
        };
        typedef Vector<NetworkDesk> VectorNetwork;

        VectorNetwork m_network;
        uint32_t m_networkTextLabelCounter;
    };
}
