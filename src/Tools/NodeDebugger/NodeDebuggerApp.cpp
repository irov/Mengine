#include "NodeDebuggerApp.h"

#include "imgui_impl_glfw_gl3_glad.h"

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Config/Stringstream.h"

#include <chrono>
#include <sstream>
#include <iterator>
#include <algorithm>

#define ZED_NET_STATIC
#define ZED_NET_IMPLEMENTATION
#include "zed_net.h"

#define MutexLocker std::lock_guard<std::mutex>

#include "IconsAtlas.h"

#include "ImGui_Ext.h"

namespace Mengine
{
    static DebuggerNode::Icon NodeTypeToIcon( const String & _type )
    {
        if( _type == "Movie2" )
        {
            return DebuggerNode::Icon::Icon_Movie2;
        }
        else if( _type == "Scene" )
        {
            return DebuggerNode::Icon::Icon_Scene;
        }
        else if( _type == "Layer2D" )
        {
            return DebuggerNode::Icon::Icon_Layer2D;
        }
        else if( _type == "Entity" )
        {
            return DebuggerNode::Icon::Icon_Entity;
        }
        else
        {
            return DebuggerNode::Icon::Icon_Unknown;
        }
    }


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
        , mIconsAtlas( 0 )
        , mServerAddress()
        , mServerPort( 18790 )
        , mServerAddressCopy()
        , mServerPortCopy( 0 )
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
        glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );

        mWindow = glfwCreateWindow( mWidth, mHeight, "Node Debugger", nullptr, nullptr );

        if( mWindow == nullptr )
        {
            glfwTerminate();

            return false;
        }

        glfwSetWindowUserPointer( mWindow, this );

        glfwMakeContextCurrent( mWindow );
        gladLoadGLLoader( reinterpret_cast<GLADloadproc>(glfwGetProcAddress) );
        glfwSwapInterval( 1 ); // enable v-sync

        LoadIconsAtlas();

        glViewport( 0, 0, mWidth, mHeight );

        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        glfwSetScrollCallback( mWindow, ImGui_ImplGlfw_ScrollCallback );
        glfwSetCharCallback( mWindow, ImGui_ImplGlfw_CharCallback );
        glfwSetKeyCallback( mWindow, ImGui_ImplGlfw_KeyCallback );
        glfwSetMouseButtonCallback( mWindow, ImGui_ImplGlfw_MouseButtonCallback );
        glfwSetWindowSizeCallback(mWindow, []( GLFWwindow * _wnd, int _width, int _height ) {
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
        ImGui_ImplGlfwGL3_Init( mWindow, false );
        ImGui::StyleColorsClassic();
        //ImGuiExt::SetBrightStyle();

        mShutdown = false;
        mNetworkThread = std::thread( &NodeDebuggerApp::NetworkLoop, this );

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

        if( mScene )
        {
            DestroyNode( mScene );
            mScene = nullptr;
        }
    }

    void NodeDebuggerApp::Resize( const int _width, const int _height )
    {
        if( mWidth != _width || mHeight != _height )
        {
            mWidth = _width;
            mHeight = _height;
            glViewport( 0, 0, mWidth, mHeight );
        }
    }

    void NodeDebuggerApp::Update()
    {
        MutexLocker lock( mDataMutex );

        if( mConnectionStatus == ConnectionStatus::Disconnected )
        {
            if( mScene )
            {
                DestroyNode( mScene );
                mScene = nullptr;
            }

            mIncomingPackets.resize( 0 );
            mOutgoingPackets.resize( 0 );
            mReceivedData.resize( 0 );
        }
        else if( mConnectionStatus == ConnectionStatus::Connected )
        {
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

            if( !mSelectedNodePath.empty() )
            {
                SendNodeSelection( mSelectedNodePath );

                mSelectedNodePath.clear();
            }
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
        String typeStr = typeAttrib.value();

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
                DestroyNode( mScene );
            }

            mScene = new DebuggerNode();
            mScene->parent = nullptr;

            DeserializeNode( xmlNode, mScene );

            mSelectedNode = nullptr;
        }
    }

    void NodeDebuggerApp::DeserializeNode( const pugi::xml_node& _xmlNode, DebuggerNode* _node )
    {
        _node->uid = _xmlNode.attribute( "uid" ).as_uint();
        _node->name = _xmlNode.attribute( "name" ).value();
        _node->type = _xmlNode.attribute( "type" ).value();

        deserializeNodeProp<bool>( "enable", _xmlNode, [_node]( bool _value )
        {
            _node->enable = _value;
        } );

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

        _node->icon = NodeTypeToIcon( _node->type );

        _node->dirty = false;
    }

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

    String NodeDebuggerApp::PathToString( const Vector<uint32_t> & _path )
    {
        Stringstream stream;
        std::copy( _path.begin(), _path.end(), std::ostream_iterator<uint32_t>( stream, "/" ) );

        return stream.str();
    }

    void NodeDebuggerApp::DestroyNode( DebuggerNode * _node )
    {
        if( mSelectedNode == _node )
        {
            mSelectedNode = nullptr;
        }

        for( auto n : _node->children )
        {
            DestroyNode( n );
        }

        _node->children.resize( 0 );
        delete _node;
    }

    void NodeDebuggerApp::LoadIconsAtlas()
    {
        int width, height, bpp;
        stbi_uc* data = stbi_load_from_memory( reinterpret_cast<const stbi_uc *>( IconsAtlas ), IconsAtlas_size, &width, &height, &bpp, STBI_rgb_alpha );
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

            mIconsAtlas = static_cast<uintptr_t>( texture );
        }
    }

    void NodeDebuggerApp::DoUI()
    {
        const ImGuiWindowFlags kPanelFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

        const float leftPanelWidth = 400.0f;

        ImGui::SetNextWindowPos( ImVec2( 0, 0 ), ImGuiCond_FirstUseEver );
        ImGui::SetNextWindowSize( ImVec2( static_cast<float>(mWidth), static_cast<float>(mHeight) ), ImGuiCond_Always );

        if( ImGui::Begin( "Node Debugger", nullptr, kPanelFlags ) )
        {
            ImGui::Columns( 2, nullptr, true );
            ImGui::SetColumnWidth( 0, leftPanelWidth );

            if( ImGui::CollapsingHeader( "Server:", ImGuiTreeNodeFlags_DefaultOpen ) )
            {
                char serverAddress[256] = {0};

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
                    int port = static_cast<int>(mServerPort);
                    if( ImGui::InputInt( "##Port", &port, 0, 0, ImGuiInputTextFlags_CharsDecimal ) )
                    {
                        mServerPort = static_cast<uint16_t>(port & 0xFFFF);
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
                    if( ImGui::BeginChild( "SceneTree", ImVec2( 0, 0 ), false, ImGuiWindowFlags_HorizontalScrollbar ) )
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

    String NodeDebuggerApp::DoIPInput( const String & _title, const String & _inIP )
    {
        int octets[4] = {127, 0, 0, 1};

        if( !_inIP.empty() )
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

        Stringstream ss;
        ss << octets[0] << '.' << octets[1] << '.' << octets[2] << '.' << octets[3];

        return ss.str();
    }

    void NodeDebuggerApp::DoNodeElement( DebuggerNode * _node )
    {
        const ImGuiTreeNodeFlags seletedFlag = (mSelectedNode == _node) ? ImGuiTreeNodeFlags_Selected : 0;
        const ImGuiTreeNodeFlags flagsNormal = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | seletedFlag;
        const ImGuiTreeNodeFlags flagsNoChildren = ImGuiTreeNodeFlags_Leaf | seletedFlag;

        std::string treeNodeId = std::to_string( _node->uid );
        std::string fullLabel = std::string( _node->name.c_str() ) + "##" + treeNodeId;

        auto result = ImGuiExt::TreeNodeWithIcon
        (
            reinterpret_cast<ImTextureID>( mIconsAtlas ),
            static_cast<size_t>( _node->icon ),
            fullLabel.c_str(),
            _node->children.empty() ? flagsNoChildren : flagsNormal
        );

        if( result.second )
        {
            OnSelectNode( _node );
        }

        if( result.first )
        {
            for( DebuggerNode* child : _node->children )
            {
                DoNodeElement( child );
            }

            ImGui::TreePop();
        }
    }

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
            bool input = ImGui::DragFloat( _caption, &testValue );

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

            if( input && _prop != testValue )
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
            uiEditorBool( "Hide", _node->render.hide );
            ImGui::Spacing();

            uiEditorColor( "Color", _node->render.color );
        }

        if( _node->hasAnimation && ImGui::CollapsingHeader( "Animation:", ImGuiTreeNodeFlags_DefaultOpen ) )
        {
            uiEditorBool( "Loop", _node->animation.loop );
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

    void NodeDebuggerApp::OnSelectNode( DebuggerNode * _node )
    {
        if( mSelectedNode != _node )
        {
            mSelectedNode = _node;

            if( mSelectedNode != nullptr )
            {
                mSelectedNodePath = PathToString( CollectNodePath( mSelectedNode ) );
            }
            else
            {
                mSelectedNodePath = "-";
            }
        }
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
            zed_net_socket_close( &socket);
            mConnectionStatus = ConnectionStatus::ConnectionFailed;
            return;
        }

        mSocket = socket;
        mConnectionStatus = ConnectionStatus::Connected;
    }

    void NodeDebuggerApp::DisconnectFromServer()
    {
        zed_net_socket_close( &mSocket);
        mConnectionStatus = ConnectionStatus::Disconnected;
    }

    void NodeDebuggerApp::SendNetworkData()
    {
        mDataMutex.lock();
        {
            if( !mOutgoingPackets.empty() )
            {
                for( const NodeDebuggerPacket & p : mOutgoingPackets )
                {
                    zed_net_tcp_socket_send( &mSocket, p.payload.data(), static_cast<int>(p.payload.size()) );
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
        bool shouldDisconnect = false;
        do
        {
            bytesReceived = zed_net_tcp_socket_receive( &mSocket, buffer, BUFFER_SIZE );
            if( bytesReceived )
            {
                mReceivedData.insert( mReceivedData.end(), &buffer[0], &buffer[bytesReceived] );
            }
            else
            {
                shouldDisconnect = true;
            }
        } while( !shouldDisconnect && bytesReceived == BUFFER_SIZE );

        if( shouldDisconnect )
        {
            DisconnectFromServer();
            return;
        }

        // check if we have read something
        if( mReceivedData.size() > sizeof( PacketHeader ) )
        {
            MutexLocker lock( mDataMutex );

            // check if we have enough data to form a packet
            PacketHeader* hdr = reinterpret_cast<PacketHeader *>(mReceivedData.data());

            // received garbage - nothing fancy, just disconnect
            if( hdr->magic != PACKET_MAGIC )
            {
                DisconnectFromServer();
                return;
            }

            while( hdr != nullptr && hdr->payloadSize <= (mReceivedData.size() - sizeof( PacketHeader )) )
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

                    hdr = reinterpret_cast<PacketHeader *>(mReceivedData.data());
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

    void NodeDebuggerApp::SendXML( const pugi::xml_document & _doc )
    {
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
            hdr.payloadSize = static_cast<uint32_t>(packet.payload.size());
            InsertPacketHeader( packet.payload, hdr );

            mOutgoingPackets.emplace_back( packet );
        }
    }

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

        serializeNodeProp( _node.enable, "enable", xmlNode );
        
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

        SendXML( doc );
    }

    void NodeDebuggerApp::SendNodeSelection(const String & _path)
    {
        pugi::xml_document doc;

        pugi::xml_node packetNode = doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "Selection" );

        pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

        pugi::xml_node xmlNode = payloadNode.append_child( "Path" );

        xmlNode.append_attribute( "value" ).set_value( _path.c_str() );

        SendXML( doc );
    }
}
