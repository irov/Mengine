#pragma once

#include "Config/Vector.h"
#include "Config/String.h"
#include "Config/Deque.h"

#include "Kernel/Color.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "math/vec2.h"
#include "math/vec3.h"

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

    struct NodeRender
    {
        bool    enable;
        bool    hide;
        Color   color;
    };

    struct NodeAnimation
    {
        bool loop;
    };

    struct DebuggerNode
    {
        enum : size_t
        {
            Icon_None       = 0,
            Icon_Render     = 1,
            Icon_Animation  = 2,
        };

        uint32_t                uid;
        String                  name;
        String                  type;
        bool                    enable;
        mt::vec3f               scale;
        mt::vec2f               skew;

        bool                    hasRender;
        bool                    hasAnimation;

        NodeRender              render;
        NodeAnimation           animation;

        Vector<DebuggerNode*>   children;
        DebuggerNode*           parent;

        bool                    dirty;
        size_t                  iconBits;

        void CheckIfChanged( const DebuggerNode* _other )
        {
            if( enable != _other->enable )
            {
                dirty = true;
            }

            // Render
            if( render.enable != _other->render.enable )
            {
                dirty = true;
            }
            if( render.hide != _other->render.hide )
            {
                dirty = true;
            }

            // Animation
            if( animation.loop != _other->animation.loop )
            {
                dirty = false;
            }
        }
    };

    class NodeDebuggerApp
    {
    public:
        NodeDebuggerApp();
        ~NodeDebuggerApp();

        bool                Initialize();
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