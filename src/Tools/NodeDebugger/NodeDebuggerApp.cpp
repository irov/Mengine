#include "NodeDebuggerApp.h"

#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Kernel/Stringstream.h"
#include "Kernel/Assertion.h"

#include "Config/StdIO.h"
#include "Config/StdString.h"

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

//////////////////////////////////////////////////////////////////////////
extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    void * LZ4_malloc( size_t _size )
    {
        void * p = Mengine::Helper::allocateMemory( _size, "lz4" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void * LZ4_calloc( size_t _count, size_t _size )
    {
        void * p = Mengine::Helper::callocateMemory( _count, _size, "lz4" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void  LZ4_free( void * _ptr )
    {
        Mengine::Helper::deallocateMemory( _ptr, "lz4" );
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static bool zed_net_ext_tcp_wait_for_data( zed_net_socket_t * _socket, const int _timeoutMs )
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
        , m_selectedArrowNode( nullptr )
        , m_selectedSceneNode( nullptr )
        , m_selectedPickerableNode( nullptr )
        , m_selectedRenderableNode( nullptr )
        , m_defaultIcon( nullptr )
        , m_currentTab( 0 )
        , m_serverAddress()
        , m_serverPort( 18790 )
        , m_serverAddressCopy()
        , m_serverPortCopy( 0 )
        , m_connectionStatus( ConnectionStatus::Disconnected )
        , m_invalidateSelectedTab( true )
        , m_arrow( nullptr )
        , m_scene( nullptr )
        , m_scenePickerable( nullptr )
        , m_sceneRenderable( nullptr )
        , m_sceneUpdateFreq( 0 )
        , m_sceneUpdateTimer( 0.0 )
        , m_updateSceneOnChange( false )
        , m_pauseRequested( false )
        , m_memoryTotal( 0 )
        , m_AvailableTextureMemory( 0 )
        , m_TextureMemoryUse( 0 )
        , m_TextureCount( 0 )
        , m_SoundSourcesCount( 0 )
        , m_SoundBuffersCount( 0 )
        , m_selectedNodeInCollapseHeader( nullptr )
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

        // Setup Dear ImGui binding
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::GetIO().IniFilename = nullptr; // disable "imgui.ini"
        //ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; // tell ImGui to not interfere with our cursors

        ImGui_ImplOpenGL3_Init();

        ImGui_ImplGlfw_InitForOpenGL( m_window, true );

        glfwSetMouseButtonCallback( m_window, ImGui_ImplGlfw_MouseButtonCallback );
        glfwSetScrollCallback( m_window, ImGui_ImplGlfw_ScrollCallback );
        glfwSetKeyCallback( m_window, ImGui_ImplGlfw_KeyCallback );
        glfwSetCharCallback( m_window, ImGui_ImplGlfw_CharCallback );

        glfwSetWindowSizeCallback( m_window, []( GLFWwindow * _wnd, int _width, int _height )
        {
            NodeDebuggerApp * _this = reinterpret_cast<NodeDebuggerApp *>(glfwGetWindowUserPointer( _wnd ));
            if( _this != nullptr )
            {
                _this->Resize( _width, _height );
            }
        } );

        ImGui::StyleColorsClassic();
        //ImGuiExt::SetBrightStyle();

        m_shutdown = false;
        m_networkThread = std::thread( &NodeDebuggerApp::NetworkLoop, this );

        // Create tabs
        m_tabs.push_back( {
            "scene",
            "Scene Debugger",
            true,
            [this]()
        {
            this->DoUISceneDebuggerTab();
        }} );
        m_tabs.push_back( {
            "memory",
            "Memory",
            true,
            [this]()
        {
            this->DoUIMemoryTab();
        }} );
        m_tabs.push_back( {
            "leak",
            "Objects Leak",
            true,
            [this]()
        {
            this->DoUIObjectsLeakTab();
        }} );
        m_tabs.push_back( {
            "network",
            "Network",
            true,
            [this]()
        {
            this->DoUINetwork();
        }} );
        m_tabs.push_back( {
            "settings",
            "Settings",
            true,
            [this]()
        {
            this->DoUISettingsTab();
        }} );

        // if requested to auto-connect, then do so
        if( _address.empty() == false && _port != 0 )
        {
            m_serverAddress = _address;
            m_serverPort = _port;

            this->OnConnectButton();
        }

        ImGui_ImplOpenGL3_CreateFontsTexture();
        ImGui_ImplOpenGL3_CreateDeviceObjects();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::Loop()
    {
        double lastTimerValue = glfwGetTime();

        while( GL_FALSE == glfwWindowShouldClose( m_window ) )
        {
            const double currentTimerValue = glfwGetTime();
            const double dt = currentTimerValue - lastTimerValue;
            lastTimerValue = currentTimerValue;

            this->Update( dt );

            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();

            ImGui::NewFrame();

            glClearColor( 0.412f, 0.796f, 1.0f, 1.0f );
            glClear( GL_COLOR_BUFFER_BIT );

            this->DoUI();

            ImGui::Render();

            ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

            glfwSwapBuffers( m_window );

            std::this_thread::sleep_for( std::chrono::microseconds( 1 ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::Shutdown()
    {
        m_shutdown = true;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();

        glfwTerminate();
        zed_net_shutdown();

        m_networkThread.join();

        if( m_arrow != nullptr )
        {
            this->DestroyNode( m_arrow );
            m_arrow = nullptr;
        }

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

        if( m_selectedNodeInCollapseHeader != nullptr )
        {
            this->DestroyNode( m_selectedNodeInCollapseHeader );
            m_selectedNodeInCollapseHeader = nullptr;
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
            if( m_arrow != nullptr )
            {
                this->DestroyNode( m_arrow );
                m_arrow = nullptr;
            }

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
                NodeDebuggerPacket packet = std::move( m_incomingPackets.front() );
                m_incomingPackets.pop_front();

                this->ProcessPacket( packet );
            }

            if( m_invalidateSelectedTab == true )
            {
                this->SendChangedTab( m_selectedTab );

                m_invalidateSelectedTab = false;
            }

            if( m_selectedTab == "scene" )
            {
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

                    const double updateInterval = 1.0 / static_cast<double>(m_sceneUpdateFreq);
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
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::CompressPacket( NodeDebuggerPacket & _packet, PacketHeader & _hdr )
    {
        Blobject::size_type payloadSize = _packet.payload.size();

        if( payloadSize < 1024 )
        {
            _hdr.compressedSize = static_cast<uint32_t>(payloadSize);
            _hdr.uncompressedSize = 0; // packet is not compressed
        }
        else
        {
            const size_t maxCompressedSize = ::LZ4_compressBound( static_cast<int>(payloadSize) );
            Mengine::Blobject compressedPayload( maxCompressedSize );

            int result = ::LZ4_compress_default( reinterpret_cast<char *>(_packet.payload.data()), reinterpret_cast<char *>(compressedPayload.data()), static_cast<int>(payloadSize), static_cast<int>(maxCompressedSize) );

            if( result < 0 || payloadSize < (Blobject::size_type)result )
            {
                _hdr.compressedSize = static_cast<uint32_t>(payloadSize);
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

            const int result = ::LZ4_decompress_safe( reinterpret_cast<const char *>(_receivedData), reinterpret_cast<char *>(_packet.payload.data()), static_cast<int>(_hdr.compressedSize), static_cast<int>(_hdr.uncompressedSize) );
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

        if( typeStr == "Arrow" )
        {
            this->ReceiveArrow( payloadNode );
        }
        else if( typeStr == "Scene" )
        {
            this->ReceiveScene( payloadNode );
        }
        else if( typeStr == "Pickerable" )
        {
            this->ReceivePickerable( payloadNode );
        }
        else if( typeStr == "Renderable" )
        {
            this->ReceiveRenderable( payloadNode );
        }
        else if( typeStr == "Memory" )
        {
            this->ReceiveMemory( payloadNode );
        }
        else if( typeStr == "ObjectsLeak" )
        {
            this->ReceiveObjectsLeak( payloadNode );
        }
        else if( typeStr == "Settings" )
        {
            this->ReceiveSettings( payloadNode );
        }
        else if( typeStr == "Network" )
        {
            this->ReceiveNetwork( payloadNode );
        }
        else if( typeStr == "SelectedNode" )
        {
            this->ReceiveSelectedNode( payloadNode );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::ReceiveArrow( const pugi::xml_node & _xmlContainer )
    {
        pugi::xml_node xmlNode = _xmlContainer.first_child();

        if( xmlNode )
        {
            if( m_arrow != nullptr )
            {
                DestroyNode( m_arrow );
            }

            m_arrow = new DebuggerNode();
            m_arrow->parent = nullptr;

            this->DeserializeNode( xmlNode, m_arrow );

            Vector<uint32_t> path = this->StringToPath( m_lastSelectedNodePath );

            m_selectedArrowNode = this->PathToNode( m_arrow, path );
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

            Vector<uint32_t> path = this->StringToPath( m_lastSelectedNodePath );

            m_selectedSceneNode = this->PathToNode( m_scene, path );
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

            Vector<uint32_t> path = this->StringToPath( m_lastSelectedNodePath );

            m_selectedPickerableNode = this->PathToNode( m_scenePickerable, path );
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

            Vector<uint32_t> path = this->StringToPath( m_lastSelectedNodePath );

            m_selectedRenderableNode = this->PathToNode( m_sceneRenderable, path );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::ReceiveMemory( const pugi::xml_node & _xmlContainer )
    {
        m_memoryTotal = _xmlContainer.attribute( "Total" ).as_uint();
        m_AvailableTextureMemory = _xmlContainer.attribute( "AvailableTextureMemory" ).as_uint();
        m_TextureMemoryUse = _xmlContainer.attribute( "TextureMemoryUse" ).as_uint();
        m_TextureCount = _xmlContainer.attribute( "TextureCount" ).as_uint();
        m_SoundSourcesCount = _xmlContainer.attribute( "SoundSourcesCount" ).as_uint();
        m_SoundBuffersCount = _xmlContainer.attribute( "SoundBuffersCount" ).as_uint();

        pugi::xml_node xml_allocators = _xmlContainer.child( "Allocators" );

        for( const pugi::xml_node & child : xml_allocators )
        {
            const pugi::char_t * name = child.attribute( "Name" ).value();
            uint32_t size = child.attribute( "Count" ).as_uint();

            m_memory[name] = size;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::ReceiveObjectsLeak( const pugi::xml_node & _xmlContainer )
    {
        m_objectLeaks.clear();

        m_objectLeakGeneration = _xmlContainer.attribute( "Generation" ).value();

        pugi::xml_node xml_leaks = _xmlContainer.child( "Leaks" );

        for( const pugi::xml_node & child : xml_leaks )
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
    void NodeDebuggerApp::ReceiveNetwork( const pugi::xml_node & _xmlContainer )
    {
        m_network.clear();

        pugi::xml_node xml_network_objects = _xmlContainer.child( "Objects" );

        for( const pugi::xml_node & obj : xml_network_objects )
        {
            const pugi::char_t * type = obj.attribute( "Type" ).value();
            const pugi::char_t * url = obj.attribute( "Url" ).value();
            uint32_t id = obj.attribute( "Id" ).as_uint();

            NetworkDesk desk;
            desk.type = type;
            desk.url = url;
            desk.id = id;

            m_network.emplace_back( desk );
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
    void NodeDebuggerApp::ReceiveSelectedNode( const pugi::xml_node & _xmlContainer )
    {
        if( m_selectedTab == "scene" )
        {
            pugi::xml_node node = _xmlContainer.child( "Node" );

            uint32_t id = node.attribute( "SelectedNodeId" ).as_uint();

            String pathToRoot = node.attribute( "PathToRoot" ).value();

            String selectedNodeName = node.attribute( "SelectedNodeName" ).value();

            Vector<uint32_t> path = this->StringToPath( pathToRoot );

            m_pathToSelectedNode = path;

            Vector<DebuggerNode *> childrens = m_scene->children;
            DebuggerNode * selectedNode = nullptr;
            for( Vector<uint32_t>::reverse_iterator iter = path.rbegin() + 1; iter != path.rend(); ++iter )
            {
                uint32_t value = *iter;
                Vector<DebuggerNode *>::iterator childrenForUid = std::find_if( childrens.begin(), childrens.end(), [value]( const DebuggerNode * _node )
                {
                    return _node->uid == value;
                } );

                if( childrenForUid == childrens.end() )
                {
                    return;
                }

                selectedNode = *childrenForUid;
                childrens = selectedNode->children;
            }

            m_selectedSceneNode = selectedNode;
            m_selectedNode = selectedNode;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DeserializeNode( const pugi::xml_node & _xmlNode, DebuggerNode * _node )
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

        if( _node->hasRender == true )
        {
            _node->render.deserialize( renderNode );
        }

        pugi::xml_node animationNode = _xmlNode.child( "Animation" );

        _node->hasAnimation = animationNode;

        if( _node->hasAnimation == true )
        {
            _node->animation.deserialize( animationNode );
        }

        pugi::xml_node componentSurface = _xmlNode.child( "Component:Surface" );

        _node->hasComponentSurface = componentSurface;

        if( _node->hasComponentSurface == true )
        {
            _node->componentSurface.deserialize( componentSurface );

            pugi::xml_node componentSurfaceNode = componentSurface.child( "Animation" );

            _node->componentSurface.hasAnimation = componentSurfaceNode;

            if( _node->componentSurface.hasAnimation == true )
            {
                _node->componentSurface.animation.deserialize( componentSurfaceNode );
            }

            pugi::xml_node typeSurfaceImageNode = componentSurface.child( "Type:SurfaceImage" );

            _node->componentSurface.isTypeSurfaceImage = typeSurfaceImageNode;

            if( _node->componentSurface.isTypeSurfaceImage == true )
            {
                _node->componentSurface.surfaceImage.deserialize( typeSurfaceImageNode );

                pugi::xml_node contentNode = typeSurfaceImageNode.child( "Content" );

                _node->componentSurface.surfaceImage.isContent = contentNode;

                if( _node->componentSurface.surfaceImage.isContent == true )
                {
                    _node->componentSurface.surfaceImage.content.deserialize( contentNode );
                }

                pugi::xml_node atlasNode = typeSurfaceImageNode.child( "Atlas" );

                _node->componentSurface.hasAtlas = atlasNode;

                if( _node->componentSurface.hasAtlas == true )
                {
                    _node->componentSurface.atlas.deserialize( atlasNode );

                    pugi::xml_node contentNode = atlasNode.child( "Content" );

                    _node->componentSurface.atlas.isContent = contentNode;

                    if( _node->componentSurface.atlas.isContent )
                    {
                        _node->componentSurface.atlas.content.deserialize( contentNode );
                    }
                }
            }
        }

        pugi::xml_node typeTextFieldNode = _xmlNode.child( "Type:TextField" );

        _node->isTypeTextField = typeTextFieldNode;

        if( _node->isTypeTextField == true )
        {
            _node->textField.deserialize( typeTextFieldNode );
        }

        pugi::xml_node typeMovie2Node = _xmlNode.child( "Type:Movie2" );

        _node->isTypeMovie2 = typeMovie2Node;

        if( _node->isTypeMovie2 == true )
        {
            _node->movie2.deserialize( typeMovie2Node );
        }

        pugi::xml_node typeSpineNode = _xmlNode.child( "Type:Spine" );

        _node->isTypeSpine = typeSpineNode;

        if( _node->isTypeSpine == true )
        {
            _node->spine.deserialize( typeSpineNode );

            pugi::xml_node contentNode = typeSpineNode.child( "Content" );

            _node->spine.isContent = contentNode;

            if( _node->spine.isContent == true )
            {
                _node->spine.content.deserialize( contentNode );
            }
        }

        pugi::xml_node childrenNode = _xmlNode.child( "Children" );

        if( childrenNode )
        {
            for( const pugi::xml_node & child : childrenNode.children() )
            {
                DebuggerNode * childNode = new DebuggerNode();
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

        const DebuggerNode * current = _node;
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
            const Char * ptr = _pathStr.c_str();

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
                    uid += static_cast<uint32_t>(*ptr - '0');
                }

                ++ptr;
            }
        }

        return path;
    }
    //////////////////////////////////////////////////////////////////////////
    DebuggerNode * NodeDebuggerApp::PathToNode( DebuggerNode * _base, const Vector<uint32_t> & _path )
    {
        if( _path.empty() == true )
        {
            return nullptr;
        }

        DebuggerNode * node = _base;

        Vector<uint32_t>::const_iterator it = _path.begin(), end = _path.end();
        for( ; it != end; ++it )
        {
            const uint32_t nextUid = *it;
            Vector<DebuggerNode *>::const_iterator found = std::find_if( node->children.begin(), node->children.end(), [nextUid]( const DebuggerNode * _n )->bool
            {
                return _n->uid == nextUid;
            } );

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
            return node;
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DestroyNode( DebuggerNode * _node )
    {
        m_selectedNode = nullptr;

        if( m_selectedSceneNode == _node )
        {
            m_selectedSceneNode = nullptr;
        }

        if( m_selectedArrowNode == _node )
        {
            m_selectedArrowNode = nullptr;
        }

        if( m_selectedPickerableNode == _node )
        {
            m_selectedPickerableNode = nullptr;
        }

        if( m_selectedRenderableNode == _node )
        {
            m_selectedRenderableNode = nullptr;
        }

        for( DebuggerNode * n : _node->children )
        {
            DestroyNode( n );
        }

        _node->children.resize( 0 );
        delete _node;
    }
    //////////////////////////////////////////////////////////////////////////
    const CachedImage * NodeDebuggerApp::GetIconImage( const String & _name )
    {
        const CachedImage * result = nullptr;

        Vector<CachedImage>::const_iterator it = std::find_if( m_imagesCache.begin(), m_imagesCache.end(), [&_name]( const CachedImage & _ci )->bool
        {
            return _ci.name == _name;
        } );

        if( it != m_imagesCache.end() )
        {
            result = &(*it);
        }
        else
        {
            int width, height, bpp;
            stbi_uc * data = stbi_load( _name.c_str(), &width, &height, &bpp, STBI_rgb_alpha );
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

                m_imagesCache.push_back( {_name, static_cast<uintptr_t>(texture), static_cast<size_t>(width), static_cast<size_t>(height)} );

                result = &m_imagesCache.back();
            }
        }

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
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

        for( const pugi::xml_node & iconNode : mainNode.children() )
        {
            pugi::xml_attribute attrName = iconNode.attribute( "name" );
            pugi::xml_attribute attrImage = iconNode.attribute( "image" );

            if( attrName && attrImage )
            {
                const CachedImage * ci = GetIconImage( attrImage.as_string() );
                if( ci != nullptr )
                {
                    pugi::xml_attribute attrX = iconNode.attribute( "x" );
                    pugi::xml_attribute attrY = iconNode.attribute( "y" );

                    float x = attrX ? attrX.as_float() : 0.f;
                    float y = attrY ? attrY.as_float() : 0.f;

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
        Vector<NodeIcon>::const_iterator it = std::find_if( m_icons.begin(), m_icons.end(), []( const NodeIcon & _ni )
        {
            return _ni.name == "?";
        } );

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
    const NodeIcon * NodeDebuggerApp::GetIconForNodeType( const String & _nodeType )
    {
        Vector<NodeIcon>::const_iterator it = std::find_if( m_icons.begin(), m_icons.end(), [&_nodeType]( const NodeIcon & _ni )->bool
        {
            return _ni.name == _nodeType;
        } );

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
                for( TabDescriptor & tab : m_tabs )
                {
                    if( ImGui::BeginTabItem( tab.title.c_str() ) )
                    {
                        m_selectedTab = tab.name;

                        if( m_cacheSelectedTab != m_selectedTab )
                        {
                            m_cacheSelectedTab = m_selectedTab;
                            m_invalidateSelectedTab = true;
                        }

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
            ImGui::PushItemFlag( ImGuiItemFlags_ReadOnly, m_updateSceneOnChange );
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

        static int SceneTagId = 1;

        if( ImGui::CollapsingHeader( "Type:" ) )
        {
            ImGui::RadioButton( "Arrow", &SceneTagId, 0 ); ImGui::SameLine();
            ImGui::RadioButton( "Full", &SceneTagId, 1 ); ImGui::SameLine();
            ImGui::RadioButton( "Picker", &SceneTagId, 2 ); ImGui::SameLine();
            ImGui::RadioButton( "Render", &SceneTagId, 3 );
        }

        switch( SceneTagId )
        {
        case 0:
            {
                if( ImGui::CollapsingHeader( "Arrow:", ImGuiTreeNodeFlags_DefaultOpen ) )
                {
                    if( m_arrow )
                    {
                        if( ImGui::BeginChild( "ArrowTree", ImVec2( 0, 200.f ), false, ImGuiWindowFlags_HorizontalScrollbar ) )
                        {
                            this->DoNodeElement( m_arrow, &m_selectedArrowNode, "ArrowFull" );

                            m_selectedNode = m_selectedArrowNode;
                        }
                        ImGui::EndChild();
                    }
                }
            }break;
        case 1:
            {
                if( ImGui::CollapsingHeader( "Scene:", ImGuiTreeNodeFlags_DefaultOpen ) )
                {
                    if( m_scene )
                    {
                        if( ImGui::BeginChild( "SceneTree", ImVec2( 0, 0.f ), false, ImGuiWindowFlags_HorizontalScrollbar ) )
                        {
                            this->DoNodeElement( m_scene, &m_selectedSceneNode, "SceneFull" );

                            m_selectedNode = m_selectedSceneNode;

                            m_pathToSelectedNode.clear();
                        }
                        ImGui::EndChild();
                    }
                }
            }break;
        case 2:
            {
                if( ImGui::CollapsingHeader( "Pickerable:", ImGuiTreeNodeFlags_DefaultOpen ) )
                {
                    if( m_scenePickerable )
                    {
                        if( ImGui::BeginChild( "SceneTree", ImVec2( 0, 0 ), false, ImGuiWindowFlags_HorizontalScrollbar ) )
                        {
                            this->DoNodeElement( m_scenePickerable, &m_selectedPickerableNode, "ScenePickerable" );

                            m_selectedNode = m_selectedPickerableNode;
                        }
                        ImGui::EndChild();
                    }
                }
            }break;
        case 3:
            {
                if( ImGui::CollapsingHeader( "Renderable:", ImGuiTreeNodeFlags_DefaultOpen ) )
                {
                    if( m_sceneRenderable )
                    {
                        if( ImGui::BeginChild( "SceneTree", ImVec2( 0, 0 ), false, ImGuiWindowFlags_HorizontalScrollbar ) )
                        {
                            this->DoNodeElement( m_sceneRenderable, &m_selectedRenderableNode, "SceneRenderable" );

                            m_selectedNode = m_selectedRenderableNode;
                        }
                        ImGui::EndChild();
                    }
                }
            }break;
        }

        ImGui::NextColumn();

        if( ImGui::BeginChild( "Panel" ) )
        {
            if( m_selectedNode != nullptr )
            {
                this->DoNodeProperties( m_selectedNode );
            }
        }
        ImGui::EndChild();

        ImGui::Columns( 1 );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DoUIMemoryTab()
    {
        ImGui::Separator();

        ImGui::TextColored( ImVec4( 0.f, 1.f, 0.f, 1.f ), "Total: %ug %um %ukb %ub"
            , m_memoryTotal / 1000000000
            , (m_memoryTotal % 1000000000) / 1000000
            , (m_memoryTotal % 1000000) / 1000
            , m_memoryTotal % 1000
        );

        ImGui::TextColored( ImVec4( 0.f, 1.f, 0.f, 1.f ), "Texture Available: %ug %um %ukb %ub"
            , m_AvailableTextureMemory / 1000000000
            , (m_AvailableTextureMemory % 1000000000) / 1000000
            , (m_AvailableTextureMemory % 1000000) / 1000
            , m_AvailableTextureMemory % 1000
        );

        ImGui::TextColored( ImVec4( 0.f, 1.f, 0.f, 1.f ), "Texture Use: %ug %um %ukb %ub"
            , m_TextureMemoryUse / 1000000000
            , (m_TextureMemoryUse % 1000000000) / 1000000
            , (m_TextureMemoryUse % 1000000) / 1000
            , m_TextureMemoryUse % 1000
        );

        ImGui::TextColored( ImVec4( 0.f, 1.f, 0.f, 1.f ), "Texture Count: %u"
            , m_TextureCount
        );

        ImGui::TextColored( ImVec4( 0.f, 1.f, 0.f, 1.f ), "Sound Sources Count: %u"
            , m_SoundSourcesCount
        );

        ImGui::TextColored( ImVec4( 0.f, 1.f, 0.f, 1.f ), "Sound Buffers Count: %u"
            , m_SoundBuffersCount
        );

        ImGui::Separator();

        struct MemoryDesc
        {
            String name;
            uint32_t size;
        };

        Vector<MemoryDesc> vmemory;

        for( auto && [name, size] : m_memory )
        {
            vmemory.emplace_back( MemoryDesc{name, size} );
        }

        std::stable_sort( vmemory.begin(), vmemory.end(), []( const MemoryDesc & l, const MemoryDesc & r )
        {
            return l.size > r.size;
        } );

        for( const MemoryDesc & desc : vmemory )
        {
            if( desc.size > 1024 * 1024 )
            {
                ImGui::TextColored( ImVec4( 1.f, 0.25f, 0.5f, 1.f ), "Allocator: %s [total %umb %ukb]"
                    , desc.name.c_str()
                    , desc.size / (1024 * 1024)
                    , desc.size / 1024 % 1024
                );
            }
            else if( desc.size > 1024 )
            {
                ImGui::TextColored( ImVec4( 0.5f, 1.f, 0.25f, 1.f ), "Allocator: %s [total %ukb]"
                    , desc.name.c_str()
                    , desc.size / 1024
                );
            }
            else
            {
                ImGui::TextColored( ImVec4( 0.f, 1.f, 1.f, 1.f ), "Allocator: %s [total %u byte]"
                    , desc.name.c_str()
                    , desc.size
                );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DoUIObjectsLeakTab()
    {
        uint32_t leaksCount = 0;

        for( auto && [type, objects] : m_objectLeaks )
        {
            uint32_t count = (uint32_t)objects.size();

            leaksCount += count;
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

            Char label_node[32] = {'\0'};
            MENGINE_SPRINTF( label_node, "##tree_node_%u"
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
                        , (Char *)leak.message.data()
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
                                , (Char *)parent.message.data()
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
    void NodeDebuggerApp::DoUINetwork()
    {
        ImGui::Separator();

        if( ImGui::BeginChild( "Network", ImVec2( 1200, 1000 ), false, ImGuiWindowFlags_None ) )
        {
            for( const NetworkDesk & desk : m_network )
            {
                if( desk.type == "Request" )
                {
                    Char label_node[32] = {'\0'};
                    MENGINE_SPRINTF( label_node, "Url: ##request%u"
                        , desk.id
                    );

                    bool openNode = ImGui::TreeNode( label_node );

                    ImGui::SameLine();

                    Char label[32] = {'\0'};
                    MENGINE_SPRINTF( label, "##url%u", desk.id );

                    ImGui::InputText( label, (Char *)desk.url.data(), desk.url.size(), ImGuiInputTextFlags_ReadOnly );

                    if( openNode == true )
                    {
                        ImGui::Separator();

                        this->ShowResponseDataForId( desk.id );

                        ImGui::Separator();

                        ImGui::TreePop();
                    }
                }
            }
        }

        ImGui::EndChild();
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::ShowResponseDataForId( uint32_t _id )
    {
        VectorNetwork::const_iterator responseIterator = std::find_if( m_network.cbegin(), m_network.cend(), [_id]( const NetworkDesk & _desk )
        {
            return (_id == _desk.id) && (_desk.type == "Response");
        } );

        Char label[32] = {'\0'};
        MENGINE_SPRINTF( label, "copy##%u"
            , _id
        );

        ImGui::Button( label );

        bool clicked = ImGui::IsItemClicked( 0 );

        if( responseIterator == m_network.end() )
        {
            ImGui::Text( "Not receive response for request ID: %ug", _id );

            if( clicked == true )
            {
                ImGui::SetClipboardText( "Not receive response for request" );
            }
        }
        else
        {
            const Char * responseStr = responseIterator->url.c_str();
            String::size_type responseStrSize = responseIterator->url.size();
            jpp::object responseJpp = jpp::load( responseStr, responseStrSize, nullptr, nullptr );

            uint32_t labelCounter = 0;
            this->ShowResponseJpp( responseJpp, 0, &labelCounter );

            if( clicked == true )
            {
                String jppstr;
                jpp::dump( responseJpp, []( const char * _buffer, jpp::jpp_size_t _size, void * _ud )
                {
                    String * jppstr = (String *)_ud;

                    jppstr->append( _buffer, _size );

                    return 0;
                }, &jppstr );
                ImGui::SetClipboardText( jppstr.c_str() );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::addSpacesWithMultiplier( String * const _out, uint32_t _spacesCount, uint32_t _multiplier )
    {
        _out->append( _spacesCount * _multiplier, ' ' );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::ShowResponseJpp( const jpp::object & _object, uint32_t _spaceCounter, uint32_t * const _labelCounter )
    {
        String spaces;
        this->addSpacesWithMultiplier( &spaces, 2, _spaceCounter );

        jpp::e_type jppType = _object.type();

        switch( jppType )
        {
        case jpp::e_type::JPP_OBJECT:
            {
                uint32_t objectElementsEnumerator = 0;

                for( auto && [key, value] : _object )
                {
                    jpp::e_type valueType = value.type();
                    if( valueType != jpp::e_type::JPP_OBJECT && valueType != jpp::e_type::JPP_ARRAY )
                    {
                        ImGui::Text( "%s%s:", spaces.c_str(), key );

                        ImGui::SameLine();

                        this->ShowResponseJpp( value, _spaceCounter, _labelCounter );
                    }
                    else
                    {
                        Char label[32] = {'\0'};
                        MENGINE_SPRINTF( label, "##o%u%u"
                            , *_labelCounter
                            , objectElementsEnumerator
                        );

                        bool openTree = ImGui::TreeNodeEx( label, ImGuiTreeNodeFlags_None );

                        ImGui::SameLine();
                        ImGui::Text( "%s%s:", spaces.c_str(), key );
                        ImGui::SameLine();

                        Char buffer_value[256] = {'\0'};

                        struct buffer_desc
                        {
                            Char * buffer;
                            size_t capacity;
                            bool end;
                        };

                        buffer_desc desc;
                        desc.buffer = buffer_value;
                        desc.capacity = 128;
                        desc.end = false;

                        jpp::dump_compact( value, []( const char * _buffer, jpp::jpp_size_t _size, void * _ud )
                        {
                            buffer_desc * desc = (buffer_desc *)_ud;

                            if( desc->capacity > _size )
                            {
                                desc->capacity -= _size;
                            }
                            else
                            {
                                _size = desc->capacity;
                                desc->capacity = 0;
                                desc->end = true;
                            }

                            MENGINE_STRNCAT( desc->buffer, _buffer, _size );

                            return 0;
                        }, &desc );

                        if( desc.end == true )
                        {
                            MENGINE_STRCAT( desc.buffer, "..." );
                        }

                        ImGui::TextColored( ImVec4( 1.0f, 1.0f, 1.0f, 0.5f ), "%s", buffer_value );

                        if( openTree == true )
                        {
                            ++_spaceCounter;
                            this->ShowResponseJpp( value, _spaceCounter, _labelCounter );
                            --_spaceCounter;

                            ImGui::TreePop();
                        }
                    }

                    ++objectElementsEnumerator;
                }
            }break;
        case jpp::e_type::JPP_ARRAY:
            {
                uint32_t arrayElementsEnumerator = 0;

                for( const jpp::object & element : jpp::array( _object ) )
                {
                    jpp::e_type elementType = element.type();

                    if( elementType != jpp::e_type::JPP_OBJECT && elementType != jpp::e_type::JPP_ARRAY )
                    {
                        ImGui::TextColored( ImVec4( 1.0f, 1.0f, 1.0f, 0.5f ), "%s %u", spaces.c_str(), arrayElementsEnumerator );

                        ImGui::SameLine();

                        this->ShowResponseJpp( element, _spaceCounter, _labelCounter );
                    }
                    else
                    {
                        Char label[32] = {'\0'};
                        MENGINE_SPRINTF( label, "##a%u%u"
                            , *_labelCounter
                            , arrayElementsEnumerator
                        );

                        if( ImGui::TreeNodeEx( label, ImGuiTreeNodeFlags_None, "%s %u", spaces.c_str(), arrayElementsEnumerator ) == true )
                        {
                            ++_spaceCounter;
                            this->ShowResponseJpp( element, _spaceCounter, _labelCounter );
                            --_spaceCounter;

                            ImGui::TreePop();
                        }
                    }

                    ++arrayElementsEnumerator;
                }
            }break;
        default:
            {
                String valueStr;
                this->GetValueStringForJppType( _object, jppType, &valueStr, _spaceCounter );

                // TODO need make ability for copy text from ImGui::InputText

                ++ * _labelCounter;

                Char label[32] = {'\0'};
                MENGINE_SPRINTF( label, "##v%u"
                    , *_labelCounter
                );

                ImGui::PushItemWidth( 25.f + 8.f * valueStr.size() );
                ImGui::InputText( label, valueStr.data(), valueStr.size(), ImGuiInputTextFlags_ReadOnly );
                ImGui::PopItemWidth();
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::GetValueStringForJppType( const jpp::object & _object, jpp::e_type _jppType, String * _out, uint32_t _spaceCounter )
    {
        MENGINE_UNUSED( _spaceCounter );

        switch( _jppType )
        {
        case jpp::e_type::JPP_INTEGER:
            {
                int32_t valueInteger = _object;
                *_out = Helper::stringFormat( "%d", valueInteger );
            }break;
        case jpp::e_type::JPP_REAL:
            {
                float valueDouble = _object;
                *_out = Helper::stringFormat( "%f", valueDouble );
            }break;
        case jpp::e_type::JPP_FALSE:
            {
                *_out = "false";
            }break;
        case jpp::e_type::JPP_TRUE:
            {
                *_out = "true";
            }break;
        case jpp::e_type::JPP_STRING:
            {
                *_out = (const Char *)_object;
            }break;
        case jpp::e_type::JPP_NULL:
            {
                *_out = "null";
            }break;
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

                        float speed = (min == max) ? 1.f : float( max - min ) * 0.0015f;

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

                bool successful = jpp::dump( desc.json, []( const char * _buffer, jpp::jpp_size_t _size, void * _ud )
                {
                    FILE * f = (FILE *)_ud;

                    size_t writebyte = fwrite( _buffer, _size, 1, f );

                    if( writebyte != 1 )
                    {
                        return -1;
                    }

                    return 0;
                }, f );

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
    void NodeDebuggerApp::DoNodeElement( DebuggerNode * _node, DebuggerNode ** _selectedNode, const String & _tag )
    {
        const ImGuiTreeNodeFlags seletedFlag = (*_selectedNode == _node) ? ImGuiTreeNodeFlags_Selected : 0;
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

        ImGuiTreeNodeFlags flag = _node->children.empty() ? flagsNoChildren : flagsNormal;

        if( std::find( m_pathToSelectedNode.begin(), m_pathToSelectedNode.end(), _node->uid ) != m_pathToSelectedNode.end() )
        {
            ImGui::SetNextItemOpen( true );

            if( _node->uid == m_pathToSelectedNode[0] )
            {
                flag = ImGuiTreeNodeFlags_Selected;
            }
        }

        std::pair<bool, bool> result = ImGuiExt::TreeNodeWithIcon
        (
            iconPtr,
            fullLabel.c_str(),
            flag,
            !_node->enable
        );

        if( result.second == true )
        {
            this->OnSelectNode( _node, _selectedNode );
        }

        if( result.first == true )
        {
            for( DebuggerNode * child : _node->children )
            {
                this->DoNodeElement( child, _selectedNode, _tag );
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

        auto uiReadOnlyBool = [_node]( const char * _caption, bool _prop )
        {
            bool testValue = _prop;

            ImGui::PushItemFlag( ImGuiItemFlags_ReadOnly, true );
            ImGui::PushStyleColor( ImGuiCol_FrameBg, ImVec4( 0.15f, 0.3f, 0.2f, 1.f ) );
            ImGui::Checkbox( _caption, &testValue );
            ImGui::PopStyleColor();
            ImGui::PopItemFlag();
        };

        auto uiEditorVec1I = [_node]( const char * _caption, int32_t & _prop )
        {
            int32_t testValue = _prop;
            bool input = ImGui::DragScalarN( _caption, ImGuiDataType_S32, &testValue, 1, 1.f );

            if( input && testValue != _prop )
            {
                _prop = testValue;
                _node->dirty = true;
            }
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

        auto uiEditorVec1f = [_node]( const char * _caption, float & _prop, bool _enable = true )
        {
            float testValue = _prop;

            if( _enable == false )
            {
                ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
            }

            bool input = ImGui::DragFloat( _caption, &testValue, 0.01f );

            if( _enable == false )
            {
                ImGui::PopItemFlag();
            }

            if( input && testValue != _prop )
            {
                _prop = testValue;
                _node->dirty = true;
            }
        };

        auto uiReadOnlyVec1f = [_node]( const char * _caption, const float & _prop )
        {
            float testValue = _prop;
            ImGui::PushItemFlag( ImGuiItemFlags_ReadOnly, true );
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

        auto uiReadOnlyVec2f = [_node]( const char * _caption, const mt::vec2f & _prop )
        {
            mt::vec2f testValue = _prop;
            ImGui::PushItemFlag( ImGuiItemFlags_ReadOnly, true );
            ImGui::PushStyleColor( ImGuiCol_FrameBg, ImVec4( 0.15f, 0.3f, 0.2f, 1.f ) );
            ImGui::DragFloat2( _caption, testValue.buff() );
            ImGui::PopStyleColor();
            ImGui::PopItemFlag();
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

        auto uiReadOnlyVec3f = [_node]( const char * _caption, const mt::vec3f & _prop )
        {
            mt::vec3f testValue = _prop;
            ImGui::PushItemFlag( ImGuiItemFlags_ReadOnly, true );
            ImGui::PushStyleColor( ImGuiCol_FrameBg, ImVec4( 0.15f, 0.3f, 0.2f, 1.f ) );
            ImGui::DragFloat3( _caption, testValue.buff() );
            ImGui::PopStyleColor();
            ImGui::PopItemFlag();
        };

        auto uiEditorColor = [_node]( const Char * _caption, Color & _prop, bool _enable = true )
        {
            Color testValue = _prop;

            if( _enable == false )
            {
                ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
            }

            bool input = ImGui::ColorEdit4( _caption, testValue.buff() );

            if( _enable == false )
            {
                ImGui::PopItemFlag();
            }

            if( input && testValue != _prop )
            {
                _prop = testValue;
                _node->dirty = true;
            }
        };

        auto uiReadOnlyColor = [_node]( const Char * _caption, const Color & _prop )
        {
            Color testValue = _prop;
            ImGui::PushItemFlag( ImGuiItemFlags_ReadOnly, true );
            ImGui::PushStyleColor( ImGuiCol_FrameBg, ImVec4( 0.15f, 0.3f, 0.2f, 1.f ) );
            ImGui::ColorEdit4( _caption, testValue.buff() );
            ImGui::PopStyleColor();
            ImGui::PopItemFlag();
        };

        auto uiEditorString = [_node]( const char * _caption, String & _prop )
        {
            char testValue[2048] = {'\0'};
            strcpy( testValue, _prop.c_str() );
            testValue[2047] = 0;
            bool input = ImGui::InputText( _caption, testValue, 2048 );

            if( input && _prop != testValue )
            {
                _prop = testValue;
                _node->dirty = true;
            }
        };

        auto uiReadOnlyString = [_node]( const char * _caption, const String & _prop )
        {
            Char testValue[2048] = {'\0'};
            MENGINE_STRNCPY( testValue, _prop.c_str(), 2047 );

            ImGui::PushStyleColor( ImGuiCol_FrameBg, ImVec4( 0.15f, 0.3f, 0.2f, 1.f ) );
            ImGui::InputText( _caption, testValue, 2048, ImGuiInputTextFlags_ReadOnly );
            ImGui::PopStyleColor();
        };

        auto uiReadOnlyUV4 = [_node]( const char * _caption, const mt::uv4f & _prop )
        {
            mt::uv4f testValue = _prop;
            ImGui::PushItemFlag( ImGuiItemFlags_ReadOnly, true );
            ImGui::PushStyleColor( ImGuiCol_FrameBg, ImVec4( 0.15f, 0.3f, 0.2f, 1.f ) );
            ImGui::DragFloat2( _caption, testValue.p0.buff() );
            ImGui::DragFloat2( _caption, testValue.p1.buff() );
            ImGui::DragFloat2( _caption, testValue.p2.buff() );
            ImGui::DragFloat2( _caption, testValue.p3.buff() );
            ImGui::PopStyleColor();
            ImGui::PopItemFlag();
        };

        auto uiEditorListBox = [_node]( const char * _caption, uint32_t & _prop, const std::initializer_list<String> & _items, uint32_t _count )
        {
            int32_t testValue = _prop;
            bool input = ImGui::ListBox( _caption, &testValue, []( void * data, int idx, const char ** out_text )
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

        auto uiReadOnlyListBox = [_node]( const char * _caption, uint32_t _prop, const std::initializer_list<String> & _items, uint32_t _count )
        {
            int32_t testValue = _prop;
            ImGui::PushItemFlag( ImGuiItemFlags_ReadOnly, true );
            ImGui::PushStyleColor( ImGuiCol_FrameBg, ImVec4( 0.15f, 0.3f, 0.2f, 1.f ) );
            ImGui::ListBox( _caption, &testValue, []( void * data, int idx, const char ** out_text )
            {
                *out_text = ((String *)data + idx)->c_str();
                return true;
            }, (void *)_items.begin(), _count );
            ImGui::PopStyleColor();
            ImGui::PopItemFlag();
        };

        if( ImGui::CollapsingHeader( "Node:", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            ImGui::Text( "Name:" );
            ImGui::SameLine();
            ImGui::PushItemWidth( 25.f + 8.f * _node->name.size() );
            ImGui::InputText( "##node_name", _node->name.data(), _node->name.size(), ImGuiInputTextFlags_ReadOnly );
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::Dummy( ImVec2( 20.0f, 5.0f ) );

            ImGui::SameLine();
            ImGui::Text( "Type:" );
            ImGui::SameLine();
            ImGui::PushItemWidth( 25.f + 8.f * _node->type.size() );
            ImGui::InputText( "##node_type", _node->type.data(), _node->type.size(), ImGuiInputTextFlags_ReadOnly );
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::Dummy( ImVec2( 20.0f, 5.0f ) );

            Char uid_text[64];
            MENGINE_SPRINTF( uid_text, "%u", _node->uid );

            ImGui::SameLine();
            ImGui::Text( "UID:" );
            ImGui::SameLine();
            ImGui::PushItemWidth( 25.f + 8.f * MENGINE_STRLEN( uid_text ) );
            ImGui::InputText( "##node_uid", uid_text, MENGINE_STRLEN( uid_text ), ImGuiInputTextFlags_ReadOnly );
            ImGui::PopItemWidth();
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
            uiReadOnlyVec3f( "World Scale", transformation.worldScale );
            uiReadOnlyVec3f( "World Orientation", transformation.worldOrientation );

            ImGui::Spacing();
        }

        if( _node->hasRender && ImGui::CollapsingHeader( "Render:", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            uiReadOnlyBool( "Enable", _node->render.enable );
            uiEditorBool( "Hide", _node->render.hide );
            ImGui::Spacing();
            uiEditorVec1I( "Z Index", _node->render.z_index );
            uiEditorVec1I( "Z Order", _node->render.z_order );
            ImGui::Spacing();
            uiEditorColor( "Local Color", _node->render.local_color );
            uiEditorColor( "Personal Color", _node->render.personal_color );

            if( _node->render.HasExtraRelationRender == true && ImGui::CollapsingHeader( "Extra Relation Render:", ImGuiTreeNodeFlags_DefaultOpen ) )
            {
                ImGui::Spacing();
                ImGui::InputText( "Base Node Name:", _node->name.data(), _node->name.size(), ImGuiInputTextFlags_ReadOnly );
                ImGui::InputText( "Base Node Type:", _node->type.data(), _node->type.size(), ImGuiInputTextFlags_ReadOnly );

                Char uid_text[64] = {'\0'};
                MENGINE_SPRINTF( uid_text, "%u", _node->uid );

                ImGui::InputText( "Base Node UID:", uid_text, MENGINE_STRLEN( uid_text ), ImGuiInputTextFlags_ReadOnly );
            }

            if( _node->render.camera.exist == true && ImGui::CollapsingHeader( "Render Camera:", ImGuiTreeNodeFlags_DefaultOpen ) )
            {
            }

            if( _node->render.viewport.exist == true && ImGui::CollapsingHeader( "Render Viewport:", ImGuiTreeNodeFlags_DefaultOpen ) )
            {
                uiReadOnlyVec2f( "begin", _node->render.viewport.begin );
                uiReadOnlyVec2f( "end", _node->render.viewport.end );
            }

            if( _node->render.transformation.exist == true && ImGui::CollapsingHeader( "Render Transformation:", ImGuiTreeNodeFlags_DefaultOpen ) )
            {
            }

            if( _node->render.scissor.exist == true && ImGui::CollapsingHeader( "Render Scissor:", ImGuiTreeNodeFlags_DefaultOpen ) )
            {
                uiReadOnlyVec2f( "begin", _node->render.scissor.begin );
                uiReadOnlyVec2f( "end", _node->render.scissor.end );
            }

            if( _node->render.target.exist == true && ImGui::CollapsingHeader( "Render Target:", ImGuiTreeNodeFlags_DefaultOpen ) )
            {
            }
        }

        if( _node->hasAnimation && ImGui::CollapsingHeader( "Animation:", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            uiEditorBool( "loop", _node->animation.loop );
            uiReadOnlyBool( "play", _node->animation.play );
            uiReadOnlyBool( "pause", _node->animation.pause );
            uiEditorVec1f( "time", _node->animation.time );
            uiReadOnlyVec1f( "duration", _node->animation.duration );
        }

        if( _node->hasComponentSurface && ImGui::CollapsingHeader( "Component Surface:", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            uiReadOnlyString( "name", _node->componentSurface.Name );
            uiReadOnlyString( "type", _node->componentSurface.Type );

            if( _node->componentSurface.Compile == true )
            {
                uiReadOnlyVec2f( "max size", _node->componentSurface.MaxSize );
                uiReadOnlyVec2f( "size", _node->componentSurface.Size );
                uiReadOnlyVec2f( "offset", _node->componentSurface.Offset );
            }

            if( _node->componentSurface.hasAnimation && ImGui::CollapsingHeader( "Surface Animation:", ImGuiTreeNodeFlags_DefaultOpen ) )
            {
                uiEditorBool( "loop", _node->componentSurface.animation.loop );
                uiReadOnlyBool( "play", _node->componentSurface.animation.play );
                uiReadOnlyBool( "pause", _node->componentSurface.animation.pause );
                uiEditorVec1f( "time", _node->componentSurface.animation.time );
                uiReadOnlyVec1f( "duration", _node->componentSurface.animation.duration );
            }

            if( _node->componentSurface.isTypeSurfaceImage && ImGui::CollapsingHeader( "Surface Image:", ImGuiTreeNodeFlags_DefaultOpen ) )
            {
                uiReadOnlyString( "resource name", _node->componentSurface.surfaceImage.ResourceName );
                uiReadOnlyString( "resource type", _node->componentSurface.surfaceImage.ResourceType );
                uiReadOnlyUV4( "UV image", _node->componentSurface.surfaceImage.UVImage );

                if( _node->componentSurface.surfaceImage.isContent && ImGui::CollapsingHeader( "Surface Image Content:", ImGuiTreeNodeFlags_DefaultOpen ) )
                {
                    uiReadOnlyString( "file group", _node->componentSurface.surfaceImage.content.FileGroup );
                    uiReadOnlyString( "file path", _node->componentSurface.surfaceImage.content.FilePath );
                    uiReadOnlyString( "codec", _node->componentSurface.surfaceImage.content.CodecType );
                    uiReadOnlyString( "converter", _node->componentSurface.surfaceImage.content.ConverterType );
                }
            }

            if( _node->componentSurface.hasAtlas && ImGui::CollapsingHeader( "Atlas:", ImGuiTreeNodeFlags_DefaultOpen ) )
            {
                uiReadOnlyString( "resource name", _node->componentSurface.atlas.ResourceName );
                uiReadOnlyString( "resource type", _node->componentSurface.atlas.ResourceType );                

                if( _node->componentSurface.atlas.isContent && ImGui::CollapsingHeader( "Atlas Content:", ImGuiTreeNodeFlags_DefaultOpen ) )
                {
                    uiReadOnlyString( "file group", _node->componentSurface.atlas.content.FileGroup );
                    uiReadOnlyString( "file path", _node->componentSurface.atlas.content.FilePath );
                    uiReadOnlyString( "codec", _node->componentSurface.atlas.content.CodecType );
                    uiReadOnlyString( "converter", _node->componentSurface.atlas.content.ConverterType );
                }
            }
        }

        if( _node->isTypeTextField && ImGui::CollapsingHeader( "TextField:", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            uiEditorVec1f( "Max Length", _node->textField.MaxLength );
            uiEditorBool( "Wrap", _node->textField.Wrap );
            uiEditorBool( "AutoScale", _node->textField.AutoScale );
            uiEditorVec2f( "AnchorPercent", _node->textField.AnchorPercent );
            uiEditorBool( "AnchorVerticalAlign", _node->textField.AnchorVerticalAlign );
            uiEditorBool( "AnchorHorizontalAlign", _node->textField.AnchorHorizontalAlign );
            uiEditorString( "TextId", _node->textField.TextId );
            uiReadOnlyString( "AliasTextId", _node->textField.TextAliasId );
            uiEditorString( "AliasEnvironment", _node->textField.TextAliasEnvironment );

            uiReadOnlyBool( "HasText", _node->textField.HasText );

            if( _node->textField.HasText == true )
            {
                uiReadOnlyString( "Format", _node->textField.Format );
                uiReadOnlyString( "Text", _node->textField.Text );
            }

            uiEditorString( "FontName", _node->textField.FontName );
            uiReadOnlyString( "TotalFontName", _node->textField.TotalFontName );

            uiEditorBool( "##HasFontColor", _node->textField.HasFontColor ); ImGui::SameLine(); uiEditorColor( "FontColor", _node->textField.FontColor, _node->textField.HasFontColor );
            uiReadOnlyColor( "TotalFontColor", _node->textField.TotalFontColor );

            uiEditorBool( "##HasLineOffset", _node->textField.HasLineOffset ); ImGui::SameLine(); uiEditorVec1f( "LineOffset", _node->textField.LineOffset, _node->textField.HasLineOffset );
            uiReadOnlyVec1f( "TotalLineOffset", _node->textField.TotalLineOffset );

            uiEditorBool( "##CharOffset", _node->textField.HasCharOffset ); ImGui::SameLine(); uiEditorVec1f( "CharOffset", _node->textField.CharOffset, _node->textField.HasCharOffset );
            uiReadOnlyVec1f( "TotalCharOffset", _node->textField.TotalCharOffset );

            uiEditorBool( "##HasCharScale", _node->textField.HasCharScale ); ImGui::SameLine(); uiEditorVec1f( "CharScale", _node->textField.CharScale, _node->textField.HasCharScale );
            uiReadOnlyVec1f( "TotalCharScale", _node->textField.TotalCharScale );

            uiEditorListBox( "HorizontAlign", _node->textField.HorizontAlign, {"Left", "Center", "Right", "None"}, 4 );
            uiReadOnlyListBox( "TotalHorizontAlign", _node->textField.TotalHorizontAlign, {"Left", "Center", "Right", "None"}, 4 );
            uiEditorListBox( "VerticalAlign", _node->textField.VerticalAlign, {"Bottom", "Center", "Top", "None"}, 4 );
            uiReadOnlyListBox( "TotalVerticalAlign", _node->textField.TotalVerticalAlign, {"Bottom", "Center", "Top", "None"}, 4 );
            uiEditorVec1U( "MaxCharCount", _node->textField.MaxCharCount );
            uiEditorBool( "Pixelsnap", _node->textField.Pixelsnap );
        }

        if( _node->isTypeMovie2 && ImGui::CollapsingHeader( "Movie2:", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            uiReadOnlyString( "Composition Name", _node->movie2.CompositionName );
            uiEditorString( "AliasEnvironment", _node->movie2.TextAliasEnvironment );
        }

        if( _node->isTypeSpine && ImGui::CollapsingHeader( "Spine:", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            uiReadOnlyString( "Resource Name", _node->spine.ResourceName );
            uiReadOnlyString( "Resource Type", _node->spine.ResourceType );

            if( _node->spine.isContent && ImGui::CollapsingHeader( "Spine Resource Content:", ImGuiTreeNodeFlags_DefaultOpen ) )
            {
                uiReadOnlyString( "file group", _node->spine.content.FileGroup );
                uiReadOnlyString( "file path", _node->spine.content.FilePath );
                uiReadOnlyString( "codec", _node->spine.content.CodecType );
                uiReadOnlyString( "converter", _node->spine.content.ConverterType );
            }
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
    void NodeDebuggerApp::OnSelectNode( DebuggerNode * _node, DebuggerNode ** _selectedNode )
    {
        if( *_selectedNode != _node )
        {
            *_selectedNode = _node;

            if( *_selectedNode != nullptr )
            {
                Vector<uint32_t> path = this->CollectNodePath( *_selectedNode );

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

                m_invalidateSelectedTab = true;
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
            zed_net_socket_close( &socket );
            m_connectionStatus = ConnectionStatus::ConnectionFailed;
            return;
        }

        m_socket = socket;
        m_connectionStatus = ConnectionStatus::Connected;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DisconnectFromServer()
    {
        zed_net_socket_close( &m_socket );
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
            PacketHeader * hdr = reinterpret_cast<PacketHeader *>(m_receivedData.data());

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

            this->CompressPacket( packet, hdr );

            Detail::InsertPacketHeader( packet.payload, hdr );

            m_outgoingPackets.emplace_back( packet );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::SendChangedTab( const String & _tab )
    {
        pugi::xml_document doc;

        pugi::xml_node packetNode = doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "Tab" );

        pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

        payloadNode.append_attribute( "value" ).set_value( _tab.c_str() );

        this->SendXML( doc );
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

        if( _node.hasComponentSurface )
        {
            pugi::xml_node xmlSurface = xmlNode.append_child( "Component:Surface" );

            _node.componentSurface.serialize( xmlSurface );

            if( _node.componentSurface.hasAnimation )
            {
                pugi::xml_node xmlAnimation = xmlSurface.append_child( "Animation" );

                _node.componentSurface.animation.serialize( xmlAnimation );
            }

            if( _node.componentSurface.isTypeSurfaceImage )
            {
                pugi::xml_node xmlSurfaceImage = xmlSurface.append_child( "Type:SurfaceImage" );

                _node.componentSurface.surfaceImage.serialize( xmlSurfaceImage );

                if( _node.componentSurface.surfaceImage.isContent )
                {
                    pugi::xml_node xmlContent = xmlSurfaceImage.append_child( "Content" );

                    _node.componentSurface.surfaceImage.content.serialize( xmlContent );
                }
            }
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
    void NodeDebuggerApp::SendNodeSelection( const String & _path )
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
