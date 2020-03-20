#include "NodeDebuggerApp.h"

#include "imgui_impl_glfw_gl3_glad.h"

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Kernel/Stringstream.h"

#include <chrono>
#include <sstream>
#include <iterator>
#include <algorithm>

#define ZED_NET_STATIC
#define ZED_NET_IMPLEMENTATION
#include "zed_net.h"

#include "lz4.h"

#include <cassert>

#define MutexLocker std::lock_guard<std::mutex>

#include "ImGui_Ext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static int jpp_dump_callback( const char * _buffer, jpp::jpp_size_t _size, void * _ud )
    {
        FILE * f = (FILE *)_ud;

        size_t writebyte = fwrite( _buffer, _size, 1, f );

        if( writebyte != 1 )
        {
            return -1;
        }

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
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
    //////////////////////////////////////////////////////////////////////////
    NodeDebuggerApp::NodeDebuggerApp()
        : m_window( nullptr )
        , m_shutdown( false )
        , m_width( 1280 )
        , m_height( 720 )
        , m_selectedNode( nullptr )
        , m_defaultIcon( nullptr )
        , m_currentTab( 0 )
        , m_serverAddress()
        , m_serverPort( 18790 )
        , m_serverAddressCopy()
        , m_serverPortCopy( 0 )
        , m_connectionStatus( ConnectionStatus::Disconnected )
        , m_scene( nullptr )
        , m_scenePickerable( nullptr )
        , m_sceneRenderable( nullptr )
        , m_sceneUpdateFreq( 0 )
        , m_sceneUpdateTimer( 0.0 )
        , m_updateSceneOnChange( false )
        , m_pauseRequested( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeDebuggerApp::~NodeDebuggerApp()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerApp::Initialize( const String & _address, const uint16_t _port )
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
        glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );

        m_window = glfwCreateWindow( m_width, m_height, "Node Debugger", nullptr, nullptr );

        if( m_window == nullptr )
        {
            glfwTerminate();

            return false;
        }

        glfwSetWindowUserPointer( m_window, this );

        glfwMakeContextCurrent( m_window );
        gladLoadGLLoader( reinterpret_cast<GLADloadproc>(glfwGetProcAddress) );
        glfwSwapInterval( 1 ); // enable v-sync

        this->LoadIconsAtlas();

        glViewport( 0, 0, m_width, m_height );

        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        glfwSetScrollCallback( m_window, ImGui_ImplGlfw_ScrollCallback );
        glfwSetCharCallback( m_window, ImGui_ImplGlfw_CharCallback );
        glfwSetKeyCallback( m_window, ImGui_ImplGlfw_KeyCallback );
        glfwSetMouseButtonCallback( m_window, ImGui_ImplGlfw_MouseButtonCallback );
        glfwSetWindowSizeCallback(m_window, []( GLFWwindow * _wnd, int _width, int _height ) {
             NodeDebuggerApp * _this = reinterpret_cast<NodeDebuggerApp *>(glfwGetWindowUserPointer( _wnd ));
             if( _this != nullptr )
             {
                 _this->Resize( _width, _height );
             }
        });

        // Setup Dear ImGui binding
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::GetIO().IniFilename = nullptr; // disable "imgui.ini"
        //ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; // tell ImGui to not interfere with our cursors
        ImGui_ImplGlfwGL3_Init( m_window, true );
        ImGui::StyleColorsClassic();
        //ImGuiExt::SetBrightStyle();

        m_shutdown = false;
        m_networkThread = std::thread( &NodeDebuggerApp::NetworkLoop, this );

        // Create tabs
        m_tabs.push_back({
            "Scene Debugger",
            true,
            [this]() { this->DoUISceneDebuggerTab(); }
        });
        m_tabs.push_back({
            "Objects Leak",
            true,
            [this]() { this->DoUIObjectsLeakTab(); }
        });
        m_tabs.push_back({
            "Settings",
            true,
            [this]() { this->DoUISettingsTab(); }
        });

        // if requested to auto-connect, then do so
        if( _address.empty() == false && _port != 0 )
        {
            m_serverAddress = _address;
            m_serverPort = _port;

            this->OnConnectButton();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::Loop()
    {
        double lastTimerValue = glfwGetTime();

        while( GL_FALSE == glfwWindowShouldClose( m_window ) )
        {
            glfwPollEvents();

            const double currentTimerValue = glfwGetTime();
            const double dt = currentTimerValue - lastTimerValue;
            lastTimerValue = currentTimerValue;

            Update( dt );

            glClearColor( 0.412f, 0.796f, 1.0f, 1.0f );
            glClear( GL_COLOR_BUFFER_BIT );

            ImGui_ImplGlfwGL3_NewFrame();

            DoUI();

            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData( ImGui::GetDrawData() );

            glfwSwapBuffers( m_window );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::Shutdown()
    {
        m_shutdown = true;

        glfwTerminate();
        zed_net_shutdown();

        m_networkThread.join();

        if( m_scene != nullptr )
        {
            this->DestroyNode( m_scene );
            m_scene = nullptr;
        }

        if( m_scenePickerable != nullptr )
        {
            this->DestroyNode( m_scenePickerable );
            m_scenePickerable = nullptr;
        }

        if( m_sceneRenderable != nullptr )
        {
            this->DestroyNode( m_sceneRenderable );
            m_sceneRenderable = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::Resize( const int _width, const int _height )
    {
        if( m_width != _width || m_height != _height )
        {
            m_width = _width;
            m_height = _height;
            glViewport( 0, 0, m_width, m_height );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::Update( const double _dt )
    {
        MutexLocker lock( m_dataMutex );

        if( m_connectionStatus == ConnectionStatus::Disconnected )
        {
            if( m_scene != nullptr )
            {
                this->DestroyNode( m_scene );
                m_scene = nullptr;
            }

            if( m_scenePickerable != nullptr )
            {
                this->DestroyNode( m_scenePickerable );
                m_scenePickerable = nullptr;
            }

            if( m_sceneRenderable != nullptr )
            {
                this->DestroyNode( m_sceneRenderable );
                m_sceneRenderable = nullptr;
            }

            m_incomingPackets.resize( 0 );
            m_outgoingPackets.resize( 0 );
            m_receivedData.resize( 0 );
        }
        else if( m_connectionStatus == ConnectionStatus::Connected )
        {
            if( m_incomingPackets.empty() == false )
            {
                NodeDebuggerPacket & packet = m_incomingPackets.front();
                this->ProcessPacket( packet );
                m_incomingPackets.pop_front();
            }

            if( m_selectedNode && m_selectedNode->dirty )
            {
                this->SendChangedNode( *m_selectedNode );
                m_selectedNode->dirty = false;
            }

            if( m_selectedNodePath.empty() == false )
            {
                this->SendNodeSelection( m_selectedNodePath );
                m_selectedNodePath.clear();
            }

            if( m_sceneUpdateFreq > 0 )
            {
                m_sceneUpdateTimer += _dt;

                const double updateInterval = 1.0 / static_cast<double>( m_sceneUpdateFreq );
                if( m_sceneUpdateTimer >= updateInterval )
                {
                    this->SendSceneRequest();
                    m_sceneUpdateTimer = 0.0;
                }
            }

            if( m_pauseRequested == true )
            {
                this->SendPauseRequest();
                m_pauseRequested = false;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::CompressPacket( NodeDebuggerPacket & _packet, PacketHeader & _hdr )
    {
        Blobject::size_type payloadSize = _packet.payload.size();

        if( payloadSize < 1024 )
        {
            _hdr.compressedSize = static_cast<uint32_t>( payloadSize );
            _hdr.uncompressedSize = 0; // packet is not compressed
        }
        else
        {
            const size_t maxCompressedSize = ::LZ4_compressBound( static_cast<int>(payloadSize) );
            Mengine::Blobject compressedPayload( maxCompressedSize );

            int result = ::LZ4_compress_default( reinterpret_cast<char*>( _packet.payload.data() ), reinterpret_cast<char*>( compressedPayload.data() ), static_cast<int>( payloadSize ), static_cast<int>( maxCompressedSize ) );

            if( result < 0 || payloadSize < (Blobject::size_type)result )
            {
                _hdr.compressedSize = static_cast<uint32_t>( payloadSize );
                _hdr.uncompressedSize = 0; // packet is not compressed
            }
            else
            {
                _hdr.compressedSize = (uint32_t)result;
                _hdr.uncompressedSize = (uint32_t)payloadSize;

                compressedPayload.resize( _hdr.compressedSize );
                _packet.payload.swap( compressedPayload );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::UncompressPacket( NodeDebuggerPacket & _packet, PacketHeader & _hdr, const uint8_t * _receivedData )
    {
        if( _hdr.uncompressedSize == 0 )
        {
            // this packet is uncompressed, just copy
            _packet.payload.resize( _hdr.compressedSize );
            memcpy( _packet.payload.data(), _receivedData, _hdr.compressedSize );
        }
        else
        {
            _packet.payload.resize( _hdr.uncompressedSize );

            const int result = ::LZ4_decompress_safe( reinterpret_cast<const char*>( _receivedData ), reinterpret_cast<char*>( _packet.payload.data() ), static_cast<int>( _hdr.compressedSize ), static_cast<int>( _hdr.uncompressedSize ) );
            assert( static_cast<uint32_t>(result) == _hdr.uncompressedSize );
        }
    }
    //////////////////////////////////////////////////////////////////////////
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
        String typeStr = typeAttrib.value();

        pugi::xml_node payloadNode = packetNode.child( "Payload" );
        if( !payloadNode )
        {
            return;
        }

        if( typeStr == "Scene" )
        {
            this->ReceiveScene( payloadNode );
        }

        if( typeStr == "Pickerable" )
        {
            this->ReceivePickerable( payloadNode );
        }

        if( typeStr == "Renderable" )
        {
            this->ReceiveRenderable( payloadNode );
        }

        if( typeStr == "ObjectsLeak" )
        {
            this->ReceiveObjectsLeak( payloadNode );
        }

        if( typeStr == "Settings" )
        {
            this->ReceiveSettings( payloadNode );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::ReceiveScene( const pugi::xml_node & _xmlContainer )
    {
        pugi::xml_node xmlNode = _xmlContainer.first_child();

        if( xmlNode )
        {
            if( m_scene != nullptr )
            {
                DestroyNode( m_scene );
            }

            m_scene = new DebuggerNode();
            m_scene->parent = nullptr;

            this->DeserializeNode( xmlNode, m_scene );

            m_selectedNode = this->PathToNode( this->StringToPath( m_lastSelectedNodePath ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::ReceivePickerable( const pugi::xml_node & _xmlContainer )
    {
        pugi::xml_node xmlNode = _xmlContainer.first_child();

        if( xmlNode )
        {
            if( m_scenePickerable != nullptr )
            {
                this->DestroyNode( m_scenePickerable );
            }

            m_scenePickerable = new DebuggerNode();
            m_scenePickerable->parent = nullptr;

            this->DeserializeNode( xmlNode, m_scenePickerable );

            m_selectedNode = this->PathToNode( this->StringToPath( m_lastSelectedNodePath ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::ReceiveRenderable( const pugi::xml_node & _xmlContainer )
    {
        pugi::xml_node xmlNode = _xmlContainer.first_child();

        if( xmlNode )
        {
            if( m_sceneRenderable )
            {
                DestroyNode( m_sceneRenderable );
            }

            m_sceneRenderable = new DebuggerNode();
            m_sceneRenderable->parent = nullptr;

            DeserializeNode( xmlNode, m_sceneRenderable );

            m_selectedNode = PathToNode( StringToPath( m_lastSelectedNodePath ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::ReceiveObjectsLeak( const pugi::xml_node & _xmlContainer )
    {
        MENGINE_UNUSED( _xmlContainer );

        m_objectLeaks.clear();

        m_objectLeakGeneration = _xmlContainer.attribute( "Generation" ).value();

        for( const pugi::xml_node & child : _xmlContainer.children() )
        {
            const pugi::char_t * type = child.attribute( "Factory" ).value();
            
            VectorLeaks & leaks = m_objectLeaks[type];

            for( const pugi::xml_node & obj : child.children() )
            {
                const pugi::char_t * file = obj.attribute( "File" ).value();
                const pugi::char_t * function = obj.attribute( "Function" ).value();
                const pugi::char_t * line = obj.attribute( "Line" ).value();
                const pugi::char_t * message = obj.attribute( "Message" ).value();

                LeakDesc desc;
                desc.file = file;
                desc.function = function;
                desc.line = line;
                desc.message = message;

                for( const pugi::xml_node & parent : obj.children() )
                {
                    const pugi::char_t * parent_file = parent.attribute( "File" ).value();
                    const pugi::char_t * parent_function = parent.attribute( "Function" ).value();
                    const pugi::char_t * parent_line = parent.attribute( "Line" ).value();
                    const pugi::char_t * parent_message = parent.attribute( "Message" ).value();

                    LeakDesc parent_desc;
                    parent_desc.file = parent_file;
                    parent_desc.function = parent_function;
                    parent_desc.line = parent_line;
                    parent_desc.message = parent_message;

                    desc.parents.emplace_back( parent_desc );
                }

                leaks.emplace_back( desc );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::ReceiveSettings( const pugi::xml_node & _xmlContainer )
    {
        MENGINE_UNUSED( _xmlContainer );

        m_settings.clear();

        for( const pugi::xml_node & child : _xmlContainer.children() )
        {
            const pugi::char_t * name = child.attribute( "name" ).value();
            const pugi::char_t * file = child.attribute( "file" ).value();

            SettingDesc desc;
            desc.name = name;
            desc.file = file;

            FILE * f = fopen( desc.file.c_str(), "rb" );

            if( f == NULL )
            {
                continue;
            }

            fseek( f, 0L, SEEK_END );
            long sz = ftell( f );
            rewind( f );

            uint8_t buffer[8096];
            fread( buffer, 1, 8096, f );
            fclose( f );

            jpp::object json = jpp::load( buffer, sz, nullptr, nullptr );

            if( json == jpp::detail::invalid )
            {
                continue;
            }

            desc.json = json;

            m_settings.emplace_back( desc );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DeserializeNode( const pugi::xml_node& _xmlNode, DebuggerNode* _node )
    {
        _node->deserialize( _xmlNode );

        _node->transformationProxy = false;

        if( _node->type == "MatrixProxy" )
        {
            _node->transformationProxy = true;
        }

        pugi::xml_node transformNode = _xmlNode.child( "Transformation" );

        _node->transformation.deserialize( transformNode );

        pugi::xml_node renderNode = _xmlNode.child( "Render" );

        _node->hasRender = renderNode;

        if( _node->hasRender )
        {
            _node->render.deserialize( renderNode );
        }

        pugi::xml_node animationNode = _xmlNode.child( "Animation" );

        _node->hasAnimation = animationNode;

        if( _node->hasAnimation )
        {
            _node->render.deserialize( animationNode );
        }

        pugi::xml_node typeTextFieldNode = _xmlNode.child( "Type:TextField" );

        _node->isTypeTextField = typeTextFieldNode;

        if( _node->isTypeTextField )
        {
            _node->textField.deserialize( typeTextFieldNode );
        }

        pugi::xml_node typeMovie2Node = _xmlNode.child( "Type:Movie2" );

        _node->isTypeMovie2 = typeMovie2Node;

        if( _node->isTypeMovie2 )
        {
            _node->movie2.deserialize( typeMovie2Node );
        }

        pugi::xml_node childrenNode = _xmlNode.child( "Children" );

        if( childrenNode )
        {
            for( const auto & child : childrenNode.children() )
            {
                DebuggerNode* childNode = new DebuggerNode();
                childNode->parent = _node;
                DeserializeNode( child, childNode );
                _node->children.push_back( childNode );
            }
        }

        _node->icon = GetIconForNodeType( _node->type );

        _node->dirty = false;
    }
    //////////////////////////////////////////////////////////////////////////
    Vector<uint32_t> NodeDebuggerApp::CollectNodePath( const DebuggerNode * _node )
    {
        Vector<uint32_t> path;

        const DebuggerNode* current = _node;
        while( current && current->parent )
        {
            path.insert( path.begin(), current->uid );
            current = current->parent;
        }

        return path;
    }
    //////////////////////////////////////////////////////////////////////////
    String NodeDebuggerApp::PathToString( const Vector<uint32_t> & _path )
    {
        Stringstream stream;
        std::copy( _path.begin(), _path.end(), std::ostream_iterator<uint32_t>( stream, "/" ) );

        return stream.str();
    }
    //////////////////////////////////////////////////////////////////////////
    Vector<uint32_t> NodeDebuggerApp::StringToPath( String & _pathStr )
    {
        Vector<uint32_t> path;

        if( !_pathStr.empty() && _pathStr[0] != '-' )
        {
            const Char* ptr = _pathStr.c_str();

            uint32_t uid = 0;
            while( *ptr )
            {
                if( *ptr == '/' )
                {
                    path.push_back( uid );
                    uid = 0;
                }
                else
                {
                    uid *= 10;
                    uid += static_cast<uint32_t>( *ptr - '0' );
                }

                ++ptr;
            }
        }

        return path;
    }
    //////////////////////////////////////////////////////////////////////////
    DebuggerNode * NodeDebuggerApp::PathToNode( const Vector<uint32_t> & _path )
    {
        DebuggerNode * result = nullptr;

        if( !_path.empty() )
        {
            DebuggerNode * node = m_scene;

            auto it = _path.begin(), end = _path.end();
            for( ; it != end; ++it )
            {
                const uint32_t nextUid = *it;
                auto found = std::find_if( node->children.begin(), node->children.end(), [nextUid]( const DebuggerNode * _n )->bool {
                    return _n->uid == nextUid;
                });

                if( found != node->children.end() )
                {
                    node = *found;
                }
                else
                {
                    break;
                }
            }

            if( it == end ) // seems like we've found it!
            {
                result = node;
            }
        }

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DestroyNode( DebuggerNode * _node )
    {
        if( m_selectedNode == _node )
        {
            m_selectedNode = nullptr;
        }

        for( auto n : _node->children )
        {
            DestroyNode( n );
        }

        _node->children.resize( 0 );
        delete _node;
    }
    //////////////////////////////////////////////////////////////////////////
    const CachedImage* NodeDebuggerApp::GetIconImage( const String & _name )
    {
        const CachedImage* result = nullptr;

        auto it = std::find_if( m_imagesCache.begin(), m_imagesCache.end(), [&_name]( const CachedImage & _ci )->bool {
            return _ci.name == _name;
        });

        if( it != m_imagesCache.end() )
        {
            result = &(*it);
        }
        else
        {
            int width, height, bpp;
            stbi_uc* data = stbi_load( _name.c_str(), &width, &height, &bpp, STBI_rgb_alpha );
            if( data != nullptr )
            {
                GLuint texture;
                glGenTextures( 1, &texture );
                glBindTexture( GL_TEXTURE_2D, texture );
                glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );

                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
                glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

                glBindTexture( GL_TEXTURE_2D, 0 );

                stbi_image_free( data );

                m_imagesCache.push_back( { _name, static_cast<uintptr_t>(texture), static_cast<size_t>(width), static_cast<size_t>(height) } );

                result = &m_imagesCache.back();
            }
        }

        return result;
    }

    void NodeDebuggerApp::LoadIconsAtlas()
    {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file( TEXT( "Icons.xml" ) );
        if( !result )
        {
            return;
        }

        pugi::xml_node mainNode = doc.child( "Icons" );
        if( !mainNode )
        {
            return;
        }

        for( const auto & iconNode : mainNode.children() )
        {
            pugi::xml_attribute attrName = iconNode.attribute( "name" );
            pugi::xml_attribute attrImage = iconNode.attribute( "image" );

            if( attrName && attrImage )
            {
                const CachedImage* ci = GetIconImage( attrImage.as_string() );
                if( ci != nullptr )
                {
                    pugi::xml_attribute attrX = iconNode.attribute("x");
                    pugi::xml_attribute attrY = iconNode.attribute("y");

                    const float x = attrX ? attrX.as_float() : 0.0f;
                    const float y = attrY ? attrY.as_float() : 0.0f;

                    NodeIcon ni;
                    ni.name = attrName.as_string();
                    ni.image = ci->image;
                    ni.uv0_X = x / static_cast<float>(ci->width);
                    ni.uv0_Y = y / static_cast<float>(ci->height);
                    ni.uv1_X = (x + 16.0f) / static_cast<float>(ci->width);
                    ni.uv1_Y = (y + 16.0f) / static_cast<float>(ci->height);

                    m_icons.emplace_back( ni );
                }
            }
        }

        // trying to assign the default icon
        auto it = std::find_if( m_icons.begin(), m_icons.end(), []( const NodeIcon & _ni )->bool {
            return _ni.name == "?";
        });

        if( it != m_icons.end() )
        {
            m_defaultIcon = &(*it);
        }
        else
        {
            m_defaultIcon = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const NodeIcon* NodeDebuggerApp::GetIconForNodeType( const String & _nodeType )
    {
        auto it = std::find_if( m_icons.begin(), m_icons.end(), [&_nodeType]( const NodeIcon & _ni )->bool {
            return _ni.name == _nodeType;
        });

        if( it == m_icons.end() )
        {
            return m_defaultIcon;
        }
        else
        {
            return &(*it);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DoUI()
    {
        const ImGuiWindowFlags kPanelFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

        ImGui::SetNextWindowPos( ImVec2( 0, 0 ), ImGuiCond_FirstUseEver );
        ImGui::SetNextWindowSize( ImVec2( static_cast<float>(m_width), static_cast<float>(m_height) ), ImGuiCond_Always );
        ImGui::GetStyle().WindowRounding = 0.f;

        if( ImGui::Begin( "Node Debugger", nullptr, kPanelFlags ) )
        {
            if( ImGui::BeginTabBar( "##Tabs", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton ) )
            {
                for( auto& tab : m_tabs )
                {
                    if( ImGui::BeginTabItem( tab.title.c_str() ) )
                    {
                        tab.functor();

                        ImGui::EndTabItem();
                    }
                }

                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DoUISceneDebuggerTab()
    {
        const float leftPanelWidth = 400.0f;

        ImGui::Columns( 2, nullptr, true );
        ImGui::SetColumnWidth( 0, leftPanelWidth );

        if( ImGui::CollapsingHeader( "Server:", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            char serverAddress[256] = {0};

            if( !m_serverAddress.empty() )
            {
                std::copy( m_serverAddress.begin(), m_serverAddress.end(), serverAddress );
            }

            m_serverAddress = this->DoIPInput( "Address:", m_serverAddress );

            ImGui::BeginGroup();
            {
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted( "IP Port:" );
                ImGui::SameLine();

                const float width = ImGui::CalcItemWidth();
                ImGui::PushItemWidth( width * 0.25f );
                int port = static_cast<int>(m_serverPort);
                if( ImGui::InputInt( "##Port", &port, 0, 0, ImGuiInputTextFlags_CharsDecimal ) )
                {
                    m_serverPort = static_cast<uint16_t>(port & 0xFFFF);
                }
                ImGui::PopItemWidth();
            }
            ImGui::EndGroup();

            if( m_connectionStatus == ConnectionStatus::Connected )
            {
                const ImVec4 redButtonColor( 0.5f, 0.f, 0.f, 1.f );
                ImGui::PushStyleColor( ImGuiCol_Button, redButtonColor );
                ImGui::PushStyleColor( ImGuiCol_ButtonHovered, redButtonColor );
                if( ImGui::Button( "Disconnect" ) )
                {
                    this->OnDisconnectButton();
                }
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }
            else if( m_connectionStatus == ConnectionStatus::Disconnected || m_connectionStatus == ConnectionStatus::ConnectionFailed )
            {
                const ImVec4 greenButtonColor( 0.1686f, 0.5686f, 0.f, 1.f );
                ImGui::PushStyleColor( ImGuiCol_Button, greenButtonColor );
                ImGui::PushStyleColor( ImGuiCol_ButtonHovered, greenButtonColor );
                if( ImGui::Button( "Connect" ) )
                {
                    this->OnConnectButton();
                }
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            }
        }

        if( ImGui::CollapsingHeader( "Game controls:" ) )
        {
            ImGui::PushItemFlag( ImGuiItemFlags_Disabled, m_updateSceneOnChange );
            int hz = m_sceneUpdateFreq;
            if( ImGui::InputInt( "Update freq (hz):", &hz ) )
            {
                m_sceneUpdateFreq = std::clamp( hz, 0, 30 );
                m_sceneUpdateTimer = 0.0;
            }
            ImGui::PopItemFlag();

            if( ImGui::Checkbox( "Update scene on change", &m_updateSceneOnChange ) )
            {
                if( m_updateSceneOnChange )
                {
                    m_sceneUpdateFreq = 0;
                    m_sceneUpdateTimer = 0.0;
                }
            }

            if( ImGui::Button( "Pause game" ) )
            {
                OnPauseButton();
            }
        }

        static int SceneTagId = 0;

        if( ImGui::CollapsingHeader( "Type:" ) )
        {
            ImGui::RadioButton( "Full", &SceneTagId, 0 ); ImGui::SameLine();
            ImGui::RadioButton( "Picker", &SceneTagId, 1 ); ImGui::SameLine();
            ImGui::RadioButton( "Render", &SceneTagId, 2 );
        }

        switch( SceneTagId )
        {
        case 0:
            {
                if( ImGui::CollapsingHeader( "Scene:", ImGuiTreeNodeFlags_DefaultOpen ) )
                {
                    if( m_scene )
                    {
                        if( ImGui::BeginChild( "SceneTree", ImVec2( 0, 0 ), false, ImGuiWindowFlags_HorizontalScrollbar ) )
                        {
                            this->DoNodeElement( m_scene, "Full" );
                        }
                        ImGui::EndChild();
                    }
                }
            }break;
        case 1:
            {
                if( ImGui::CollapsingHeader( "Pickerable:", ImGuiTreeNodeFlags_DefaultOpen ) )
                {
                    if( m_scenePickerable )
                    {
                        if( ImGui::BeginChild( "SceneTree", ImVec2( 0, 0 ), false, ImGuiWindowFlags_HorizontalScrollbar ) )
                        {
                            this->DoNodeElement( m_scenePickerable, "Pickerable" );
                        }
                        ImGui::EndChild();
                    }
                }
            }break;
        case 2:
            {
                if( ImGui::CollapsingHeader( "Renderable:", ImGuiTreeNodeFlags_DefaultOpen ) )
                {
                    if( m_sceneRenderable )
                    {
                        if( ImGui::BeginChild( "SceneTree", ImVec2( 0, 0 ), false, ImGuiWindowFlags_HorizontalScrollbar ) )
                        {
                            this->DoNodeElement( m_sceneRenderable, "Renderable" );
                        }
                        ImGui::EndChild();
                    }
                }
            }break;
        }

        ImGui::NextColumn();

        if( ImGui::BeginChild( "Panel" ) )
        {
            if( m_selectedNode )
            {
                this->DoNodeProperties( m_selectedNode );
            }
        }
        ImGui::EndChild();

        ImGui::Columns( 1 );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DoUIObjectsLeakTab()
    {
        uint32_t leaksCount = 0;

        for( auto && [type, objects] : m_objectLeaks )
        {
            leaksCount += objects.size();
        }

        ImGui::TextColored( ImVec4( 0.f, 1.f, 0.f, 1.f ), "Generator: %s [total %u]"
            , m_objectLeakGeneration.c_str()
            , leaksCount
        );

        ImGui::Separator();

        uint32_t index = 0;

        for( auto && [type, objects] : m_objectLeaks )
        {
            ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.f, 1.f, 0.f, 1.f ) );

            char label_node[32];
            sprintf( label_node, "##tree_node_%u"
                , index
            );

            bool isOpened = ImGui::TreeNodeEx( label_node, ImGuiTreeNodeFlags_DefaultOpen, "%s [%u]"
                , type.c_str()
                , objects.size()
            );

            ImGui::PopStyleColor();

            if( isOpened == true )
            {
                ImGui::Separator();

                for( const LeakDesc & leak : objects )
                {
                    ImGui::BulletText( "file: %s"
                        , leak.file.c_str()
                    );

                    ImGui::BulletText( "line: %s"
                        , leak.line.c_str()
                    );

                    ImGui::BulletText( "function: %s"
                        , leak.function.c_str()
                    );

                    char label_text[16];
                    sprintf( label_text, "##text_%u"
                        , index
                    );

                    ImGui::InputTextMultiline( label_text
                        , (char *)leak.message.data()
                        , leak.message.size()
                        , ImVec2( -1.f, ImGui::GetTextLineHeight() * 2.5f )
                        , ImGuiInputTextFlags_ReadOnly
                    );

                    ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0.f, 0.f, 1.f, 1.f ) );

                    char label_parent_node[32];
                    sprintf( label_parent_node, "##tree_parent_node_%u"
                        , index
                    );

                    bool isParentOpened = ImGui::TreeNodeEx( label_parent_node, ImGuiTreeNodeFlags_None, "traceback" );

                    ImGui::PopStyleColor();

                    if( isParentOpened == true )
                    {
                        ImGui::Separator();

                        for( const LeakDesc & parent : leak.parents )
                        {
                            ImGui::BulletText( "file: %s"
                                , parent.file.c_str()
                            );

                            ImGui::BulletText( "line: %s"
                                , parent.line.c_str()
                            );

                            ImGui::BulletText( "function: %s"
                                , parent.function.c_str()
                            );

                            //char label_text[16];
                            //sprintf( label_text, "##text_%u"
                            //    , index
                            //);

                            ImGui::InputTextMultiline( label_text
                                , (char *)parent.message.data()
                                , parent.message.size()
                                , ImVec2( -1.f, ImGui::GetTextLineHeight() * 2.5f )
                                , ImGuiInputTextFlags_ReadOnly
                            );

                            ImGui::Separator();
                        }

                        ImGui::TreePop();
                    }

                    ImGui::Separator();

                    ++index;
                }

                ImGui::TreePop();
            }

            ++index;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DoUISettingsTab()
    {
        const float leftPanelWidth = 400.0f;

        ImGui::Columns( 2, nullptr, true );
        //ImGui::SetColumnWidth( 0, leftPanelWidth );

        if( ImGui::CollapsingHeader( "Settings:", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            if( ImGui::BeginChild( "SceneTree", ImVec2( 0, 0 ), false, ImGuiWindowFlags_HorizontalScrollbar ) )
            {
                for( const SettingDesc & desc : m_settings )
                {
                    ImGuiTreeNodeFlags seletedFlag = ImGuiTreeNodeFlags_Leaf | (m_selectedSetting == desc.name ? ImGuiTreeNodeFlags_Selected : 0);

                    bool isOpened = ImGui::TreeNodeEx( desc.name.c_str(), seletedFlag );

                    bool isClicked = ImGui::IsItemClicked();

                    if( isClicked == true )
                    {
                        m_selectedSetting = desc.name;
                    }

                    if( isOpened == true )
                    {
                        ImGui::TreePop();
                    }
                }
            }

            ImGui::EndChild();
        }

        ImGui::NextColumn();

        for( SettingDesc & desc : m_settings )
        {
            if( m_selectedSetting != desc.name )
            {
                continue;
            }

            bool dirty = false;

            for( auto && [key, record] : desc.json )
            {
                const jpp::object & j = record["value"];

                jpp::e_type type = j.type();

                switch( type )
                {
                case jpp::e_type::JPP_STRING:
                    {
                        char value[2048] = {0};
                        const char * jv = j;
                        strcpy( value, jv );
                        if( ImGui::InputText( key, value, 2048 ) == true )
                        {
                            record.set( "value", value );

                            dirty = true;
                        }
                    }break;
                case jpp::e_type::JPP_INTEGER:
                    {
                        int min = record.get( "min", 0 );
                        int max = record.get( "max", 0 );
                        
                        float speed = (min == max) ? 1.f : float(max - min) * 0.0015f;

                        int value = j;
                        if( ImGui::DragInt( key, &value, speed, min, max ) == true )
                        {
                            record.set( "value", value );

                            dirty = true;
                        }
                    }break;
                case jpp::e_type::JPP_REAL:
                    {
                        float min = record.get( "min", 0.f );
                        float max = record.get( "max", 0.f );

                        float speed = (min == max) ? 1.f : float( max - min ) * 0.0015f;

                        float value = j;
                        if( ImGui::DragFloat( key, &value, speed, min, max ) == true )
                        {
                            record.set( "value", value );

                            dirty = true;
                        }
                    }break;
                case jpp::e_type::JPP_TRUE:
                case jpp::e_type::JPP_FALSE:
                    {
                        bool value = j;
                        if( ImGui::Checkbox( key, &value ) == true )
                        {
                            record.set( "value", value );

                            dirty = true;
                        }
                    }break;
                default:
                    break;
                }
            }

            if( dirty == true )
            {
                FILE * f = fopen( desc.file.c_str(), "wb" );

                bool successful = jpp::dump( desc.json, &jpp_dump_callback, f );
                MENGINE_UNUSED( successful );

                fclose( f );                
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    String NodeDebuggerApp::DoIPInput( const String & _title, const String & _inIP )
    {
        int octets[4] = {127, 0, 0, 1};

        if( _inIP.empty() == false )
        {
            int idx = 0;
            octets[idx] = 0;

            for( const char c : _inIP )
            {
                if( isdigit( c ) )
                {
                    octets[idx] *= 10;
                    octets[idx] += static_cast<int>(c - '0');
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
        ImGui::AlignTextToFramePadding();
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

        Stringstream ss;
        ss << octets[0] << '.' << octets[1] << '.' << octets[2] << '.' << octets[3];

        return ss.str();
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DoNodeElement( DebuggerNode * _node, const String & _tag )
    {
        const ImGuiTreeNodeFlags seletedFlag = (m_selectedNode == _node) ? ImGuiTreeNodeFlags_Selected : 0;
        const ImGuiTreeNodeFlags flagsNormal = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | seletedFlag;
        const ImGuiTreeNodeFlags flagsNoChildren = ImGuiTreeNodeFlags_Leaf | seletedFlag;

        String treeNodeName = _node->name.empty() == false ? _node->name : "***unnamed***";
        String treeNodeId = _tag + "_" + (Stringstream() << _node->uid).str();
        String fullLabel = treeNodeName + " [" + _node->type + "]" + "##" + treeNodeId;

        ImGuiExt::ImIcon icon;
        ImGuiExt::ImIcon * iconPtr = nullptr;
        if( _node->icon )
        {
            icon.image = reinterpret_cast<ImTextureID>(_node->icon->image);
            icon.uv0 = ImVec2( _node->icon->uv0_X, _node->icon->uv0_Y );
            icon.uv1 = ImVec2( _node->icon->uv1_X, _node->icon->uv1_Y );

            iconPtr = &icon;
        }

        auto result = ImGuiExt::TreeNodeWithIcon
        (
            iconPtr,
            fullLabel.c_str(),
            _node->children.empty() ? flagsNoChildren : flagsNormal,
            !_node->enable
        );

        if( result.second )
        {
            this->OnSelectNode( _node );
        }

        if( result.first )
        {
            for( DebuggerNode* child : _node->children )
            {
                this->DoNodeElement( child, _tag );
            }

            ImGui::TreePop();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DoNodeProperties( DebuggerNode * _node )
    {
        auto uiEditorBool = [_node]( const char * _caption, bool & _prop )
        {
            bool testValue = _prop;
            bool input = ImGui::Checkbox( _caption, &testValue );

            if( input && testValue != _prop )
            {
                _prop = testValue;
                _node->dirty = true;
            }
        };

        auto uiReadOnlyBool = [_node]( const char * _caption, bool & _prop )
        {
            bool testValue = _prop;

            ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
            ImGui::PushStyleColor( ImGuiCol_FrameBg, ImVec4( 0.15f, 0.3f, 0.2f, 1.f ) );
            ImGui::Checkbox( _caption, &testValue );
            ImGui::PopStyleColor();
            ImGui::PopItemFlag();
        };

        auto uiEditorVec1U = [_node]( const char * _caption, uint32_t & _prop )
        {
            uint32_t testValue = _prop;
            bool input = ImGui::DragScalarN( _caption, ImGuiDataType_U32, &testValue, 1, 1.f );

            if( input && testValue != _prop )
            {
                _prop = testValue;
                _node->dirty = true;
            }
        };

        auto uiEditorVec3f = [_node]( const char * _caption, mt::vec3f & _prop )
        {
            mt::vec3f testValue = _prop;
            bool input = ImGui::DragFloat3( _caption, testValue.buff() );

            if( input && testValue != _prop )
            {
                _prop = testValue;
                _node->dirty = true;
            }
        };

        auto uiReadOnlyVec3f = [_node]( const char * _caption, mt::vec3f & _prop )
        {
            mt::vec3f testValue = _prop;
            ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
            ImGui::PushStyleColor( ImGuiCol_FrameBg, ImVec4( 0.15f, 0.3f, 0.2f, 1.f ) );
            ImGui::DragFloat3( _caption, testValue.buff() );
            ImGui::PopStyleColor();
            ImGui::PopItemFlag();
        };

        auto uiEditorVec1f = [_node]( const char * _caption, float & _prop )
        {
            float testValue = _prop;
            bool input = ImGui::DragFloat( _caption, &testValue, 0.01f );

            if( input && testValue != _prop )
            {
                _prop = testValue;
                _node->dirty = true;
            }
        };

        auto uiReadOnlyVec1f = [_node]( const char * _caption, float & _prop )
        {
            float testValue = _prop;
            ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
            ImGui::PushStyleColor( ImGuiCol_FrameBg, ImVec4( 0.15f, 0.3f, 0.2f, 1.f ) );
            ImGui::DragFloat( _caption, &testValue );
            ImGui::PopStyleColor();
            ImGui::PopItemFlag();
        };

        auto uiEditorVec2f = [_node]( const char * _caption, mt::vec2f & _prop )
        {
            mt::vec2f testValue = _prop;
            bool input = ImGui::DragFloat2( _caption, testValue.buff() );

            if( input && testValue != _prop )
            {
                _prop = testValue;
                _node->dirty = true;
            }
        };

        auto uiEditorColor = [_node]( const Char * _caption, Color & _prop )
        {
            Color testValue = _prop;
            bool input = ImGui::ColorEdit4( _caption, testValue.buff() );

            if( input && testValue != _prop )
            {
                _prop = testValue;
                _node->dirty = true;
            }
        };

        auto uiReadOnlyColor = [_node]( const Char * _caption, Color & _prop )
        {
            Color testValue = _prop;
            ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
            ImGui::PushStyleColor( ImGuiCol_FrameBg, ImVec4( 0.15f, 0.3f, 0.2f, 1.f ) );
            ImGui::ColorEdit4( _caption, testValue.buff() );
            ImGui::PopStyleColor();
            ImGui::PopItemFlag();
        };

        auto uiEditorString = [_node]( const char * _caption, String & _prop )
        {
            char testValue[2048] = { 0 };
            strcpy( testValue, _prop.c_str() );
            testValue[2047] = 0;
            bool input = ImGui::InputText( _caption, testValue, 2048 );

            if( input && _prop != testValue )
            {
                _prop = testValue;
                _node->dirty = true;
            }
        };

        auto uiReadOnlyString = [_node]( const char * _caption, String & _prop )
        {
            char testValue[2048] = {0};
            strcpy( testValue, _prop.c_str() );
            testValue[2047] = 0;
            ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
            ImGui::PushStyleColor( ImGuiCol_FrameBg, ImVec4( 0.15f, 0.3f, 0.2f, 1.f ) );
            ImGui::InputText( _caption, testValue, 2048 );
            ImGui::PopStyleColor();
            ImGui::PopItemFlag();
        };

        auto uiEditorListBox = [_node]( const char * _caption, uint32_t & _prop, const std::initializer_list<String> & _items, uint32_t _count )
        {
            int32_t testValue = _prop;
            bool input = ImGui::ListBox( _caption, &testValue, []( void* data, int idx, const char** out_text )
            {
                *out_text = ((String *)data + idx)->c_str();
                return true;
            }, (void *)_items.begin(), _count );

            if( input && _prop != static_cast<uint32_t>(testValue) )
            {
                _prop = testValue;
                _node->dirty = true;
            }
        };

        auto uiReadOnlyListBox = [_node]( const char * _caption, uint32_t & _prop, const std::initializer_list<String> & _items, uint32_t _count )
        {
            int32_t testValue = _prop;
            ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
            ImGui::PushStyleColor( ImGuiCol_FrameBg, ImVec4( 0.15f, 0.3f, 0.2f, 1.f ) );
            ImGui::ListBox( _caption, &testValue, []( void* data, int idx, const char** out_text )
            {
                *out_text = ((String *)data + idx)->c_str();
                return true;
            }, (void *)_items.begin(), _count );
            ImGui::PopStyleColor();
            ImGui::PopItemFlag();
        };

        if( ImGui::CollapsingHeader( "Node:", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            ImGui::Text( "Name: %s", _node->name.c_str() );
            ImGui::SameLine();
            ImGui::Dummy( ImVec2( 20.0f, 5.0f ) );
            ImGui::SameLine();
            ImGui::Text( "Type: %s", _node->type.c_str() );
            ImGui::Spacing();

            uiEditorBool( "Enable##node_enable", _node->enable );
            ImGui::Spacing();
        }

        if( ImGui::CollapsingHeader( "Transformation:", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            NodeTransformation & transformation = _node->transformation;

            if( _node->transformationProxy == false )
            {
                uiEditorVec3f( "Position", transformation.position );
                uiEditorVec3f( "Origin", transformation.origin );
                uiEditorVec2f( "Skew", transformation.skew );
                uiEditorVec3f( "Scale", transformation.scale );
                uiEditorVec3f( "Orientation", transformation.orientation );

                ImGui::Spacing();
            }

            uiReadOnlyVec3f( "World Position", transformation.worldPosition );
            ImGui::Spacing();
        }

        if( _node->hasRender && ImGui::CollapsingHeader( "Render:", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            uiReadOnlyBool( "Enable", _node->render.enable );
            uiEditorBool( "Hide", _node->render.hide );
            ImGui::Spacing();

            uiEditorColor( "Local Color", _node->render.local_color );
            uiEditorColor( "Personal Color", _node->render.personal_color );            
        }

        if( _node->hasAnimation && ImGui::CollapsingHeader( "Animation:", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            uiEditorBool( "loop", _node->animation.loop );
            uiReadOnlyBool( "play", _node->animation.play );
            uiReadOnlyBool( "pause", _node->animation.pause );
            uiEditorVec1f( "time", _node->animation.time );
            uiReadOnlyVec1f( "duration", _node->animation.duration );
        }

        if( _node->isTypeTextField && ImGui::CollapsingHeader( "TextField:", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            uiEditorVec1f( "Max Length", _node->textField.MaxLength );
            uiEditorBool( "Wrap", _node->textField.Wrap );
            uiEditorString( "TextID", _node->textField.TextID );
            uiEditorString( "AliasEnvironment", _node->textField.TextAliasEnvironment );

            if( _node->textField.HasText == true )
            {
                uiReadOnlyString( "Format", _node->textField.Text );
                uiReadOnlyString( "Text", _node->textField.Text );
            }

            uiEditorString( "FontName", _node->textField.FontName );
            uiReadOnlyString( "Total FontName", _node->textField.TotalFontName );
            uiEditorColor( "FontColor", _node->textField.FontColor );
            uiReadOnlyColor( "TotalFontColor", _node->textField.TotalFontColor );
            uiEditorVec1f( "LineOffset", _node->textField.LineOffset );
            uiReadOnlyVec1f( "TotalLineOffset", _node->textField.TotalLineOffset );
            uiEditorVec1f( "CharOffset", _node->textField.CharOffset );
            uiReadOnlyVec1f( "TotalCharOffset", _node->textField.TotalCharOffset );
            uiEditorVec1f( "CharScale", _node->textField.CharScale );
            uiReadOnlyVec1f( "TotalCharScale", _node->textField.TotalCharScale );
            uiEditorListBox( "HorizontAlign", _node->textField.HorizontAlign, { "Left", "Center", "Right", "None" }, 4 );
            uiReadOnlyListBox( "TotalHorizontAlign", _node->textField.TotalHorizontAlign, { "Left", "Center", "Right", "None" }, 4 );
            uiEditorListBox( "VerticalAlign", _node->textField.VerticalAlign, { "Bottom", "Center", "Top", "None" }, 4 );
            uiReadOnlyListBox( "TotalVerticalAlign", _node->textField.TotalVerticalAlign, { "Bottom", "Center", "Top", "None" }, 4 );
            uiEditorVec1U( "MaxCharCount", _node->textField.MaxCharCount );
            uiEditorBool( "Pixelsnap", _node->textField.Pixelsnap );
        }

        if( _node->isTypeMovie2 && ImGui::CollapsingHeader( "Movie2:", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            uiReadOnlyString( "Composition Name", _node->movie2.CompositionName );
            uiEditorString( "AliasEnvironment", _node->movie2.TextAliasEnvironment );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::OnConnectButton()
    {
        m_serverAddressCopy = m_serverAddress;
        m_serverPortCopy = m_serverPort;

        m_connectionStatus = ConnectionStatus::ConnectionRequested;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::OnDisconnectButton()
    {
        m_connectionStatus = ConnectionStatus::DisconnectionRequested;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::OnSelectNode( DebuggerNode * _node )
    {
        if( m_selectedNode != _node )
        {
            m_selectedNode = _node;

            if( m_selectedNode != nullptr )
            {
                Vector<uint32_t> path = this->CollectNodePath( m_selectedNode );

                m_selectedNodePath = this->PathToString( path );
            }
            else
            {
                m_selectedNodePath = "-";
            }

            m_lastSelectedNodePath = m_selectedNodePath;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::OnPauseButton()
    {
        m_pauseRequested = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::NetworkLoop()
    {
        while( m_shutdown == false )
        {
            if( m_connectionStatus == ConnectionStatus::ConnectionRequested )
            {
                this->ConnectToServer();
            }
            else if( m_connectionStatus == ConnectionStatus::DisconnectionRequested )
            {
                this->DisconnectFromServer();
            }
            else if( m_connectionStatus == ConnectionStatus::Connected )
            {
                this->SendNetworkData();
                this->ReceiveNetworkData();
            }

            std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::ConnectToServer()
    {
        zed_net_address_t address;
        if( 0 != zed_net_get_address( &address, m_serverAddressCopy.c_str(), m_serverPortCopy ) )
        {
            m_connectionStatus = ConnectionStatus::ConnectionFailed;
            return;
        }

        zed_net_socket_t socket;
        zed_net_tcp_socket_open( &socket, 0, 0, 0 );

        if( zed_net_tcp_connect( &socket, address ) )
        {
            zed_net_socket_close( &socket);
            m_connectionStatus = ConnectionStatus::ConnectionFailed;
            return;
        }

        m_socket = socket;
        m_connectionStatus = ConnectionStatus::Connected;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DisconnectFromServer()
    {
        zed_net_socket_close( &m_socket);
        m_connectionStatus = ConnectionStatus::Disconnected;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::SendNetworkData()
    {
        m_dataMutex.lock();
        {
            if( !m_outgoingPackets.empty() )
            {
                for( const NodeDebuggerPacket & p : m_outgoingPackets )
                {
                    zed_net_tcp_socket_send( &m_socket, p.payload.data(), static_cast<int>(p.payload.size()) );
                }

                m_outgoingPackets.clear();
            }
        }
        m_dataMutex.unlock();
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::ReceiveNetworkData()
    {
        const bool hasData = zed_net_ext_tcp_wait_for_data( &m_socket, 100 );
        if( !hasData )
        {
            return;
        }

        static const int BUFFER_SIZE = 4096;
        uint8_t buffer[BUFFER_SIZE];

        int bytesReceived = 0;
        bool shouldDisconnect = false;
        do
        {
            bytesReceived = zed_net_tcp_socket_receive( &m_socket, buffer, BUFFER_SIZE );
            if( bytesReceived )
            {
                m_receivedData.insert( m_receivedData.end(), &buffer[0], &buffer[bytesReceived] );
            }
            else
            {
                shouldDisconnect = true;
            }
        } while( !shouldDisconnect && bytesReceived == BUFFER_SIZE );

        if( shouldDisconnect )
        {
            this->DisconnectFromServer();

            return;
        }

        // check if we have read something
        if( m_receivedData.size() > sizeof( PacketHeader ) )
        {
            MutexLocker lock( m_dataMutex );

            // check if we have enough data to form a packet
            PacketHeader* hdr = reinterpret_cast<PacketHeader *>(m_receivedData.data());

            // received garbage - nothing fancy, just disconnect
            if( hdr->magic != PACKET_MAGIC )
            {
                this->DisconnectFromServer();

                return;
            }

            while( hdr != nullptr && hdr->compressedSize <= (m_receivedData.size() - sizeof( PacketHeader )) )
            {
                // received garbage - nothing fancy, just disconnect
                if( hdr->magic != PACKET_MAGIC )
                {
                    this->DisconnectFromServer();

                    return;
                }

                const size_t dataSizeWithHeader = hdr->compressedSize + sizeof( PacketHeader );

                NodeDebuggerPacket packet;
                this->UncompressPacket( packet, *hdr, m_receivedData.data() + sizeof( PacketHeader ) );

                m_incomingPackets.emplace_back( packet );

                // now remove this packet data from the buffer
                const size_t newSize = m_receivedData.size() - dataSizeWithHeader;
                if( newSize )
                {
                    memmove( m_receivedData.data(), m_receivedData.data() + dataSizeWithHeader, newSize );
                    m_receivedData.resize( newSize );

                    hdr = reinterpret_cast<PacketHeader *>(m_receivedData.data());
                }
                else
                {
                    m_receivedData.clear();
                    hdr = nullptr;
                }

                if( newSize <= sizeof( PacketHeader ) )
                {
                    break;
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::SendXML( const pugi::xml_document & _doc )
    {
        if( m_connectionStatus != ConnectionStatus::Connected )
        {
            return;
        }

        NodeDebuggerPacket packet;

        MyXMLWriter writer( packet.payload );

#ifdef _DEBUG
        const unsigned int xmlFlags = pugi::format_indent;
#else
        const unsigned int xmlFlags = pugi::format_raw;
#endif
        _doc.save( writer, "  ", xmlFlags, pugi::encoding_utf8 );

        if( !packet.payload.empty() )
        {
            const uint32_t payloadLength = static_cast<uint32_t>(packet.payload.size());

            PacketHeader hdr;
            hdr.magic = PACKET_MAGIC;

            this->CompressPacket( packet, hdr);

            Detail::InsertPacketHeader( packet.payload, hdr );

            m_outgoingPackets.emplace_back( packet );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::SendChangedNode( const DebuggerNode & _node )
    {
        pugi::xml_document doc;

        pugi::xml_node packetNode = doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "Node" );

        pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

        pugi::xml_node xmlNode = payloadNode.append_child( "Node" );

        String pathStr = PathToString( CollectNodePath( &_node ) );

        xmlNode.append_attribute( "path" ).set_value( pathStr.c_str() );
        xmlNode.append_attribute( "name" ).set_value( _node.name.c_str() );
        xmlNode.append_attribute( "type" ).set_value( _node.type.c_str() );

        _node.serialize( xmlNode );

        if( _node.transformationProxy == false )
        {
            pugi::xml_node xmlTransformation = xmlNode.append_child( "Transformation" );

            _node.transformation.serialize( xmlTransformation );
        }

        if( _node.hasRender )
        {
            pugi::xml_node xmlRender = xmlNode.append_child( "Render" );

            _node.render.serialize( xmlRender );
        }

        if( _node.hasAnimation )
        {
            pugi::xml_node xmlAnimation = xmlNode.append_child( "Animation" );

            _node.animation.serialize( xmlAnimation );
        }

        if( _node.isTypeTextField )
        {
            pugi::xml_node xmlTextField = xmlNode.append_child( "Type:TextField" );

            _node.textField.serialize( xmlTextField );
        }

        if( _node.isTypeMovie2 )
        {
            pugi::xml_node xmlMovie2 = xmlNode.append_child( "Type:Movie2" );

            _node.movie2.serialize( xmlMovie2 );
        }

        this->SendXML( doc );

        if( m_updateSceneOnChange == true )
        {
            this->SendSceneRequest();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::SendNodeSelection(const String & _path)
    {
        pugi::xml_document doc;

        pugi::xml_node packetNode = doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "Selection" );

        pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

        pugi::xml_node xmlNode = payloadNode.append_child( "Path" );

        xmlNode.append_attribute( "value" ).set_value( _path.c_str() );

        this->SendXML( doc );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::SendGameControlCommand( const String & _command )
    {
        pugi::xml_document doc;

        pugi::xml_node packetNode = doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "GameControl" );

        pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

        pugi::xml_node xmlNode = payloadNode.append_child( "Command" );

        xmlNode.append_attribute( "value" ).set_value( _command.c_str() );

        this->SendXML( doc );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::SendSceneRequest()
    {
        this->SendGameControlCommand( "scene" );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::SendPauseRequest()
    {
        this->SendGameControlCommand( "pause" );
    }
    //////////////////////////////////////////////////////////////////////////
}
