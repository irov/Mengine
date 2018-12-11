#pragma once
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <deque>

#include "zed_net.h"

// fwd-declaration
typedef struct GLFWwindow GLFWwindow;

namespace pugi { class xml_node; }

namespace mt
{
    struct vec2f
    {
        float x, y;

        inline const float* buff() const { return &x; }
        inline float* buff() { return &x; }

        inline bool operator !=( const vec2f & _other ) const
        {
            return abs( x - _other.x ) > 0.00001f ||
                   abs( y - _other.y ) > 0.00001f;
        }
    };

    struct vec3f
    {
        float x, y, z;

        inline const float* buff() const { return &x; }
        inline float* buff() { return &x; }

        inline bool operator !=( const vec3f & _other ) const
        {
            return abs( x - _other.x ) > 0.00001f ||
                   abs( y - _other.y ) > 0.00001f ||
                   abs( z - _other.z ) > 0.00001f;
        }
    };
}

struct Color
{
    union
    {
        struct { float r, g, b, a; };
        float arr[4];
    };

    Color( const uint32_t _argb = 0u )
    {
        const uint8_t b8 = (_argb >> 0) & 0xFF;
        const uint8_t g8 = (_argb >> 8) & 0xFF;
        const uint8_t r8 = (_argb >> 16) & 0xFF;
        const uint8_t a8 = (_argb >> 24) & 0xFF;

        r = static_cast<float>(r8) * 0.00392f;
        g = static_cast<float>(g8) * 0.00392f;
        b = static_cast<float>(b8) * 0.00392f;
        a = static_cast<float>(a8) * 0.00392f;
    }

    inline bool operator !=( const Color & _other ) const
    {
        return abs( r - _other.r ) > 0.00001f ||
               abs( g - _other.g ) > 0.00001f ||
               abs( b - _other.b ) > 0.00001f ||
               abs( a - _other.a ) > 0.00001f;
    }

    uint32_t getAsARGB() const
    {
        uint32_t r8 = static_cast<uint32_t>(r * 255.5f) & 0xFF;
        uint32_t g8 = static_cast<uint32_t>(g * 255.5f) & 0xFF;
        uint32_t b8 = static_cast<uint32_t>(b * 255.5f) & 0xFF;
        uint32_t a8 = static_cast<uint32_t>(a * 255.5f) & 0xFF;

        return (a8 << 24) | (r8 << 16) | (g8 << 8) | (b8 << 0);
    }
};

enum class ConnectionStatus : size_t
{
    Disconnected,
    ConnectionRequested,
    DisconnectionRequested,
    Connected,
    ConnectionFailed
};

struct NetPacket
{
    std::vector<uint8_t> payload;
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

struct Node
{
    uint32_t            uid;
    std::string         name;
    std::string         type;
    bool                enable;
    mt::vec3f           scale;
    mt::vec2f           skew;

    bool                hasRender;
    bool                hasAnimation;

    NodeRender          render;
    NodeAnimation       animation;

    std::vector<Node*>  children;
    Node*               parent;

    bool                dirty;

    void CheckIfChanged( const Node* _other )
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

    bool                    Initialize();
    void                    Loop();
    void                    Shutdown();

private:
    void                    Update();
    void                    ProcessPacket( const NetPacket& packet );
    void                    ReceiveScene( const pugi::xml_node& xmlContainer );
    void                    DeserializeNode( const pugi::xml_node& xmlNode, Node* node );
    std::vector<uint32_t>   CollectNodePath( const Node* node );
    std::string             PathToString( const std::vector<uint32_t>& path );

    // UI
    void                    DoUI();
    std::string             DoIPInput( const std::string& title, const std::string& inIP );
    void                    DoNodeElement( Node* node );
    void                    DoNodeProperties( Node* node );
    void                    OnConnectButton();
    void                    OnDisconnectButton();

    // network
    void                    NetworkLoop();
    void                    ConnectToServer();
    void                    DisconnectFromServer();
    void                    SendNetworkData();
    void                    ReceiveNetworkData();
    void                    SendChangedNode( const Node& node );

private:
    GLFWwindow*                 mWindow;
    bool                        mShutdown;
    int                         mWidth;
    int                         mHeight;

    // UI
    Node*                       mSelectedNode;

    // Server connection
    std::string                 mServerAddress;
    uint16_t                    mServerPort;
    std::string                 mServerAddressCopy;
    uint16_t                    mServerPortCopy;
    bool                        mServerConnectionEstablished;
    bool                        mServerForceReconnect;
    volatile ConnectionStatus   mConnectionStatus;
    zed_net_socket_t            mSocket;
    std::deque<NetPacket>       mIncomingPackets;
    std::deque<NetPacket>       mOutgoingPackets;
    std::vector<uint8_t>        mReceivedData;

    std::thread                 mNetworkThread;
    std::mutex                  mDataMutex;

    Node*                       mScene;
};
