#pragma once

#include "Config/Vector.h"
#include "Config/String.h"
#include "Config/Deque.h"

#include "Plugins/NodeDebuggerPlugin/NodeDebuggerSerialization.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define PUGIXML_NO_STL
#define PUGIXML_HEADER_ONLY
#include "pugixml.hpp"

#include "zed_net.h"

#include <thread>
#include <mutex>
#include <deque>

namespace Mengine
{
    enum class ConnectionStatus: uint32_t
    {
        Disconnected,
        ConnectionRequested,
        DisconnectionRequested,
        Connected,
        ConnectionFailed
    };

    struct NodeDebuggerPacket
    {
        Vector<uint8_t> payload;
    };

    

#define DESERIALIZE_PROP(NAME)\
    deserializeNodeProp<decltype(this->NAME)>( #NAME, _xmlNode, [this]( decltype(this->NAME) _value ) { this->NAME = _value; } )

#define SERIALIZE_PROP(NAME)\
    serializeNodeProp( this->NAME, #NAME, _xmlNode )

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
        bool    rendering;
        bool    hide;
        Color   color;

        void serialize( pugi::xml_node & _xmlNode ) const
        {
            SERIALIZE_PROP( hide );
            SERIALIZE_PROP( color );
        }

        void deserialize( const pugi::xml_node & _xmlNode )
        {
            DESERIALIZE_PROP( rendering );
            DESERIALIZE_PROP( hide );
            DESERIALIZE_PROP( color );
        }
    };

    struct NodeAnimation
    {
        bool loop;

        void serialize( pugi::xml_node & _xmlNode ) const
        {
            SERIALIZE_PROP( loop );
        }

        void deserialize( const pugi::xml_node & _xmlNode )
        {
            DESERIALIZE_PROP( loop );
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

        bool transformationProxy;
        NodeTransformation transformation;

        bool                    hasRender;
        NodeRender              render;

        bool                    hasAnimation;        
        NodeAnimation           animation;

        bool                    isTypeTextField;
        NodeTextField           textField;

        Vector<DebuggerNode*>   children;
        DebuggerNode*           parent;

        bool                    dirty;
        Icon                    icon;
    };

    class NodeDebuggerApp
    {
    public:
        NodeDebuggerApp();
        ~NodeDebuggerApp();

        bool                Initialize( const std::string& _address, const uint16_t _port );
        void                Loop();
        void                Shutdown();

    private:
        void                Resize( const int _width, const int _height );
        void                Update();
        void                ProcessPacket( const NodeDebuggerPacket & _packet );
        void                ReceiveScene( const pugi::xml_node & _xmlContainer );
        void                DeserializeNode( const pugi::xml_node & _xmlNode, DebuggerNode * _node );
        Vector<uint32_t>    CollectNodePath( const DebuggerNode * _node );
        String              PathToString( const Vector<uint32_t> & _path );
        void                DestroyNode( DebuggerNode * _node );

        // UI
        void                LoadIconsAtlas();
        void                DoUI();
        String              DoIPInput( const String & _title, const String & _inIP );
        void                DoNodeElement( DebuggerNode * _node );
        void                DoNodeProperties( DebuggerNode * _node );
        void                OnConnectButton();
        void                OnDisconnectButton();
        void                OnSelectNode( DebuggerNode * _node );

        // network
        void                NetworkLoop();
        void                ConnectToServer();
        void                DisconnectFromServer();
        void                SendNetworkData();
        void                ReceiveNetworkData();
        void                SendXML( const pugi::xml_document & _doc );
        void                SendChangedNode( const DebuggerNode & _node );
        void                SendNodeSelection( const String & _path );

    private:
        GLFWwindow*                 mWindow;
        bool                        mShutdown;
        int                         mWidth;
        int                         mHeight;

        // UI
        DebuggerNode*               mSelectedNode;
        uintptr_t                   mIconsAtlas;

        // Server connection
        String                      mServerAddress;
        uint16_t                    mServerPort;
        String                      mServerAddressCopy;
        uint16_t                    mServerPortCopy;
        volatile ConnectionStatus   mConnectionStatus;
        zed_net_socket_t            mSocket;
        Deque<NodeDebuggerPacket>   mIncomingPackets;
        Deque<NodeDebuggerPacket>   mOutgoingPackets;
        Vector<uint8_t>             mReceivedData;

        std::thread                 mNetworkThread;
        std::mutex                  mDataMutex;

        DebuggerNode*               mScene;
        String                      mSelectedNodePath;
    };
}