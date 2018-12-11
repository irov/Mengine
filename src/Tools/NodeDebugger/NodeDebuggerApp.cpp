#include "NodeDebuggerApp.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui_impl_glfw_gl3_glad.h"

#define ZED_NET_IMPLEMENTATION
#include "zed_net.h"

#define PUGIXML_HEADER_ONLY
#include "pugixml.hpp"

#include <chrono>
#include <sstream>
#include <iterator>
#include <algorithm>

#include "Plugins/NodeDebuggerPlugin/NodePropsSerializaton.h"

#define MutexLocker std::lock_guard<std::mutex>


static bool zed_net_ext_tcp_wait_for_data( zed_net_socket_t* _socket, const int _timeoutMs )
{
    fd_set socketsSet;
    socketsSet.fd_count = 1;
    socketsSet.fd_array[0] = _socket->handle;

    timeval tv;
    tv.tv_sec = static_cast<long>(_timeoutMs / 1000);
    tv.tv_usec = static_cast<long>((_timeoutMs - static_cast<int>(tv.tv_sec * 1000)) * 1000);

    const int result = ::select( 0, &socketsSet, nullptr, nullptr, (_timeoutMs == 0) ? nullptr : &tv );

    return (1 == result);
}



NodeDebuggerApp::NodeDebuggerApp()
    : mWindow( nullptr )
    , mShutdown( false )
    , mWidth( 1280 )
    , mHeight( 720 )
    , mSelectedNode( nullptr )
    , mServerAddress()
    , mServerPort( 18790 )
    , mServerAddressCopy()
    , mServerPortCopy( 0 )
    , mServerConnectionEstablished( false )
    , mServerForceReconnect( false )
    , mConnectionStatus( ConnectionStatus::Disconnected )
    , mScene( nullptr )
{
}
NodeDebuggerApp::~NodeDebuggerApp()
{
}

bool NodeDebuggerApp::Initialize()
{
    if( 0 != zed_net_init() )
    {
        return false;
    }

    if( GL_FALSE == glfwInit() )
    {
        return false;
    }

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

    mWindow = glfwCreateWindow( mWidth, mHeight, "Node Debugger", nullptr, nullptr );

    if( mWindow == nullptr )
    {
        glfwTerminate();

        return false;
    }

    glfwSetWindowUserPointer( mWindow, this );

    glfwMakeContextCurrent( mWindow );
    gladLoadGLLoader( reinterpret_cast<GLADloadproc>( glfwGetProcAddress ) );
    glfwSwapInterval( 1 ); // enable v-sync

    glViewport( 0, 0, mWidth, mHeight );

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glfwSetScrollCallback( mWindow, ImGui_ImplGlfw_ScrollCallback );
    glfwSetCharCallback( mWindow, ImGui_ImplGlfw_CharCallback );
    glfwSetKeyCallback( mWindow, ImGui_ImplGlfw_KeyCallback );
    glfwSetMouseButtonCallback( mWindow, ImGui_ImplGlfw_MouseButtonCallback );

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::GetIO().IniFilename = nullptr; // disable "imgui.ini"
    //ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; // tell ImGui to not interfere with our cursors
    ImGui_ImplGlfwGL3_Init( mWindow, false );
    ImGui::StyleColorsClassic();

    mShutdown = false;
    mNetworkThread = std::thread(&NodeDebuggerApp::NetworkLoop, this);

    return true;
}

void NodeDebuggerApp::Loop()
{
    while( GL_FALSE == glfwWindowShouldClose( mWindow ) )
    {
        glfwPollEvents();

        Update();

        glClearColor( 0.412f, 0.796f, 1.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT );

        ImGui_ImplGlfwGL3_NewFrame();

        DoUI();

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData( ImGui::GetDrawData() );

        glfwSwapBuffers( mWindow );
    }
}

void NodeDebuggerApp::Shutdown()
{
    mShutdown = true;

    glfwTerminate();
    zed_net_shutdown();

    mNetworkThread.join();
}

void NodeDebuggerApp::Update()
{
    MutexLocker lock( mDataMutex );

    if( !mIncomingPackets.empty() )
    {
        ProcessPacket( mIncomingPackets.front() );
        mIncomingPackets.pop_front();
    }

    if( mSelectedNode && mSelectedNode->dirty )
    {
        SendChangedNode( *mSelectedNode );
        mSelectedNode->dirty = false;
    }
}

void NodeDebuggerApp::ProcessPacket( const NodeDebuggerPacket & _packet )
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer( _packet.payload.data(), _packet.payload.size() );
    if( !result )
    {
        return;
    }

    pugi::xml_node packetNode = doc.child( "Packet" );
    if( !packetNode )
    {
        return;
    }

    pugi::xml_attribute typeAttrib = packetNode.attribute( "type" );
    if( !typeAttrib )
    {
        return;
    }

    // type name to enum
    std::string typeStr = typeAttrib.value();

    pugi::xml_node payloadNode = packetNode.child( "Payload" );
    if( !payloadNode )
    {
        return;
    }

    if( typeStr == "Scene" )
    {
        ReceiveScene( payloadNode );
    }
}

void NodeDebuggerApp::ReceiveScene( const pugi::xml_node & _xmlContainer )
{
    pugi::xml_node xmlNode = _xmlContainer.first_child();

    if( xmlNode )
    {
        if( mScene )
        {
            delete mScene;
        }

        mScene = new Node();
        mScene->parent = nullptr;

        DeserializeNode( xmlNode, mScene );

        mSelectedNode = nullptr;
    }
}

void NodeDebuggerApp::DeserializeNode( const pugi::xml_node& _xmlNode, Node* _node )
{
    _node->uid = _xmlNode.attribute( "uid" ).as_uint();
    _node->name = _xmlNode.attribute( "name" ).value();
    _node->type = _xmlNode.attribute( "type" ).value();

    deserializeNodeProp<bool>( *_node, "enable", _xmlNode, []( Node & _node, auto _value )
    {
        _node.enable = _value;
    } );

    deserializeNodeProp<mt::vec3f>( *_node, "scale", _xmlNode, []( Node & _node, auto _value )
    {
        _node.scale = _value;
    } );

    deserializeNodeProp<mt::vec2f>( *_node, "skew", _xmlNode, []( Node & _node, auto _value )
    {
        _node.skew = _value;
    } );

    pugi::xml_node renderNode = _xmlNode.child( "Render" );
    pugi::xml_node animationNode = _xmlNode.child( "Animation" );
    pugi::xml_node childrenNode = _xmlNode.child( "Children" );

    _node->hasRender = renderNode;
    _node->hasAnimation = animationNode;

    if( _node->hasRender )
    {
        deserializeNodeProp<bool>( *_node, "enable", renderNode, []( Node & _node, auto _value )
        {
            _node.render.enable = _value;
        } );

        deserializeNodeProp<bool>( *_node, "hide", renderNode, []( Node & _node, auto _value )
        {
            _node.render.hide = _value;
        } );

        deserializeNodeProp<Color>( *_node, "color", renderNode, []( Node & _node, auto _value )
        {
            _node.render.color = _value;
        } );
    }

    if( _node->hasAnimation )
    {
        deserializeNodeProp<bool>( *_node, "loop", animationNode, []( Node & _node, auto _value )
        {
            _node.animation.loop = _value;
        } );
    }

    if( childrenNode )
    {
        for( const auto & child : childrenNode.children() )
        {
            Node* childNode = new Node();
            childNode->parent = _node;
            DeserializeNode( child, childNode );
            _node->children.push_back( childNode );
        }
    }

    _node->dirty = false;
}

std::vector<uint32_t> NodeDebuggerApp::CollectNodePath( const Node * _node )
{
    std::vector<uint32_t> path;

    const Node* current = _node;
    while( current && current->parent )
    {
        path.insert( path.begin(), current->uid );
        current = current->parent;
    }

    return path;
}

std::string NodeDebuggerApp::PathToString( const std::vector<uint32_t> & _path )
{
    std::stringstream stream;
    std::copy( _path.begin(), _path.end(), std::ostream_iterator<uint32_t>( stream, "/" ) );

    return stream.str();
}

void NodeDebuggerApp::DoUI()
{
    const ImGuiWindowFlags kPanelFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

    const float leftPanelWidth = 400.0f;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(mWidth), static_cast<float>(mHeight)), ImGuiCond_FirstUseEver);

    if( ImGui::Begin( "Node Debugger", nullptr, kPanelFlags ) )
    {
        ImGui::Columns( 2, nullptr, true );
        ImGui::SetColumnWidth( 0, leftPanelWidth );

        if( ImGui::CollapsingHeader( "Server:", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            char serverAddress[256] = { 0 };

            if( !mServerAddress.empty() )
            {
                std::copy( mServerAddress.begin(), mServerAddress.end(), serverAddress );
            }

            mServerAddress = DoIPInput( "Address:", mServerAddress );

            ImGui::BeginGroup();
            {
                ImGui::AlignFirstTextHeightToWidgets();
                ImGui::TextUnformatted( "IP Port:" );
                ImGui::SameLine();

                const float width = ImGui::CalcItemWidth();
                ImGui::PushItemWidth( width * 0.25f );
                int port = static_cast<int>( mServerPort );
                if( ImGui::InputInt( "##Port", &port, 0, 0, ImGuiInputTextFlags_CharsDecimal ) )
                {
                    mServerPort = static_cast<uint16_t>( port & 0xFFFF );
                }
                ImGui::PopItemWidth();
            }
            ImGui::EndGroup();

            if( mConnectionStatus == ConnectionStatus::Connected )
            {
                const ImVec4 redButtonColor( 0.5f, 0.f, 0.f, 1.f );
                ImGui::PushStyleColor( ImGuiCol_Button, redButtonColor );
                ImGui::PushStyleColor( ImGuiCol_ButtonHovered, redButtonColor );
                if( ImGui::Button( "Disconnect" ) )
                {
                    OnDisconnectButton();
                }
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }
            else if( mConnectionStatus == ConnectionStatus::Disconnected || mConnectionStatus == ConnectionStatus::ConnectionFailed )
            {
                const ImVec4 greenButtonColor( 0.1686f, 0.5686f, 0.f, 1.f );
                ImGui::PushStyleColor( ImGuiCol_Button, greenButtonColor );
                ImGui::PushStyleColor( ImGuiCol_ButtonHovered, greenButtonColor );
                if( ImGui::Button( "Connect" ) )
                {
                    OnConnectButton();
                }
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }
        }

        if( ImGui::CollapsingHeader( "Scene:", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            if( mScene )
            {
                if( ImGui::BeginChild( "SceneTree", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar ) )
                {
                    DoNodeElement( mScene );
                }
                ImGui::EndChild();
            }
        }

        ImGui::NextColumn();
        if( ImGui::BeginChild( "Panel" ) )
        {
            if( mSelectedNode )
            {
                DoNodeProperties( mSelectedNode );
            }
        }
        ImGui::EndChild();

        ImGui::Columns( 1 );
    }
    ImGui::End();
}

std::string NodeDebuggerApp::DoIPInput( const std::string & _title, const std::string & _inIP )
{
    int octets[4] = { 127, 0, 0, 1 };

    if( !_inIP.empty() )
    {
        int idx = 0;
        octets[idx] = 0;

        for( const char c : _inIP )
        {
            if( isdigit( c ) )
            {
                octets[idx] *= 10;
                octets[idx] += static_cast<int>( c - '0' );
            }
            else
            {
                ++idx;
                if( idx > 3 ) // in case of a weird string
                {
                    break;
                }
                octets[idx] = 0;
            }
        }
    }

    const float width = ImGui::CalcItemWidth();
    ImGui::BeginGroup();
    ImGui::AlignFirstTextHeightToWidgets();
    ImGui::PushID( "IPInputForm" );
    ImGui::TextUnformatted( _title.c_str() );
    for( int i = 0; i < 4; ++i )
    {
        ImGui::SameLine();
        ImGui::PushItemWidth( width * 0.25f );
        ImGui::PushID( i );
        bool invalidOctet = false;
        if( octets[i] > 255 )
        {
            // Make values over 255 red, and when focus is lost reset it to 255.
            octets[i] = 255;
            invalidOctet = true;
            ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        }
        if( octets[i] < 0 )
        {
            // Make values below 0 yellow, and when focus is lost reset it to 0.
            octets[i] = 0;
            invalidOctet = true;
            ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 1.0f, 0.0f, 1.0f ) );
        }
        ImGui::InputInt( "##v", &octets[i], 0, 0, ImGuiInputTextFlags_CharsDecimal );
        if( invalidOctet )
        {
            ImGui::PopStyleColor();
        }
        ImGui::PopID();
        ImGui::PopItemWidth();
    }
    ImGui::PopID();
    ImGui::EndGroup();

    std::string result;
    result = std::to_string( octets[0] ) + '.' +
             std::to_string( octets[1] ) + '.' +
             std::to_string( octets[2] ) + '.' +
             std::to_string( octets[3] );

    return result;
}

void NodeDebuggerApp::DoNodeElement( Node * _node )
{
    const ImGuiTreeNodeFlags seletedFlag = (mSelectedNode == _node ) ? ImGuiTreeNodeFlags_Selected : 0;
    const ImGuiTreeNodeFlags flagsNormal = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | seletedFlag;
    const ImGuiTreeNodeFlags flagsNoChildren = ImGuiTreeNodeFlags_Leaf | seletedFlag;

    const bool nodeOpen = ImGui::TreeNodeEx( _node->name.c_str(), _node->children.empty() ? flagsNoChildren : flagsNormal );
    if( ImGui::IsItemClicked() )
    {
        mSelectedNode = _node;
    }

    if( nodeOpen )
    {
        for( Node* child : _node->children )
        {
            DoNodeElement( child );
        }

        ImGui::TreePop();
    }
}

void NodeDebuggerApp::DoNodeProperties( Node * _node )
{
    auto uiEditorBool = [_node]( const char * _caption, bool & _prop )
    {
        bool testValue = _prop;
        const bool input = ImGui::Checkbox( _caption, &testValue );
        if( input && testValue != _prop )
        {
            _prop = testValue;
            _node->dirty = true;
        }
    };

    auto uiEditorVec3f = [_node]( const char * _caption, mt::vec3f & _prop )
    {
        mt::vec3f testValue = _prop;
        const bool input = ImGui::DragFloat3( _caption, testValue.buff() );
        if( input && testValue != _prop )
        {
            _prop = testValue;
            _node->dirty = true;
        }
    };

    auto uiEditorVec2f = [_node]( const char * _caption, mt::vec2f & _prop )
    {
        mt::vec2f testValue = _prop;
        const bool input = ImGui::DragFloat2( _caption, testValue.buff() );
        if( input && testValue != _prop )
        {
            _prop = testValue;
            _node->dirty = true;
        }
    };

    auto uiEditorColor = [_node]( const char * _caption, Color & _prop )
    {
        Color testValue = _prop;
        const bool input = ImGui::ColorEdit4( _caption, testValue.arr );
        if( input && testValue != _prop )
        {
            _prop = testValue;
            _node->dirty = true;
        }
    };


    if( ImGui::CollapsingHeader( "Node:", ImGuiTreeNodeFlags_DefaultOpen ) )
    {
        ImGui::Text( "Name: %s", _node->name.c_str() );
        ImGui::SameLine();
        ImGui::Dummy( ImVec2( 20.0f, 5.0f ) );
        ImGui::SameLine();
        ImGui::Text( "Type: %s", _node->type.c_str() );
        ImGui::Spacing();

        uiEditorVec3f( "Scale", _node->scale );
        ImGui::Spacing();

        uiEditorVec2f( "Skew", _node->skew );
    }

    if( _node->hasRender && ImGui::CollapsingHeader( "Render:", ImGuiTreeNodeFlags_DefaultOpen ) )
    {
        uiEditorBool( "Enable", _node->render.enable );
        ImGui::Spacing();

        uiEditorBool( "Hide", _node->render.hide );
        ImGui::Spacing();

        uiEditorColor( "Color", _node->render.color );
    }

    if( _node->hasAnimation && ImGui::CollapsingHeader( "Animation:", ImGuiTreeNodeFlags_DefaultOpen ) )
    {
        uiEditorBool( "Loop", _node->animation.loop );
    }
}

void NodeDebuggerApp::OnConnectButton()
{
    mServerAddressCopy = mServerAddress;
    mServerPortCopy = mServerPort;

    mConnectionStatus = ConnectionStatus::ConnectionRequested;
}

void NodeDebuggerApp::OnDisconnectButton()
{
    mConnectionStatus = ConnectionStatus::DisconnectionRequested;
}


void NodeDebuggerApp::NetworkLoop()
{
    while( !mShutdown )
    {
        if( mConnectionStatus == ConnectionStatus::ConnectionRequested )
        {
            ConnectToServer();
        }
        else if( mConnectionStatus == ConnectionStatus::DisconnectionRequested )
        {
            DisconnectFromServer();
        }
        else if( mConnectionStatus == ConnectionStatus::Connected )
        {
            SendNetworkData();
            ReceiveNetworkData();
        }

        std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
    }
}

void NodeDebuggerApp::ConnectToServer()
{
    zed_net_address_t address;
    if( 0 != zed_net_get_address( &address, mServerAddressCopy.c_str(), mServerPortCopy ) )
    {
        mConnectionStatus = ConnectionStatus::ConnectionFailed;
        return;
    }

    zed_net_socket_t socket;
    zed_net_tcp_socket_open( &socket, 0, 0, 0 );

    if( zed_net_tcp_connect( &socket, address ) )
    {
        zed_net_socket_close( &socket );
        mConnectionStatus = ConnectionStatus::ConnectionFailed;
        return;
    }

    mSocket = socket;
    mConnectionStatus = ConnectionStatus::Connected;
}

void NodeDebuggerApp::DisconnectFromServer()
{
    zed_net_socket_close( &mSocket );
    mConnectionStatus = ConnectionStatus::Disconnected;
}

void NodeDebuggerApp::SendNetworkData()
{
    mDataMutex.lock();
    {
        if( !mOutgoingPackets.empty() )
        {
            for( const NodeDebuggerPacket & p : mOutgoingPackets)
            {
                zed_net_tcp_socket_send( &mSocket, p.payload.data(), static_cast<int>( p.payload.size() ) );
            }

            mOutgoingPackets.clear();
        }
    }
    mDataMutex.unlock();
}

void NodeDebuggerApp::ReceiveNetworkData()
{
    const bool hasData = zed_net_ext_tcp_wait_for_data( &mSocket, 100 );
    if( !hasData )
    {
        return;
    }

    static const int BUFFER_SIZE = 4096;
    uint8_t buffer[BUFFER_SIZE];

    int bytesReceived = 0;
    do
    {
        bytesReceived = zed_net_tcp_socket_receive( &mSocket, buffer, BUFFER_SIZE );
        if( bytesReceived )
        {
            mReceivedData.insert( mReceivedData.end(), &buffer[0], &buffer[bytesReceived] );
        }
    }
    while( bytesReceived > 0 && bytesReceived == BUFFER_SIZE );

    // check if we have read something
    if( mReceivedData.size() > sizeof( PacketHeader ) )
    {
        MutexLocker lock( mDataMutex );

        // check if we have enough data to form a packet
        PacketHeader* hdr = reinterpret_cast<PacketHeader *>( mReceivedData.data() );

        // received garbage - nothing fancy, just disconnect
        if( hdr->magic != PACKET_MAGIC )
        {
            DisconnectFromServer();
            return;
        }

        while( hdr != nullptr && hdr->payloadSize <= ( mReceivedData.size() - sizeof( PacketHeader ) ) )
        {
            // received garbage - nothing fancy, just disconnect
            if( hdr->magic != PACKET_MAGIC )
            {
                DisconnectFromServer();
                return;
            }

            const size_t dataSizeWithHeader = hdr->payloadSize + sizeof( PacketHeader );

            NodeDebuggerPacket packet;
            packet.payload.resize( hdr->payloadSize );
            memcpy( packet.payload.data(), mReceivedData.data() + sizeof( PacketHeader ), hdr->payloadSize );
            mIncomingPackets.emplace_back( packet );

            // now remove this packet data from the buffer
            const size_t newSize = mReceivedData.size() - dataSizeWithHeader;
            if( newSize )
            {
                memmove( mReceivedData.data(), mReceivedData.data() + dataSizeWithHeader, newSize );
                mReceivedData.resize( newSize );

                hdr = reinterpret_cast<PacketHeader *>( mReceivedData.data() );
            }
            else
            {
                mReceivedData.clear();
                hdr = nullptr;
            }

            if( newSize <= sizeof( PacketHeader ) )
            {
                break;
            }
        }
    }
}

void NodeDebuggerApp::SendChangedNode( const Node& _node )
{
    pugi::xml_document doc;

    pugi::xml_node packetNode = doc.append_child( "Packet" );
    packetNode.append_attribute( "type" ).set_value( "Node" );

    pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

    pugi::xml_node xmlNode = payloadNode.append_child( "Node" );

    std::string pathStr = PathToString( CollectNodePath( &_node ) );

    xmlNode.append_attribute( "path" ).set_value( pathStr.c_str() );
    xmlNode.append_attribute( "name" ).set_value( _node.name.c_str() );
    xmlNode.append_attribute( "type" ).set_value( _node.type.c_str() );

    serializeNodeProp( _node.enable, "enable", xmlNode );
    serializeNodeProp( _node.scale, "scale", xmlNode );

    if( _node.hasRender )
    {
        pugi::xml_node renderNode = xmlNode.append_child( "Render" );

        serializeNodeProp( _node.render.enable, "enable", renderNode );
        serializeNodeProp( _node.render.hide, "hide", renderNode );
        serializeNodeProp( _node.render.color, "color", renderNode );
    }

    if( _node.hasAnimation )
    {
        pugi::xml_node animationNode = xmlNode.append_child( "Animation" );

        serializeNodeProp( _node.animation.loop, "loop", animationNode );
    }

    NodeDebuggerPacket packet;

    MyXMLWriter writer( packet.payload );

#ifdef _DEBUG
    const unsigned int xmlFlags = pugi::format_indent;
#else
    const unsigned int xmlFlags = pugi::format_raw;
#endif
    doc.save( writer, "  ", xmlFlags, pugi::encoding_utf8 );

    if( !packet.payload.empty() )
    {
        const uint32_t payloadLength = static_cast<uint32_t>( packet.payload.size() );

        PacketHeader hdr;
        hdr.magic = PACKET_MAGIC;
        hdr.payloadSize = static_cast<uint32_t>( packet.payload.size() );
        InsertPacketHeader( packet.payload, hdr );

        mOutgoingPackets.emplace_back( packet );
    }
}
