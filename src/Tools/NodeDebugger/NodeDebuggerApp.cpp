#include "NodeDebuggerApp.h"

#include <cstdarg>

#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Kernel/RenderContext.h"
#include "Kernel/Stringstream.h"
#include "Kernel/Assertion.h"
#include "Kernel/UniqueHelper.h"

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
        FD_ZERO( &socketsSet );
        FD_SET( _socket->handle, &socketsSet );

        timeval tv;
        tv.tv_sec = static_cast<long>(_timeoutMs / 1000);
        tv.tv_usec = static_cast<decltype(tv.tv_usec)>((_timeoutMs % 1000) * 1000);

#if defined(_WIN32)
        const int descriptorCount = 0;
#else
        const int descriptorCount = _socket->handle + 1;
#endif

        const int result = ::select( descriptorCount, &socketsSet, nullptr, nullptr, (_timeoutMs == 0) ? nullptr : &tv );

        return (1 == result);
    }
    //////////////////////////////////////////////////////////////////////////
    NodeDebuggerApp::NodeDebuggerApp()
        : m_mosaicContext( nullptr )
        , m_mosaicPointerDown( 0 )
        , m_mosaicPointerPressed( 0 )
        , m_mosaicPointerReleased( 0 )
        , m_mosaicPointerClicks( 0 )
        , m_window( nullptr )
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
        , m_mosaicNodePathApplied( true )
        , m_selectedTabIndex( 0 )
        , m_sceneUpdateFreq( 0 )
        , m_sceneUpdateTimer( 0.0 )
        , m_updateSceneOnChange( false )
        , m_pauseRequested( false )
        , m_muteRequested( false )
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
        gladLoadGL( reinterpret_cast<GLADloadfunc>(&glfwGetProcAddress) );
        glfwSwapInterval( 1 ); // enable v-sync

        glViewport( 0, 0, m_width, m_height );

        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        glfwSetMouseButtonCallback( m_window, []( GLFWwindow * _wnd, int _button, int _action, int _mods )
        {
            NodeDebuggerApp * _this = reinterpret_cast<NodeDebuggerApp *>(glfwGetWindowUserPointer( _wnd ));

            if( _this != nullptr )
            {
                _this->PushMosaicMouseButton( _button, _action, _mods );
            }
        } );

        glfwSetScrollCallback( m_window, []( GLFWwindow * _wnd, double _offsetX, double _offsetY )
        {
            NodeDebuggerApp * _this = reinterpret_cast<NodeDebuggerApp *>(glfwGetWindowUserPointer( _wnd ));

            if( _this != nullptr )
            {
                _this->PushMosaicScroll( _offsetX, _offsetY );
            }
        } );

        glfwSetKeyCallback( m_window, []( GLFWwindow * _wnd, int _key, int _scancode, int _action, int _mods )
        {
            NodeDebuggerApp * _this = reinterpret_cast<NodeDebuggerApp *>(glfwGetWindowUserPointer( _wnd ));

            if( _this != nullptr )
            {
                _this->PushMosaicKey( _key, _action, _mods );
            }
        } );

        glfwSetCharCallback( m_window, []( GLFWwindow * _wnd, unsigned int _codepoint )
        {
            NodeDebuggerApp * _this = reinterpret_cast<NodeDebuggerApp *>(glfwGetWindowUserPointer( _wnd ));

            if( _this != nullptr )
            {
                _this->PushMosaicChar( _codepoint );
            }
        } );

        glfwSetWindowSizeCallback( m_window, []( GLFWwindow * _wnd, int _width, int _height )
        {
            NodeDebuggerApp * _this = reinterpret_cast<NodeDebuggerApp *>(glfwGetWindowUserPointer( _wnd ));
            if( _this != nullptr )
            {
                _this->Resize( _width, _height );
            }
        } );

        if( this->InitializeMosaic() == false )
        {
            return false;
        }

        this->LoadIconsAtlas();

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
            "resolutions",
            "Resolutions",
            true,
            [this]()
        {
            this->DoUIResolutionsTab();
        }} );
        m_tabs.push_back( {
            "sounds",
            "Sounds",
            true,
            [this]()
        {
            this->DoUISoundsTab();
        }} );
        m_tabs.push_back( {
            "settings",
            "Settings",
            true,
            [this]()
        {
            this->DoUISettingsTab();
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
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static Mosaic::Key makeKey_( const Char * _value )
        {
            Mosaic::StringView value( _value );

            Mosaic::Key key( value );

            return key;
        }
        //////////////////////////////////////////////////////////////////////////
        static Mosaic::Key makeKey_( const String & _value )
        {
            Mosaic::StringView value( _value.c_str(), _value.size() );

            Mosaic::Key key( value );

            return key;
        }
        //////////////////////////////////////////////////////////////////////////
        static Mosaic::KeyCode getMosaicKeyCode_( int _key )
        {
            if( _key >= GLFW_KEY_A && _key <= GLFW_KEY_Z )
            {
                return (Mosaic::KeyCode)((int)Mosaic::KeyCode::A + (_key - GLFW_KEY_A));
            }

            if( _key >= GLFW_KEY_0 && _key <= GLFW_KEY_9 )
            {
                return (Mosaic::KeyCode)((int)Mosaic::KeyCode::D0 + (_key - GLFW_KEY_0));
            }

            if( _key >= GLFW_KEY_F1 && _key <= GLFW_KEY_F12 )
            {
                return (Mosaic::KeyCode)((int)Mosaic::KeyCode::F1 + (_key - GLFW_KEY_F1));
            }

            switch( _key )
            {
            case GLFW_KEY_TAB: return Mosaic::KeyCode::Tab;
            case GLFW_KEY_ENTER: return Mosaic::KeyCode::Enter;
            case GLFW_KEY_KP_ENTER: return Mosaic::KeyCode::Enter;
            case GLFW_KEY_ESCAPE: return Mosaic::KeyCode::Escape;
            case GLFW_KEY_SPACE: return Mosaic::KeyCode::Space;
            case GLFW_KEY_BACKSPACE: return Mosaic::KeyCode::Backspace;
            case GLFW_KEY_DELETE: return Mosaic::KeyCode::Delete;
            case GLFW_KEY_LEFT: return Mosaic::KeyCode::Left;
            case GLFW_KEY_RIGHT: return Mosaic::KeyCode::Right;
            case GLFW_KEY_UP: return Mosaic::KeyCode::Up;
            case GLFW_KEY_DOWN: return Mosaic::KeyCode::Down;
            case GLFW_KEY_HOME: return Mosaic::KeyCode::Home;
            case GLFW_KEY_END: return Mosaic::KeyCode::End;
            case GLFW_KEY_PAGE_UP: return Mosaic::KeyCode::PageUp;
            case GLFW_KEY_PAGE_DOWN: return Mosaic::KeyCode::PageDown;
            default: break;
            }

            return Mosaic::KeyCode::Unknown;
        }
        //////////////////////////////////////////////////////////////////////////
        static Mosaic::Modifiers getMosaicModifiers_( int _mods )
        {
            Mosaic::Modifiers modifiers;
            modifiers.shift = (_mods & GLFW_MOD_SHIFT) != 0;
            modifiers.control = (_mods & GLFW_MOD_CONTROL) != 0;
            modifiers.alt = (_mods & GLFW_MOD_ALT) != 0;
            modifiers.super = (_mods & GLFW_MOD_SUPER) != 0;

#if defined(__APPLE__)
            modifiers.primary = modifiers.super;
#else
            modifiers.primary = modifiers.control;
#endif

            return modifiers;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::PushMosaicMouseButton( int _button, int _action, int _mods )
    {
        unsigned bit = 0;

        switch( _button )
        {
        case GLFW_MOUSE_BUTTON_LEFT: bit = 1U << (unsigned)Mosaic::PointerButton::Primary; break;
        case GLFW_MOUSE_BUTTON_RIGHT: bit = 1U << (unsigned)Mosaic::PointerButton::Secondary; break;
        case GLFW_MOUSE_BUTTON_MIDDLE: bit = 1U << (unsigned)Mosaic::PointerButton::Middle; break;
        default: return;
        }

        if( _action == GLFW_PRESS )
        {
            m_mosaicPointerDown = (uint8_t)(m_mosaicPointerDown | bit);
            m_mosaicPointerPressed = (uint8_t)(m_mosaicPointerPressed | bit);
            m_mosaicPointerClicks = 1;
        }
        else if( _action == GLFW_RELEASE )
        {
            m_mosaicPointerDown = (uint8_t)(m_mosaicPointerDown & ~bit);
            m_mosaicPointerReleased = (uint8_t)(m_mosaicPointerReleased | bit);
        }

        m_mosaicInput.modifiers = Detail::getMosaicModifiers_( _mods );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::PushMosaicKey( int _key, int _action, int _mods )
    {
        Mosaic::KeyCode code = Detail::getMosaicKeyCode_( _key );

        if( code == Mosaic::KeyCode::Unknown )
        {
            return;
        }

        Mosaic::KeyEvent event;
        event.key = code;
        event.pressed = _action == GLFW_PRESS || _action == GLFW_REPEAT;
        event.released = _action == GLFW_RELEASE;
        event.repeat = _action == GLFW_REPEAT;
        event.modifiers = Detail::getMosaicModifiers_( _mods );

        m_mosaicInput.keyboard.push_back( event );
        m_mosaicInput.modifiers = event.modifiers;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::PushMosaicChar( unsigned int _codepoint )
    {
        if( _codepoint < 0x20 || _codepoint == 0x7f )
        {
            return;
        }

        Mosaic::String text;

        if( _codepoint < 0x80 )
        {
            text += (char)_codepoint;
        }
        else if( _codepoint < 0x800 )
        {
            text += (char)(0xc0 | (_codepoint >> 6));
            text += (char)(0x80 | (_codepoint & 0x3f));
        }
        else if( _codepoint < 0x10000 )
        {
            text += (char)(0xe0 | (_codepoint >> 12));
            text += (char)(0x80 | ((_codepoint >> 6) & 0x3f));
            text += (char)(0x80 | (_codepoint & 0x3f));
        }
        else
        {
            text += (char)(0xf0 | (_codepoint >> 18));
            text += (char)(0x80 | ((_codepoint >> 12) & 0x3f));
            text += (char)(0x80 | ((_codepoint >> 6) & 0x3f));
            text += (char)(0x80 | (_codepoint & 0x3f));
        }

        m_mosaicInput.text.push_back( text );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::PushMosaicScroll( double _offsetX, double _offsetY )
    {
        m_mosaicInput.wheel.x += (float)_offsetX * 24.f;
        m_mosaicInput.wheel.y += (float)_offsetY * 24.f;
    }
    //////////////////////////////////////////////////////////////////////////
    bool NodeDebuggerApp::InitializeMosaic()
    {
        Mosaic::setDefaultAllocator( &m_mosaicAllocator );

        if( m_mosaicRenderer.initialize() == false )
        {
            return false;
        }

        if( m_mosaicFont.initialize( &m_mosaicRenderer ) == false )
        {
            return false;
        }

        if( m_mosaicPlatform.initialize( m_window ) == false )
        {
            return false;
        }

        Mosaic::ContextOptions options;
        options.allocator = &m_mosaicAllocator;
        options.platform = &m_mosaicPlatform;
        options.fontProvider = &m_mosaicFont;

        m_mosaicContext = Mosaic::newContext( options );

        if( m_mosaicContext == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::FinalizeMosaic()
    {
        if( m_mosaicContext != nullptr )
        {
            Mosaic::deleteContext( m_mosaicContext );
            m_mosaicContext = nullptr;
        }

        m_mosaicPlatform.finalize();
        m_mosaicFont.finalize();
        m_mosaicRenderer.finalize();

        Mosaic::setDefaultAllocator( nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::UpdateMosaicInput( double _dt )
    {
        double cursorX = 0.0;
        double cursorY = 0.0;
        glfwGetCursorPos( m_window, &cursorX, &cursorY );

        m_mosaicInput.pointers.resize( 1 );

        Mosaic::PointerState & pointer = m_mosaicInput.pointers.front();

        Mosaic::Vec2 position = {(float)cursorX, (float)cursorY};

        pointer.id = 1;
        pointer.type = Mosaic::PointerType::Mouse;
        pointer.delta = {position.x - pointer.position.x, position.y - pointer.position.y};
        pointer.position = position;

        pointer.down = (uint8_t)(m_mosaicPointerDown | m_mosaicPointerPressed);
        pointer.pressed = m_mosaicPointerPressed;
        pointer.released = m_mosaicPointerReleased;

        if( pointer.pressed != 0 )
        {
            pointer.pressPositions[0] = position;
            pointer.pressPositionValid = 1;
            pointer.clickCounts[0] = m_mosaicPointerClicks;
            pointer.clickCount = m_mosaicPointerClicks;
        }
        else
        {
            pointer.clickCounts[0] = 0;
            pointer.clickCount = 0;
        }

        m_mosaicInput.windowFocused = glfwGetWindowAttrib( m_window, GLFW_FOCUSED ) == GLFW_TRUE;
        m_mosaicInput.deltaTime = (float)_dt;
        m_mosaicInput.timestamp = glfwGetTime();
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::TextLine( const Mosaic::Color & _color, const Char * _format, ... )
    {
        Char buffer[1024] = {'\0'};

        va_list args;
        va_start( args, _format );
        vsnprintf( buffer, sizeof( buffer ) - 1, _format, args );
        va_end( args );

        m_mosaicReadout.emplace_back( buffer );

        const Mosaic::String & line = m_mosaicReadout.back();

        size_t lineIndex = m_mosaicReadout.size();

        Mosaic::Key key( lineIndex );

        Mosaic::Scope lineScope = Mosaic::scope( m_mosaicContext, key );

        Mosaic::Theme theme = Mosaic::getTheme( m_mosaicContext );
        theme.colors.text = _color;

        Mosaic::Scope colorScope = Mosaic::styleScope( m_mosaicContext, theme );

        Mosaic::text( m_mosaicContext, Mosaic::StringView( line.c_str(), line.size() ) );
    }
    //////////////////////////////////////////////////////////////////////////
    Mosaic::LayoutOptions NodeDebuggerApp::FillLayout() const
    {
        Mosaic::LayoutOptions layout;
        layout.width = Mosaic::SizeRule::Fill;
        layout.height = Mosaic::SizeRule::Fill;

        return layout;
    }
    //////////////////////////////////////////////////////////////////////////
    Mosaic::Scope NodeDebuggerApp::PropertyRow( const Char * _label )
    {
        Mosaic::LayoutOptions rowLayout;
        rowLayout.width = Mosaic::SizeRule::Fill;

        Mosaic::Key key = Detail::makeKey_( _label );

        Mosaic::Scope row = Mosaic::row( m_mosaicContext, key, rowLayout );

        Mosaic::TextOptions labelOptions;
        labelOptions.layout.width = Mosaic::Dimension::fixed( Mosaic::getTheme( m_mosaicContext ).metrics.propertyLabelWidth );

        Mosaic::text( m_mosaicContext, _label, labelOptions );

        return row;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DoUI()
    {
        m_mosaicReadout.clear();

        Mosaic::WindowOptions windowOptions;
        windowOptions.titleBar = false;
        windowOptions.movable = false;
        windowOptions.resizable = false;

        Mosaic::setNextWindowPosition( m_mosaicContext, {0.f, 0.f}, Mosaic::Condition::Always );
        Mosaic::setNextWindowSize( m_mosaicContext, {(float)m_width, (float)m_height}, Mosaic::Condition::Always );

        Mosaic::WindowScope window = Mosaic::window( m_mosaicContext, "Node Debugger", windowOptions );

        if( window.visible() == false )
        {
            return;
        }

        m_tabTitles.clear();

        for( const TabDescriptor & tab : m_tabs )
        {
            m_tabTitles.emplace_back( tab.title.c_str(), tab.title.size() );
        }

        const Mosaic::StringView * titleData = m_tabTitles.data();
        size_t titleCount = m_tabTitles.size();

        Mosaic::StringViewSpan titles( titleData, titleCount );

        Mosaic::tabs( m_mosaicContext, "Tabs", &m_selectedTabIndex, titles );

        TabDescriptor & tab = m_tabs[m_selectedTabIndex];

        m_selectedTab = tab.name;

        if( m_cacheSelectedTab != m_selectedTab )
        {
            m_cacheSelectedTab = m_selectedTab;
            m_invalidateSelectedTab = true;
        }

        Mosaic::Key key = Detail::makeKey_( tab.name );

        Mosaic::Scope body = Mosaic::column( m_mosaicContext, key, this->FillLayout() );

        tab.functor();
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DoUIMemoryTab()
    {
        const Mosaic::Color green = {0.f, 1.f, 0.f, 1.f};

        Mosaic::separator( m_mosaicContext );

        this->TextLine( green, "Total: %ug %um %ukb %ub"
            , m_memoryTotal / 1000000000
            , (m_memoryTotal % 1000000000) / 1000000
            , (m_memoryTotal % 1000000) / 1000
            , m_memoryTotal % 1000
        );

        this->TextLine( green, "Texture Available: %ug %um %ukb %ub"
            , m_AvailableTextureMemory / 1000000000
            , (m_AvailableTextureMemory % 1000000000) / 1000000
            , (m_AvailableTextureMemory % 1000000) / 1000
            , m_AvailableTextureMemory % 1000
        );

        this->TextLine( green, "Texture Use: %ug %um %ukb %ub"
            , m_TextureMemoryUse / 1000000000
            , (m_TextureMemoryUse % 1000000000) / 1000000
            , (m_TextureMemoryUse % 1000000) / 1000
            , m_TextureMemoryUse % 1000
        );

        this->TextLine( green, "Texture Count: %u", m_TextureCount );
        this->TextLine( green, "Sound Sources Count: %u", m_SoundSourcesCount );
        this->TextLine( green, "Sound Buffers Count: %u", m_SoundBuffersCount );

        Mosaic::separator( m_mosaicContext );

        struct MemoryDesc
        {
            String name;
            uint32_t size;
        };

        typedef Vector<MemoryDesc> VectorMemoryDesc;
        VectorMemoryDesc vmemory;

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
                this->TextLine( {1.f, 0.25f, 0.5f, 1.f}, "Allocator: %s [total %umb %ukb]"
                    , desc.name.c_str()
                    , desc.size / (1024 * 1024)
                    , desc.size / 1024 % 1024
                );
            }
            else if( desc.size > 1024 )
            {
                this->TextLine( {0.5f, 1.f, 0.25f, 1.f}, "Allocator: %s [total %ukb]"
                    , desc.name.c_str()
                    , desc.size / 1024
                );
            }
            else
            {
                this->TextLine( {0.f, 1.f, 1.f, 1.f}, "Allocator: %s [total %u byte]"
                    , desc.name.c_str()
                    , desc.size
                );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::BulletLine( const Char * _format, ... )
    {
        Char buffer[1024] = {'\0'};

        va_list args;
        va_start( args, _format );
        vsnprintf( buffer, sizeof( buffer ) - 1, _format, args );
        va_end( args );

        m_mosaicReadout.emplace_back( buffer );

        const Mosaic::String & line = m_mosaicReadout.back();

        size_t lineIndex = m_mosaicReadout.size();

        Mosaic::Key key( lineIndex );

        Mosaic::Scope lineScope = Mosaic::scope( m_mosaicContext, key );

        Mosaic::bulletText( m_mosaicContext, Mosaic::StringView( line.c_str(), line.size() ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::ReadOnlyText( uint32_t _index, const String & _message )
    {
        m_mosaicReadout.emplace_back( _message.c_str(), _message.size() );

        Mosaic::String & text = m_mosaicReadout.back();

        Mosaic::TextInputOptions textOptions;
        textOptions.readOnly = true;

        Mosaic::LayoutOptions textLayout;
        textLayout.width = Mosaic::SizeRule::Fill;
        textLayout.height = Mosaic::Dimension::fixed( Mosaic::getTheme( m_mosaicContext ).metrics.lineHeight * 2.5f );

        Mosaic::Scope textScope = Mosaic::scope( m_mosaicContext, Mosaic::Key( _index ) );

        Mosaic::inputMultiline( m_mosaicContext, "message", &text, textOptions, textLayout );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DoUIObjectsLeakTab()
    {
        uint32_t leaksCount = 0;

        for( auto && [type, objects] : m_objectLeaks )
        {
            leaksCount += (uint32_t)objects.size();
        }

        this->TextLine( {0.f, 1.f, 0.f, 1.f}, "Generator: %s [total %u]"
            , m_objectLeakGeneration.c_str()
            , leaksCount
        );

        Mosaic::separator( m_mosaicContext );

        uint32_t index = 0;

        for( auto && [type, objects] : m_objectLeaks )
        {
            Mosaic::Scope typeScope = Mosaic::scope( m_mosaicContext, Mosaic::Key( index ) );

            m_mosaicReadout.emplace_back();
            Mosaic::String & typeLabel = m_mosaicReadout.back();
            typeLabel.assign( type.c_str() );
            typeLabel += " [";
            typeLabel += std::to_string( objects.size() ).c_str();
            typeLabel += ']';

            Mosaic::Theme typeTheme = Mosaic::getTheme( m_mosaicContext );
            typeTheme.colors.text = {0.f, 1.f, 0.f, 1.f};

            bool typeOpened = false;

            {
                Mosaic::Scope typeColor = Mosaic::styleScope( m_mosaicContext, typeTheme );

                Mosaic::TreeScope typeNode = Mosaic::treeNode( m_mosaicContext, Mosaic::Key( index ), typeLabel, true );

                typeOpened = typeNode.expanded();

                if( typeOpened == true )
                {
                    Mosaic::separator( m_mosaicContext );

                    for( const LeakDesc & leak : objects )
                    {
                        Mosaic::Scope leakScope = Mosaic::scope( m_mosaicContext, Mosaic::Key( index ) );

                        this->BulletLine( "file: %s", leak.file.c_str() );
                        this->BulletLine( "line: %s", leak.line.c_str() );
                        this->BulletLine( "function: %s", leak.function.c_str() );

                        this->ReadOnlyText( index, leak.message );

                        Mosaic::Theme parentTheme = Mosaic::getTheme( m_mosaicContext );
                        parentTheme.colors.text = {0.f, 0.f, 1.f, 1.f};

                        Mosaic::Scope parentColor = Mosaic::styleScope( m_mosaicContext, parentTheme );

                        Mosaic::TreeScope parentNode = Mosaic::treeNode( m_mosaicContext, Mosaic::Key( index ), "traceback" );

                        if( parentNode.expanded() == true )
                        {
                            Mosaic::separator( m_mosaicContext );

                            uint32_t parentIndex = 0;

                            for( const LeakDesc & parent : leak.parents )
                            {
                                Mosaic::Scope parentScope = Mosaic::scope( m_mosaicContext, Mosaic::Key( parentIndex ) );

                                this->BulletLine( "file: %s", parent.file.c_str() );
                                this->BulletLine( "line: %s", parent.line.c_str() );
                                this->BulletLine( "function: %s", parent.function.c_str() );

                                this->ReadOnlyText( index, parent.message );

                                Mosaic::separator( m_mosaicContext );

                                ++parentIndex;
                            }
                        }

                        Mosaic::separator( m_mosaicContext );

                        ++index;
                    }
                }
            }

            ++index;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DisabledCheckbox( const Char * _label, bool _value )
    {
        Mosaic::Scope disabled = Mosaic::disabledScope( m_mosaicContext, true );

        bool value = _value;

        Mosaic::checkbox( m_mosaicContext, _label, &value );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DoUISoundsTab()
    {
        for( const SoundDesc & desc : m_sounds )
        {
            Mosaic::Scope soundScope = Mosaic::scope( m_mosaicContext, Mosaic::Key( desc.id ) );

            switch( desc.state )
            {
            case ESS_INIT:
                this->TextLine( {0.5f, 0.5f, 0.5f, 1.f}, "[init]" );
                break;
            case ESS_STOP:
                this->TextLine( {1.f, 0.f, 0.f, 1.f}, "[stop]" );
                break;
            case ESS_PLAY:
                this->TextLine( {0.f, 1.f, 0.f, 1.f}, "[play]" );
                break;
            case ESS_PAUSE:
                this->TextLine( {0.f, 0.5f, 1.f, 1.f}, "[pause]" );
                break;
            case ESS_END:
                this->TextLine( {0.5f, 0.5f, 0.5f, 1.f}, "[end]" );
                break;
            }

            Mosaic::sameLine( m_mosaicContext );
            this->DisabledCheckbox( "streamable", desc.streamable );

            Mosaic::sameLine( m_mosaicContext );
            this->DisabledCheckbox( "loop", desc.loop );

            Mosaic::sameLine( m_mosaicContext );
            this->DisabledCheckbox( "turn", desc.turn );

            Mosaic::sameLine( m_mosaicContext );

            const Mosaic::Color white = {1.f, 1.f, 1.f, 1.f};

            switch( desc.category )
            {
            case ES_SOURCE_CATEGORY_SOUND:
                this->TextLine( white, "[sound]" );
                break;
            case ES_SOURCE_CATEGORY_MUSIC:
                this->TextLine( white, "[music]" );
                break;
            case ES_SOURCE_CATEGORY_VOICE:
                this->TextLine( white, "[voice]" );
                break;
            }

            Mosaic::sameLine( m_mosaicContext );
            this->TextLine( white, "[%u]", desc.id );

            Mosaic::sameLine( m_mosaicContext );
            this->TextLine( white, "time: %f", desc.time_left );

            Mosaic::sameLine( m_mosaicContext );
            this->TextLine( white, "volume: %f", desc.volume );

            Mosaic::sameLine( m_mosaicContext );
            this->TextLine( white, "file: %s", desc.file.c_str() );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DoUINetwork()
    {
        Mosaic::separator( m_mosaicContext );

        Mosaic::Scope network = Mosaic::scrollArea( m_mosaicContext, "Network" );

        for( const NetworkDesk & desk : m_network )
        {
            if( desk.type != "Request" )
            {
                continue;
            }

            Mosaic::Scope deskScope = Mosaic::scope( m_mosaicContext, Mosaic::Key( desk.id ) );

            Mosaic::TreeScope urlNode = Mosaic::treeNode( m_mosaicContext, Mosaic::Key( desk.id ), "Url:" );

            Mosaic::sameLine( m_mosaicContext );

            m_mosaicReadout.emplace_back( desk.url.c_str(), desk.url.size() );

            Mosaic::String & url = m_mosaicReadout.back();

            Mosaic::TextInputOptions urlOptions;
            urlOptions.readOnly = true;

            Mosaic::inputText( m_mosaicContext, "url", &url, urlOptions );

            if( urlNode.expanded() == true )
            {
                Mosaic::separator( m_mosaicContext );

                this->ShowResponseDataForId( desk.id );

                Mosaic::separator( m_mosaicContext );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::ShowResponseJpp( const jpp::object & _object, uint32_t _spaceCounter, uint32_t * const _labelCounter )
    {
        String spaces;
        this->addSpacesWithMultiplier( &spaces, 2, _spaceCounter );

        const Mosaic::Color white = {1.f, 1.f, 1.f, 1.f};
        const Mosaic::Color dim = {1.f, 1.f, 1.f, 0.5f};

        jpp::e_type jppType = _object.type();

        switch( jppType )
        {
        case jpp::e_type::JPP_OBJECT:
            {
                uint32_t objectElementsEnumerator = 0;

                for( auto && [key, value] : _object )
                {
                    Mosaic::Scope elementScope = Mosaic::scope( m_mosaicContext, Mosaic::Key( objectElementsEnumerator ) );

                    jpp::e_type valueType = value.type();

                    if( valueType != jpp::e_type::JPP_OBJECT && valueType != jpp::e_type::JPP_ARRAY )
                    {
                        this->TextLine( white, "%s%s:", spaces.c_str(), key );

                        Mosaic::sameLine( m_mosaicContext );

                        this->ShowResponseJpp( value, _spaceCounter, _labelCounter );
                    }
                    else
                    {
                        m_mosaicReadout.emplace_back();
                        Mosaic::String & nodeLabel = m_mosaicReadout.back();
                        nodeLabel.assign( spaces.c_str(), spaces.size() );
                        nodeLabel.append( key );
                        nodeLabel += ':';

                        Mosaic::TreeScope valueNode = Mosaic::treeNode( m_mosaicContext, Mosaic::Key( objectElementsEnumerator ), nodeLabel );

                        Mosaic::sameLine( m_mosaicContext );

                        Char buffer_value[256 + 1] = {'\0'};

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

                            StdString::strzcat_safe( desc->buffer, _buffer, _size, 256 );

                            return 0;
                        }, &desc );

                        if( desc.end == true )
                        {
                            StdString::strcat_safe( desc.buffer, "...", 256 );
                        }

                        this->TextLine( dim, "%s", buffer_value );

                        if( valueNode.expanded() == true )
                        {
                            ++_spaceCounter;
                            this->ShowResponseJpp( value, _spaceCounter, _labelCounter );
                            --_spaceCounter;
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
                    Mosaic::Scope elementScope = Mosaic::scope( m_mosaicContext, Mosaic::Key( arrayElementsEnumerator ) );

                    jpp::e_type elementType = element.type();

                    if( elementType != jpp::e_type::JPP_OBJECT && elementType != jpp::e_type::JPP_ARRAY )
                    {
                        this->TextLine( dim, "%s %u", spaces.c_str(), arrayElementsEnumerator );

                        Mosaic::sameLine( m_mosaicContext );

                        this->ShowResponseJpp( element, _spaceCounter, _labelCounter );
                    }
                    else
                    {
                        m_mosaicReadout.emplace_back();
                        Mosaic::String & elementLabel = m_mosaicReadout.back();
                        elementLabel.assign( spaces.c_str(), spaces.size() );
                        elementLabel += ' ';
                        elementLabel += std::to_string( arrayElementsEnumerator ).c_str();

                        Mosaic::TreeScope elementNode = Mosaic::treeNode( m_mosaicContext, Mosaic::Key( arrayElementsEnumerator ), elementLabel );

                        if( elementNode.expanded() == true )
                        {
                            ++_spaceCounter;
                            this->ShowResponseJpp( element, _spaceCounter, _labelCounter );
                            --_spaceCounter;
                        }
                    }

                    ++arrayElementsEnumerator;
                }
            }break;
        default:
            {
                String valueStr;
                this->GetValueStringForJppType( _object, jppType, &valueStr, _spaceCounter );

                ++ * _labelCounter;

                m_mosaicReadout.emplace_back( valueStr.c_str(), valueStr.size() );

                Mosaic::String & value = m_mosaicReadout.back();

                Mosaic::TextInputOptions valueOptions;
                valueOptions.readOnly = true;

                Mosaic::Scope valueScope = Mosaic::scope( m_mosaicContext, Mosaic::Key( *_labelCounter ) );

                Mosaic::setNextItemWidth( m_mosaicContext, 25.f + 8.f * (float)valueStr.size() );

                Mosaic::inputText( m_mosaicContext, "value", &value, valueOptions );
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::ShowResponseDataForId( uint32_t _id )
    {
        VectorNetwork::const_iterator responseIterator = std::find_if( m_network.cbegin(), m_network.cend(), [_id]( const NetworkDesk & _desk )
        {
            return (_id == _desk.id) && (_desk.type == "Response");
        } );

        bool clicked = Mosaic::button( m_mosaicContext, "copy" ).clicked();

        if( responseIterator == m_network.end() )
        {
            this->TextLine( {1.f, 1.f, 1.f, 1.f}, "Not receive response for request ID: %ug", _id );

            if( clicked == true )
            {
                m_mosaicPlatform.setClipboardText( "Not receive response for request" );
            }
        }
        else
        {
            const Char * responseStr = responseIterator->url.c_str();
            String::size_type responseStrSize = responseIterator->url.size();
            jpp::object responseJpp = jpp::load( responseStr, responseStrSize, jpp::JPP_LOAD_MODE_DISABLE_EOF_CHECK, nullptr, nullptr );

            uint32_t labelCounter = 0;
            this->ShowResponseJpp( responseJpp, 0, &labelCounter );

            if( clicked == true )
            {
                String jppstr;
                jpp::dump( responseJpp, 2, []( const char * _buffer, jpp::jpp_size_t _size, void * _ud )
                {
                    String * jppstr = (String *)_ud;

                    jppstr->append( _buffer, _size );

                    return 0;
                }, &jppstr );

                m_mosaicPlatform.setClipboardText( jppstr.c_str() );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DoUISettingsTab()
    {
        Mosaic::Scope workspace = Mosaic::split( m_mosaicContext, "Settings workspace", Mosaic::Orientation::Horizontal, 0.4f, this->FillLayout() );

        {
            Mosaic::Scope list = Mosaic::scrollArea( m_mosaicContext, "Settings list", Mosaic::Orientation::Vertical, this->FillLayout() );

            Mosaic::TreeScope header = Mosaic::collapsingHeader( m_mosaicContext, "Settings:", true );

            if( header.expanded() == true )
            {
                uint32_t settingIndex = 0;

                for( const SettingDesc & desc : m_settings )
                {
                    bool selected = m_selectedSetting == desc.name;

                    if( Mosaic::selectable( m_mosaicContext, Mosaic::Key( settingIndex ), desc.name.c_str(), selected ).clicked() == true )
                    {
                        m_selectedSetting = desc.name;
                    }

                    ++settingIndex;
                }
            }
        }

        {
            Mosaic::Scope values = Mosaic::scrollArea( m_mosaicContext, "Settings values", Mosaic::Orientation::Vertical, this->FillLayout() );

            for( SettingDesc & desc : m_settings )
            {
                if( m_selectedSetting != desc.name )
                {
                    continue;
                }

                uint32_t keyIndex = 0;

                for( SettingKeyDesc & key : desc.keys )
                {
                    Mosaic::Scope keyScope = Mosaic::scope( m_mosaicContext, Mosaic::Key( keyIndex ) );

                    ++keyIndex;

                    switch( key.type )
                    {
                    case EST_BOOL:
                        {
                            bool v;
                            Helper::stringalized( key.value, &v );

                            if( Mosaic::checkbox( m_mosaicContext, key.name.c_str(), &v ).changed() == true )
                            {
                                Helper::stringalized( v, key.value, 256 );

                                this->SendSetting( desc.name, key.name, key.value );
                            }
                        }break;
                    case EST_INTEGER:
                        {
                            int32_t v;
                            Helper::stringalized( key.value, &v );

                            Mosaic::Scope row = this->PropertyRow( key.name.c_str() );

                            if( Mosaic::inputInt( m_mosaicContext, key.name.c_str(), &v ).committed() == true )
                            {
                                Helper::stringalized( v, key.value, 256 );

                                this->SendSetting( desc.name, key.name, key.value );
                            }
                        }break;
                    case EST_REAL:
                        {
                            float v;
                            Helper::stringalized( key.value, &v );

                            Mosaic::Scope row = this->PropertyRow( key.name.c_str() );

                            if( Mosaic::inputFloat( m_mosaicContext, key.name.c_str(), &v ).committed() == true )
                            {
                                Helper::stringalized( v, key.value, 256 );

                                this->SendSetting( desc.name, key.name, key.value );
                            }
                        }break;
                    case EST_STRING:
                        {
                            m_mosaicReadout.emplace_back( key.value );

                            Mosaic::String & value = m_mosaicReadout.back();

                            Mosaic::Scope row = this->PropertyRow( key.name.c_str() );

                            if( Mosaic::inputText( m_mosaicContext, key.name.c_str(), &value ).committed() == true )
                            {
                                StdString::strcpy_safe( key.value, value.c_str(), 256 );

                                this->SendSetting( desc.name, key.name, key.value );
                            }
                        }break;
                    case EST_VEC2F:
                        {
                            mt::vec2f v;
                            Helper::stringalized( key.value, &v );

                            float buff[2] = {v.x, v.y};

                            Mosaic::Scope row = this->PropertyRow( key.name.c_str() );

                            Mosaic::FloatSpan values( buff, 2 );

                            if( Mosaic::inputFloatVector( m_mosaicContext, key.name.c_str(), values ).committed() == true )
                            {
                                v.from_f2( buff );

                                Helper::stringalized( v, key.value, 256 );

                                this->SendSetting( desc.name, key.name, key.value );
                            }
                        }break;
                    case EST_VEC3F:
                        {
                            mt::vec3f v;
                            Helper::stringalized( key.value, &v );

                            float buff[3] = {v.x, v.y, v.z};

                            Mosaic::Scope row = this->PropertyRow( key.name.c_str() );

                            Mosaic::FloatSpan values( buff, 3 );

                            if( Mosaic::inputFloatVector( m_mosaicContext, key.name.c_str(), values ).committed() == true )
                            {
                                v.from_f3( buff );

                                Helper::stringalized( v, key.value, 256 );

                                this->SendSetting( desc.name, key.name, key.value );
                            }
                        }break;
                    case EST_COLOR:
                        {
                            Color v;
                            Helper::stringalized( key.value, &v );

                            Mosaic::Color color = {v.getR(), v.getG(), v.getB(), v.getA()};

                            if( Mosaic::colorEditorRgba( m_mosaicContext, key.name.c_str(), &color ).changed() == true )
                            {
                                float buff[4] = {color.r, color.g, color.b, color.a};

                                v.setRGBA4( buff );

                                Helper::stringalized( v, key.value, 256 );

                                this->SendSetting( desc.name, key.name, key.value );
                            }
                        }break;
                    default:
                        break;
                    }
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::RenderMosaic()
    {
        if( m_mosaicContext == nullptr )
        {
            return;
        }

        Mosaic::Viewport viewport;
        viewport.id = 1;
        viewport.bounds = {0.f, 0.f, (float)m_width, (float)m_height};

        Mosaic::beginFrame( m_mosaicContext, m_mosaicInput, viewport );

        this->DoUI();

        const Mosaic::Frame & frame = Mosaic::endFrame( m_mosaicContext );

        for( const Mosaic::String & diagnostic : frame.diagnostics )
        {
            std::printf( "mosaic diagnostic: %s\n", diagnostic.c_str() );
        }

        if( m_mosaicBridge.prepare( frame, m_mosaicPlatform ) == false )
        {
            return;
        }

        const Mosaic::RenderMesh * mesh = m_mosaicBridge.renderData();

        if( mesh == nullptr )
        {
            return;
        }

        m_mosaicRenderer.render( frame.viewports.front(), *mesh );

        m_mosaicInput.keyboard.clear();
        m_mosaicInput.text.clear();
        m_mosaicInput.ime.clear();
        m_mosaicInput.wheel = {};

        m_mosaicPointerPressed = 0;
        m_mosaicPointerReleased = 0;
        m_mosaicPointerClicks = 0;
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

            if( glfwGetWindowAttrib( m_window, GLFW_FOCUSED ) == GLFW_FALSE )
            {
                std::this_thread::sleep_for( std::chrono::microseconds( 100 ) );

                continue;
            }

            this->Update( dt );

            glClearColor( 0.412f, 0.796f, 1.0f, 1.0f );
            glClear( GL_COLOR_BUFFER_BIT );

            this->UpdateMosaicInput( dt );
            this->RenderMosaic();

            glfwSwapBuffers( m_window );

            std::this_thread::sleep_for( std::chrono::microseconds( 1 ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::Shutdown()
    {
        m_shutdown = true;

        this->FinalizeMosaic();

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
    void NodeDebuggerApp::Resize( int _width, int _height )
    {
        if( m_width != _width || m_height != _height )
        {
            m_width = _width;
            m_height = _height;
            glViewport( 0, 0, m_width, m_height );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::Update( double _dt )
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
                if( m_selectedNode != nullptr && m_selectedNode->dirty == true )
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

                if( m_muteRequested == true )
                {
                    this->SendMuteRequest();
                    m_muteRequested = false;
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::CompressPacket( NodeDebuggerPacket & _packet, PacketHeader & _hdr )
    {
        Data::size_type payloadSize = _packet.payload.size();

        if( payloadSize < 1024 )
        {
            _hdr.compressedSize = static_cast<uint32_t>(payloadSize);
            _hdr.uncompressedSize = 0; // packet is not compressed
        }
        else
        {
            const size_t maxCompressedSize = ::LZ4_compressBound( static_cast<int>(payloadSize) );
            Mengine::Data compressedPayload( maxCompressedSize );

            int result = ::LZ4_compress_default( reinterpret_cast<char *>(_packet.payload.data()), reinterpret_cast<char *>(compressedPayload.data()), static_cast<int>(payloadSize), static_cast<int>(maxCompressedSize) );

            if( result < 0 || payloadSize < (Data::size_type)result )
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
            MENGINE_UNUSED( result );
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
        else if( typeStr == "Sounds" )
        {
            this->ReceiveSounds( payloadNode );
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
        m_settings.clear();

        for( const pugi::xml_node & setting : _xmlContainer.children() )
        {
            const pugi::char_t * name = setting.attribute( "name" ).value();
            const pugi::char_t * file = setting.attribute( "file" ).value();

            SettingDesc desc;
            desc.name = name;
            desc.file = file;

            for( const pugi::xml_node & key : setting.children( "Key" ) )
            {
                const pugi::char_t * key_name = key.attribute( "name" ).value();
                const pugi::char_t * key_type = key.attribute( "type" ).value();
                const pugi::char_t * key_value = key.attribute( "value" ).value();

                SettingKeyDesc key_desc;
                key_desc.name = key_name;

                if( strcmp( key_type, "bool" ) == 0 )
                {
                    key_desc.type = EST_BOOL;
                }
                else if( strcmp( key_type, "int" ) == 0 )
                {
                    key_desc.type = EST_INTEGER;
                }
                else if( strcmp( key_type, "real" ) == 0 )
                {
                    key_desc.type = EST_REAL;
                }
                else if( strcmp( key_type, "string" ) == 0 )
                {
                    key_desc.type = EST_STRING;
                }
                else if( strcmp( key_type, "vec2f" ) == 0 )
                {
                    key_desc.type = EST_VEC2F;
                }
                else if( strcmp( key_type, "vec3f" ) == 0 )
                {
                    key_desc.type = EST_VEC3F;
                }
                else if( strcmp( key_type, "color" ) == 0 )
                {
                    key_desc.type = EST_COLOR;
                }
                else
                {
                    key_desc.type = EST_NONE;
                }

                StdString::strcpy_safe( key_desc.value, key_value, 256 );

                desc.keys.emplace_back( key_desc );
            }

            m_settings.emplace_back( desc );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::ReceiveSounds( const pugi::xml_node & _xmlContainer )
    {
        m_sounds.clear();

        for( const pugi::xml_node & sound : _xmlContainer.children() )
        {
            uint32_t id = sound.attribute( "id" ).as_uint();
            bool streamable = sound.attribute( "streamable" ).as_bool();
            bool loop = sound.attribute( "loop" ).as_bool();
            bool turn = sound.attribute( "turn" ).as_bool();
            uint32_t category = sound.attribute( "category" ).as_uint();
            uint32_t state = sound.attribute( "state" ).as_uint();
            float time_left = sound.attribute( "time_left" ).as_float();
            float volume = sound.attribute( "volume" ).as_float();

            const pugi::char_t * file = sound.attribute( "file" ).value();

            SoundDesc desc;
            desc.id = id;
            desc.streamable = streamable;
            desc.loop = loop;
            desc.turn = turn;
            desc.category = (ESoundSourceCategory)category;
            desc.state = (ESoundSourceState)state;
            desc.time_left = time_left;
            desc.volume = volume;
            desc.file = file;

            m_sounds.emplace_back( desc );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::ReceiveSelectedNode( const pugi::xml_node & _xmlContainer )
    {
        if( m_selectedTab == "scene" )
        {
            pugi::xml_node node = _xmlContainer.child( "Node" );

            uint32_t id = node.attribute( "SelectedNodeId" ).as_uint();

            //ToDo

            MENGINE_UNUSED( id );

            String pathToRoot = node.attribute( "PathToRoot" ).value();

            String selectedNodeName = node.attribute( "SelectedNodeName" ).value();

            Vector<uint32_t> path = this->StringToPath( pathToRoot );

            m_pathToSelectedNode = path;

            Vector<DebuggerNode *> childrens = m_scene->children;
            DebuggerNode * selectedNode = nullptr;
            for( Vector<uint32_t>::reverse_iterator iter = path.rbegin() + 1; iter != path.rend(); ++iter )
            {
                uint32_t value = *iter;
                Vector<DebuggerNode *>::iterator childrenForUid = Helper::findUnique( childrens, value, &DebuggerNode::uid );

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

                pugi::xml_node contentImageNode = typeSurfaceImageNode.child( "Content" );

                _node->componentSurface.surfaceImage.isContent = contentImageNode;

                if( _node->componentSurface.surfaceImage.isContent == true )
                {
                    _node->componentSurface.surfaceImage.content.deserialize( contentImageNode );
                }

                pugi::xml_node atlasNode = typeSurfaceImageNode.child( "Atlas" );

                _node->componentSurface.hasAtlas = atlasNode;

                if( _node->componentSurface.hasAtlas == true )
                {
                    _node->componentSurface.atlas.deserialize( atlasNode );

                    pugi::xml_node contentAtlasNode = atlasNode.child( "Content" );

                    _node->componentSurface.atlas.isContent = contentAtlasNode;

                    if( _node->componentSurface.atlas.isContent == true )
                    {
                        _node->componentSurface.atlas.content.deserialize( contentAtlasNode );
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
    Vector<uint32_t> NodeDebuggerApp::StringToPath( const String & _pathStr )
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
            Vector<DebuggerNode *>::const_iterator found = Helper::findUnique( node->children, nextUid, &DebuggerNode::uid );

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
                size_t pixelsSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;

                Mosaic::TextureHandle texture = m_mosaicRenderer.createTexture( static_cast<uint32_t>(width), static_cast<uint32_t>(height), Mosaic::ByteSpan( reinterpret_cast<const std::byte *>(data), pixelsSize ) );

                stbi_image_free( data );

                m_imagesCache.push_back( {_name, texture, static_cast<size_t>(width), static_cast<size_t>(height)} );

                result = &m_imagesCache.back();
            }
        }

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::LoadIconsAtlas()
    {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file( "Icons.xml" );
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
    void NodeDebuggerApp::addSpacesWithMultiplier( String * const _out, uint32_t _spacesCount, uint32_t _multiplier )
    {
        _out->append( _spacesCount * _multiplier, ' ' );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::GetValueStringForJppType( const jpp::object & _object, jpp::e_type _jppType, String * _out, uint32_t _spaceCounter )
    {
        MENGINE_UNUSED( _spaceCounter );

        switch( _jppType )
        {
        case jpp::e_type::JPP_OBJECT:
        case jpp::e_type::JPP_ARRAY:
            {
                _out->clear();
            }break;
        case jpp::e_type::JPP_INTEGER:
            {
                int32_t valueInteger = _object;
                *_out = Helper::stringInt32( valueInteger );
            }break;
        case jpp::e_type::JPP_REAL:
            {
                double valueDouble = _object;
                *_out = Helper::stringDouble( valueDouble );
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
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        struct ResolutionDesc
        {
            char label[256];
            uint32_t width;
            uint32_t height;
            float aspect;
        };

        static const ResolutionDesc resolutions_iPhone_[] =
        {
            {"iPhone SE (1st Gen) 640x1136 [16:9]", 640, 1136, 640.f / 1136.f},
            {"iPhone 6/7/8 750x1334 [16:9]", 750, 1334, 750.f / 1334.f},
            {"iPhone 6+/7+/8+ 1080x1920 [16:9]", 1080, 1920, 1080.f / 1920.f},
            {"iPhone X/XS/11 Pro 1125x2436 [19.5:9]", 1125, 2436, 1125.f / 2436.f},
            {"iPhone XR/11 828x1792 [19.5:9]", 828, 1792, 828.f / 1792.f},
            {"iPhone XS Max/11 Pro Max 1242x2688 [19.5:9]", 1242, 2688, 1242.f / 2688.f},
            {"iPhone 12 mini 1080x2340 [19.5:9]", 1080, 2340, 1080.f / 2340.f},
            {"iPhone 12/12 Pro 1170x2532 [19.5:9]", 1170, 2532, 1170.f / 2532.f},
            {"iPhone 12 Pro Max 1284x2778 [19.5:9]", 1284, 2778, 1284.f / 2778.f},
            {"iPhone 13 mini 1080x2340 [19.5:9]", 1080, 2340, 1080.f / 2340.f},
            {"iPhone 13/13 Pro 1170x2532 [19.5:9]", 1170, 2532, 1170.f / 2532.f},
            {"iPhone 13 Pro Max 1284x2778 [19.5:9]", 1284, 2778, 1284.f / 2778.f},
            {"iPhone 14/14 Pro 1179x2556 [19.5:9]", 1179, 2556, 1179.f / 2556.f},
            {"iPhone 14 Plus 1284x2778 [19.5:9]", 1284, 2778, 1284.f / 2778.f},
            {"iPhone 14 Pro Max 1290x2796 [19.5:9]", 1290, 2796, 1290.f / 2796.f},
            {"iPhone 15/15 Pro 1179x2556 [19.5:9]", 1179, 2556, 1179.f / 2556.f},
            {"iPhone 15 Plus 1290x2796 [19.5:9]", 1290, 2796, 1290.f / 2796.f},
            {"iPhone 15 Pro Max 1290x2796 [19.5:9]", 1290, 2796, 1290.f / 2796.f},
            {"iPhone 16 1179x2556 [~19.5:9]", 1179, 2556, 1179.f / 2556.f},
            {"iPhone 16 Plus 1290x2796 [~19.5:9]", 1290, 2796, 1290.f / 2796.f},
            {"iPhone 16 Pro 1206x2622 [~19.5:9]", 1206, 2622, 1206.f / 2622.f},
            {"iPhone 16 Pro Max 1320x2868 [~19.5:9]", 1320, 2868, 1320.f / 2868.f},
        };

        static const ResolutionDesc resolutions_iPad_[] =
        {
            {"iPad (1st-4th Gen) 1024x768 [4:3]", 1024, 768, 1024.f / 768.f},
            {"iPad Air (1st Gen) 2048x1536 [4:3]", 2048, 1536, 2048.f / 1536.f},
            {"iPad Air 2 2048x1536 [4:3]", 2048, 1536, 2048.f / 1536.f},
            {"iPad Pro 9.7\" 2048x1536 [4:3]", 2048, 1536, 2048.f / 1536.f},
            {"iPad Pro 10.5\" 2224x1668 [4:3]", 2224, 1668, 2224.f / 1668.f},
            {"iPad Pro 11\" (1st Gen) 2388x1668 [4:3]", 2388, 1668, 2388.f / 1668.f},
            {"iPad Pro 11\" (2nd-4th Gen) 2388x1668 [4:3]", 2388, 1668, 2388.f / 1668.f},
            {"iPad Pro 12.9\" (1st Gen) 2732x2048 [4:3]", 2732, 2048, 2732.f / 2048.f},
            {"iPad Pro 12.9\" (2nd-6th Gen) 2732x2048 [4:3]", 2732, 2048, 2732.f / 2048.f},
            {"iPad Mini (1st Gen) 1024x768 [4:3]", 1024, 768, 1024.f / 768.f},
            {"iPad Mini (2nd-5th Gen) 2048x1536 [4:3]", 2048, 1536, 2048.f / 1536.f},
            {"iPad Mini (6th Gen) 2266x1488 [4:3]", 2266, 1488, 2266.f / 1488.f},
            {"iPad (5th-9th Gen) 2048x1536 [4:3]", 2048, 1536, 2048.f / 1536.f},
            {"iPad (10th Gen) 2360x1640 [4:3]", 2360, 1640, 2360.f / 1640.f},
            {"iPad Pro 11\" (2022) 2388x1668 [~4:3]", 2388, 1668, 2388.f / 1668.f},
        };

        static const ResolutionDesc resolutions_Android_[] =
        {
            {"Android HD 720x1280 [16:9]", 720, 1280, 720.f / 1280.f},
            {"Android HD+ 720x1600 [20:9]", 720, 1600, 720.f / 1600.f},
            {"Android FHD 1080x1920 [16:9]", 1080, 1920, 1080.f / 1920.f},
            {"Android FHD+ 1080x2160 [18:9]", 1080, 2160, 1080.f / 2160.f},
            {"Android FHD+ 1080x2340 [19.5:9]", 1080, 2340, 1080.f / 2340.f},
            {"Android FHD+ 1080x2400 [20:9]", 1080, 2400, 1080.f / 2400.f},
            {"Android QHD+ 1440x3200 [20:9]", 1440, 3200, 1440.f / 3200.f},
        };
        //////////////////////////////////////////////////////////////////////////
        static void mosaicResolutionList_( Mosaic::Context * _ui, const Char * _title, const ResolutionDesc * _resolutions, size_t _count, uint32_t * const _index, const ResolutionDesc ** const _select, const ResolutionDesc * _apply )
        {
            {
                Mosaic::Key titleKey( (*_index)++ );

                Mosaic::Scope titleScope = Mosaic::scope( _ui, titleKey );

                Mosaic::Theme theme = Mosaic::getTheme( _ui );
                theme.colors.text = theme.colors.textDisabled;

                Mosaic::Scope titleStyle = Mosaic::styleScope( _ui, theme );

                Mosaic::text( _ui, _title );
            }

            for( size_t resolutionIndex = 0; resolutionIndex != _count; ++resolutionIndex )
            {
                const ResolutionDesc & res = _resolutions[resolutionIndex];

                Mosaic::Key rowKey( (*_index)++ );

                Mosaic::Scope rowScope = Mosaic::scope( _ui, rowKey );

                Mosaic::Theme theme = Mosaic::getTheme( _ui );

                if( _apply == &res )
                {
                    theme.colors.header = {1.f, 0.5f, 0.f, 1.f};
                    theme.colors.headerHovered = {1.f, 0.7f, 0.3f, 1.f};
                    theme.colors.headerActive = {1.f, 0.3f, 0.f, 1.f};
                }

                Mosaic::Scope rowStyle = Mosaic::styleScope( _ui, theme );

                bool selected = *_select == &res || _apply == &res;

                if( Mosaic::selectable( _ui, res.label, selected ).clicked() == true )
                {
                    *_select = &res;
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DoUIResolutionsTab()
    {
        static const Detail::ResolutionDesc * select_resolution = nullptr;
        static const Detail::ResolutionDesc * apply_resolution = nullptr;

        Mosaic::Scope workspace = Mosaic::split( m_mosaicContext, "Resolutions workspace", Mosaic::Orientation::Horizontal, 0.6f, this->FillLayout() );

        {
            Mosaic::Scope tree = Mosaic::scrollArea( m_mosaicContext, "ResoulutionTree", Mosaic::Orientation::Vertical, this->FillLayout() );

            Mosaic::TreeScope header = Mosaic::collapsingHeader( m_mosaicContext, "Resolutions:", true );

            if( header.expanded() == true )
            {
                uint32_t index = 0;

                Detail::mosaicResolutionList_( m_mosaicContext, "iOS:", Detail::resolutions_iPhone_, MENGINE_ARRAY_SIZE( Detail::resolutions_iPhone_ ), &index, &select_resolution, apply_resolution );

                Mosaic::spacer( m_mosaicContext, 4.f );

                Detail::mosaicResolutionList_( m_mosaicContext, "iPad:", Detail::resolutions_iPad_, MENGINE_ARRAY_SIZE( Detail::resolutions_iPad_ ), &index, &select_resolution, apply_resolution );

                Mosaic::spacer( m_mosaicContext, 4.f );

                Detail::mosaicResolutionList_( m_mosaicContext, "Android:", Detail::resolutions_Android_, MENGINE_ARRAY_SIZE( Detail::resolutions_Android_ ), &index, &select_resolution, apply_resolution );
            }
        }

        {
            Mosaic::Scope apply = Mosaic::scrollArea( m_mosaicContext, "Resolution apply", Mosaic::Orientation::Vertical, this->FillLayout() );

            if( select_resolution == nullptr )
            {
                return;
            }

            if( Mosaic::button( m_mosaicContext, "Apply" ).clicked() == true )
            {
                apply_resolution = select_resolution;

                uint32_t width = apply_resolution->width;
                uint32_t height = apply_resolution->height;

                this->SendResolutionRequest( width, height );
            }

            if( apply_resolution != nullptr )
            {
                this->TextLine( Mosaic::getTheme( m_mosaicContext ).colors.text, "Apply resolution: %s", apply_resolution->label );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    String NodeDebuggerApp::DoIPInput( const Char * _title, const String & _inIP )
    {
        int32_t octets[4] = {127, 0, 0, 1};

        if( _inIP.empty() == false )
        {
            int32_t idx = 0;
            octets[idx] = 0;

            for( const Char c : _inIP )
            {
                if( isdigit( c ) )
                {
                    octets[idx] *= 10;
                    octets[idx] += static_cast<int32_t>(c - '0');
                }
                else
                {
                    ++idx;

                    if( idx > 3 )
                    {
                        break;
                    }

                    octets[idx] = 0;
                }
            }
        }

        bool invalidAbove = false;
        bool invalidBelow = false;

        for( int32_t & octet : octets )
        {
            if( octet > 255 )
            {
                octet = 255;
                invalidAbove = true;
            }

            if( octet < 0 )
            {
                octet = 0;
                invalidBelow = true;
            }
        }

        Mosaic::Theme theme = Mosaic::getTheme( m_mosaicContext );

        if( invalidAbove == true )
        {
            theme.colors.text = {1.f, 0.f, 0.f, 1.f};
        }
        else if( invalidBelow == true )
        {
            theme.colors.text = {1.f, 1.f, 0.f, 1.f};
        }

        Mosaic::Scope style = Mosaic::styleScope( m_mosaicContext, theme );

        Mosaic::Scope row = this->PropertyRow( _title );

        Mosaic::Int32Span values( octets, 4 );

        Mosaic::inputIntVector( m_mosaicContext, _title, values, 0, 0 );

        Stringstream ss;
        ss << octets[0] << '.' << octets[1] << '.' << octets[2] << '.' << octets[3];

        return ss.str();
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DoUISceneDebuggerTab()
    {
        Mosaic::Scope workspace = Mosaic::split( m_mosaicContext, "Scene workspace", Mosaic::Orientation::Horizontal, 0.4f, this->FillLayout() );

        {
            Mosaic::Scope tree = Mosaic::scrollArea( m_mosaicContext, "Scene tree", Mosaic::Orientation::Vertical, this->FillLayout() );

            m_mosaicNodePathApplied = true;

            {
                Mosaic::TreeScope header = Mosaic::collapsingHeader( m_mosaicContext, "Server:", true );

                if( header.expanded() == true )
                {
                    m_serverAddress = this->DoIPInput( "Address:", m_serverAddress );

                    {
                        int32_t port = static_cast<int32_t>(m_serverPort);

                        Mosaic::Scope portRow = this->PropertyRow( "IP Port:" );

                        if( Mosaic::inputInt( m_mosaicContext, "IP Port:", &port, 0, 0 ).changed() == true )
                        {
                            m_serverPort = static_cast<uint16_t>(port & 0xFFFF);
                        }
                    }

                    if( m_connectionStatus == ConnectionStatus::Connected )
                    {
                        Mosaic::Theme theme = Mosaic::getTheme( m_mosaicContext );
                        theme.colors.button = {0.5f, 0.f, 0.f, 1.f};
                        theme.colors.buttonHovered = theme.colors.button;

                        Mosaic::Scope style = Mosaic::styleScope( m_mosaicContext, theme );

                        if( Mosaic::button( m_mosaicContext, "Disconnect" ).clicked() == true )
                        {
                            this->OnDisconnectButton();
                        }
                    }
                    else if( m_connectionStatus == ConnectionStatus::Disconnected || m_connectionStatus == ConnectionStatus::ConnectionFailed )
                    {
                        Mosaic::Theme theme = Mosaic::getTheme( m_mosaicContext );
                        theme.colors.button = {0.1686f, 0.5686f, 0.f, 1.f};
                        theme.colors.buttonHovered = theme.colors.button;

                        Mosaic::Scope style = Mosaic::styleScope( m_mosaicContext, theme );

                        if( Mosaic::button( m_mosaicContext, "Connect" ).clicked() == true )
                        {
                            this->OnConnectButton();
                        }
                    }
                }
            }

            {
                Mosaic::TreeScope header = Mosaic::collapsingHeader( m_mosaicContext, "Game controls:" );

                if( header.expanded() == true )
                {
                    {
                        Mosaic::Scope disabled = Mosaic::disabledScope( m_mosaicContext, m_updateSceneOnChange );

                        int32_t hz = m_sceneUpdateFreq;

                        Mosaic::Scope row = this->PropertyRow( "Update freq (hz):" );

                        if( Mosaic::inputInt( m_mosaicContext, "Update freq (hz):", &hz ).changed() == true )
                        {
                            m_sceneUpdateFreq = std::clamp( hz, 0, 30 );
                            m_sceneUpdateTimer = 0.0;
                        }
                    }

                    if( Mosaic::checkbox( m_mosaicContext, "Update scene on change", &m_updateSceneOnChange ).changed() == true )
                    {
                        if( m_updateSceneOnChange == true )
                        {
                            m_sceneUpdateFreq = 0;
                            m_sceneUpdateTimer = 0.0;
                        }
                    }

                    if( Mosaic::button( m_mosaicContext, "Pause game" ).clicked() == true )
                    {
                        this->OnPauseButton();
                    }

                    if( Mosaic::button( m_mosaicContext, "Mute sound" ).clicked() == true )
                    {
                        this->OnMuteButton();
                    }
                }
            }

            static int32_t SceneTagId = 1;

            {
                Mosaic::TreeScope header = Mosaic::collapsingHeader( m_mosaicContext, "Type:" );

                if( header.expanded() == true )
                {
                    Mosaic::Scope types = Mosaic::row( m_mosaicContext );

                    if( Mosaic::radioButton( m_mosaicContext, "Arrow", SceneTagId == 0 ).clicked() == true )
                    {
                        SceneTagId = 0;
                    }

                    if( Mosaic::radioButton( m_mosaicContext, "Full", SceneTagId == 1 ).clicked() == true )
                    {
                        SceneTagId = 1;
                    }

                    if( Mosaic::radioButton( m_mosaicContext, "Picker", SceneTagId == 2 ).clicked() == true )
                    {
                        SceneTagId = 2;
                    }

                    if( Mosaic::radioButton( m_mosaicContext, "Render", SceneTagId == 3 ).clicked() == true )
                    {
                        SceneTagId = 3;
                    }
                }
            }

            {
                Mosaic::String filter( m_selectFilter );

                Mosaic::Scope row = this->PropertyRow( "Filter:" );

                if( Mosaic::inputText( m_mosaicContext, "Filter:", &filter ).changed() == true )
                {
                    StdString::strcpy_safe( m_selectFilter, filter.c_str(), 2048 );
                }
            }

            switch( SceneTagId )
            {
            case 0:
                {
                    Mosaic::TreeScope header = Mosaic::collapsingHeader( m_mosaicContext, "Arrow:", true );

                    if( header.expanded() == true && m_arrow != nullptr )
                    {
                        this->DoNodeElement( m_arrow, m_selectFilter, &m_selectedArrowNode );

                        m_selectedNode = m_selectedArrowNode;
                    }
                }break;
            case 1:
                {
                    Mosaic::TreeScope header = Mosaic::collapsingHeader( m_mosaicContext, "Scene:", true );

                    if( header.expanded() == true && m_scene != nullptr )
                    {
                        this->DoNodeElement( m_scene, m_selectFilter, &m_selectedSceneNode );

                        m_selectedNode = m_selectedSceneNode;

                        if( m_mosaicNodePathApplied == true )
                        {
                            m_pathToSelectedNode.clear();
                        }
                    }
                }break;
            case 2:
                {
                    Mosaic::TreeScope header = Mosaic::collapsingHeader( m_mosaicContext, "Pickerable:", true );

                    if( header.expanded() == true && m_scenePickerable != nullptr )
                    {
                        this->DoNodeElement( m_scenePickerable, m_selectFilter, &m_selectedPickerableNode );

                        m_selectedNode = m_selectedPickerableNode;
                    }
                }break;
            case 3:
                {
                    Mosaic::TreeScope header = Mosaic::collapsingHeader( m_mosaicContext, "Renderable:", true );

                    if( header.expanded() == true && m_sceneRenderable != nullptr )
                    {
                        this->DoNodeElement( m_sceneRenderable, m_selectFilter, &m_selectedRenderableNode );

                        m_selectedNode = m_selectedRenderableNode;
                    }
                }break;
            }
        }

        {
            Mosaic::Scope panel = Mosaic::scrollArea( m_mosaicContext, "Scene panel", Mosaic::Orientation::Vertical, this->FillLayout() );

            if( m_selectedNode != nullptr )
            {
                this->DoNodeProperties( m_selectedNode );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DoNodeElement( DebuggerNode * _node, const Char * _filter, DebuggerNode ** _selectedNode )
    {
        if( *_filter != '\0' && _node->name.find( _filter ) == String::npos )
        {
            for( DebuggerNode * child : _node->children )
            {
                this->DoNodeElement( child, _filter, _selectedNode );
            }

            return;
        }

        const String & treeNodeName = _node->name.empty() == false ? _node->name : "***unnamed***";

        Char label[1024] = {'\0'};
        MENGINE_SNPRINTF( label, 1023, "%s [%s]", treeNodeName.c_str(), _node->type.c_str() );

        Mosaic::TreeNodeOptions options;
        options.openOnDoubleClick = true;
        options.openOnArrow = true;
        options.leaf = _node->children.empty();
        options.selected = *_selectedNode == _node;

        Mosaic::Key key = Mosaic::Key( _node->uid );

        bool onPath = std::find( m_pathToSelectedNode.begin(), m_pathToSelectedNode.end(), _node->uid ) != m_pathToSelectedNode.end();

        if( onPath == true && _node->uid == m_pathToSelectedNode[0] )
        {
            options.selected = true;
        }

        Mosaic::Scope disabled = Mosaic::disabledScope( m_mosaicContext, _node->enable == false );

        if( _node->icon != nullptr )
        {
            {
                Mosaic::Scope iconScope = Mosaic::scope( m_mosaicContext, key );

                Mosaic::ImageOptions imageOptions;
                imageOptions.uv = {_node->icon->uv0_X, _node->icon->uv0_Y, _node->icon->uv1_X - _node->icon->uv0_X, _node->icon->uv1_Y - _node->icon->uv0_Y};

                Mosaic::image( m_mosaicContext, _node->icon->image, {16.f, 16.f}, imageOptions );
            }

            Mosaic::sameLine( m_mosaicContext );
        }

        Mosaic::TreeScope tree = Mosaic::treeNode( m_mosaicContext, key, label, options );

        if( onPath == true && options.leaf == false && tree.expanded() == false )
        {
            Mosaic::setTreeExpanded( m_mosaicContext, tree.id(), true );

            m_mosaicNodePathApplied = false;
        }

        Mosaic::Response response;

        if( Mosaic::itemResponse( m_mosaicContext, tree.id(), &response ) == true )
        {
            if( response.clicked() == true )
            {
                this->OnSelectNode( _node, _selectedNode );
            }
        }

        if( tree.expanded() == false )
        {
            return;
        }

        for( DebuggerNode * child : _node->children )
        {
            this->DoNodeElement( child, _filter, _selectedNode );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    Mosaic::Theme NodeDebuggerApp::ReadOnlyTheme() const
    {
        Mosaic::Theme theme = Mosaic::getTheme( m_mosaicContext );
        theme.colors.frame = {0.15f, 0.3f, 0.2f, 1.f};
        theme.colors.frameHovered = theme.colors.frame;
        theme.colors.frameActive = theme.colors.frame;

        return theme;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::DoNodeProperties( DebuggerNode * _node )
    {
        Mosaic::Context * ui = m_mosaicContext;

        Mosaic::SliderOptions unitOptions;
        unitOptions.minimum = 0.0;
        unitOptions.maximum = 0.0;
        unitOptions.dragSpeed = 1.0;

        Mosaic::SliderOptions fineOptions = unitOptions;
        fineOptions.dragSpeed = 0.01;

        typedef std::initializer_list<Mosaic::StringView> InitializerListMosaicStringView;

        auto uiEditorBool = [ui, _node]( const Char * _caption, bool & _prop )
        {
            Mosaic::Key key = Detail::makeKey_( _caption );

            Mosaic::Scope scope = Mosaic::scope( ui, key );

            bool testValue = _prop;

            if( Mosaic::checkbox( ui, _caption, &testValue ).changed() == false )
            {
                return;
            }

            if( testValue == _prop )
            {
                return;
            }

            _prop = testValue;
            _node->dirty = true;
        };

        auto uiEditorBoolHidden = [ui, _node]( const Char * _key, bool & _prop )
        {
            Mosaic::Key key = Detail::makeKey_( _key );

            Mosaic::Scope scope = Mosaic::scope( ui, key );

            bool testValue = _prop;

            if( Mosaic::checkbox( ui, "", &testValue ).changed() == false )
            {
                return;
            }

            if( testValue == _prop )
            {
                return;
            }

            _prop = testValue;
            _node->dirty = true;
        };

        auto uiReadOnlyBool = [this, ui]( const Char * _caption, bool _prop )
        {
            Mosaic::Key key = Detail::makeKey_( _caption );

            Mosaic::Scope scope = Mosaic::scope( ui, key );
            Mosaic::Scope style = Mosaic::styleScope( ui, this->ReadOnlyTheme() );
            Mosaic::Scope disabled = Mosaic::disabledScope( ui, true );

            bool testValue = _prop;

            Mosaic::checkbox( ui, _caption, &testValue );
        };

        auto uiReadOnlyVec1I = [this, ui, unitOptions]( const Char * _caption, int32_t _prop )
        {
            Mosaic::Key key = Detail::makeKey_( _caption );

            Mosaic::Scope scope = Mosaic::scope( ui, key );
            Mosaic::Scope style = Mosaic::styleScope( ui, this->ReadOnlyTheme() );
            Mosaic::Scope disabled = Mosaic::disabledScope( ui, true );

            int32_t testValue = _prop;

            Mosaic::dragValue( ui, _caption, &testValue, unitOptions );
        };

        auto uiEditorVec1I = [ui, _node, unitOptions]( const Char * _caption, int32_t & _prop )
        {
            Mosaic::Key key = Detail::makeKey_( _caption );

            Mosaic::Scope scope = Mosaic::scope( ui, key );

            int32_t testValue = _prop;

            if( Mosaic::dragValue( ui, _caption, &testValue, unitOptions ).changed() == false )
            {
                return;
            }

            if( testValue == _prop )
            {
                return;
            }

            _prop = testValue;
            _node->dirty = true;
        };

        auto uiEditorVec1U = [ui, _node, unitOptions]( const Char * _caption, uint32_t & _prop )
        {
            Mosaic::Key key = Detail::makeKey_( _caption );

            Mosaic::Scope scope = Mosaic::scope( ui, key );

            uint32_t testValue = _prop;

            if( Mosaic::dragValue( ui, _caption, &testValue, (uint32_t)0, (uint32_t)0, unitOptions ).changed() == false )
            {
                return;
            }

            if( testValue == _prop )
            {
                return;
            }

            _prop = testValue;
            _node->dirty = true;
        };

        auto uiEditorVec1f = [ui, _node, fineOptions]( const Char * _caption, float & _prop, bool _enable = true )
        {
            Mosaic::Key key = Detail::makeKey_( _caption );

            Mosaic::Scope scope = Mosaic::scope( ui, key );
            Mosaic::Scope disabled = Mosaic::disabledScope( ui, _enable == false );

            float testValue = _prop;

            if( Mosaic::dragValue( ui, _caption, &testValue, fineOptions ).changed() == false )
            {
                return;
            }

            if( testValue == _prop )
            {
                return;
            }

            _prop = testValue;
            _node->dirty = true;
        };

        auto uiReadOnlyVec1f = [this, ui, unitOptions]( const Char * _caption, float _prop )
        {
            Mosaic::Key key = Detail::makeKey_( _caption );

            Mosaic::Scope scope = Mosaic::scope( ui, key );
            Mosaic::Scope style = Mosaic::styleScope( ui, this->ReadOnlyTheme() );
            Mosaic::Scope disabled = Mosaic::disabledScope( ui, true );

            float testValue = _prop;

            Mosaic::dragValue( ui, _caption, &testValue, unitOptions );
        };

        auto uiEditorVec2f = [this, ui, _node, unitOptions]( const Char * _caption, mt::vec2f & _prop )
        {
            Mosaic::Key key = Detail::makeKey_( _caption );

            Mosaic::Scope scope = Mosaic::scope( ui, key );
            Mosaic::Scope row = this->PropertyRow( _caption );

            mt::vec2f testValue = _prop;

            float * valueData = testValue.buff();

            Mosaic::FloatSpan values( valueData, 2 );

            if( Mosaic::dragFloatVector( ui, _caption, values, unitOptions ).changed() == false )
            {
                return;
            }

            if( testValue == _prop )
            {
                return;
            }

            _prop = testValue;
            _node->dirty = true;
        };

        auto uiReadOnlyVec2f = [this, ui, unitOptions]( const Char * _caption, const mt::vec2f & _prop )
        {
            Mosaic::Key key = Detail::makeKey_( _caption );

            Mosaic::Scope scope = Mosaic::scope( ui, key );
            Mosaic::Scope style = Mosaic::styleScope( ui, this->ReadOnlyTheme() );
            Mosaic::Scope disabled = Mosaic::disabledScope( ui, true );
            Mosaic::Scope row = this->PropertyRow( _caption );

            mt::vec2f testValue = _prop;

            float * valueData = testValue.buff();

            Mosaic::FloatSpan values( valueData, 2 );

            Mosaic::dragFloatVector( ui, _caption, values, unitOptions );
        };

        auto uiEditorVec3f = [this, ui, _node, unitOptions]( const Char * _caption, mt::vec3f & _prop )
        {
            Mosaic::Key key = Detail::makeKey_( _caption );

            Mosaic::Scope scope = Mosaic::scope( ui, key );
            Mosaic::Scope row = this->PropertyRow( _caption );

            mt::vec3f testValue = _prop;

            float * valueData = testValue.buff();

            Mosaic::FloatSpan values( valueData, 3 );

            if( Mosaic::dragFloatVector( ui, _caption, values, unitOptions ).changed() == false )
            {
                return;
            }

            if( testValue == _prop )
            {
                return;
            }

            _prop = testValue;
            _node->dirty = true;
        };

        auto uiReadOnlyVec3f = [this, ui, unitOptions]( const Char * _caption, const mt::vec3f & _prop )
        {
            Mosaic::Key key = Detail::makeKey_( _caption );

            Mosaic::Scope scope = Mosaic::scope( ui, key );
            Mosaic::Scope style = Mosaic::styleScope( ui, this->ReadOnlyTheme() );
            Mosaic::Scope disabled = Mosaic::disabledScope( ui, true );
            Mosaic::Scope row = this->PropertyRow( _caption );

            mt::vec3f testValue = _prop;

            float * valueData = testValue.buff();

            Mosaic::FloatSpan values( valueData, 3 );

            Mosaic::dragFloatVector( ui, _caption, values, unitOptions );
        };

        auto uiEditorColor = [ui, _node]( const Char * _caption, Color & _prop, bool _enable = true )
        {
            Mosaic::Key key = Detail::makeKey_( _caption );

            Mosaic::Scope scope = Mosaic::scope( ui, key );
            Mosaic::Scope disabled = Mosaic::disabledScope( ui, _enable == false );

            Mosaic::Color testValue = {_prop.getR(), _prop.getG(), _prop.getB(), _prop.getA()};

            if( Mosaic::colorEditorRgba( ui, _caption, &testValue ).changed() == false )
            {
                return;
            }

            Color updated( testValue.r, testValue.g, testValue.b, testValue.a );

            if( updated == _prop )
            {
                return;
            }

            _prop = updated;
            _node->dirty = true;
        };

        auto uiReadOnlyColor = [this, ui]( const Char * _caption, const Color & _prop )
        {
            Mosaic::Key key = Detail::makeKey_( _caption );

            Mosaic::Scope scope = Mosaic::scope( ui, key );
            Mosaic::Scope style = Mosaic::styleScope( ui, this->ReadOnlyTheme() );
            Mosaic::Scope disabled = Mosaic::disabledScope( ui, true );

            Mosaic::Color testValue = {_prop.getR(), _prop.getG(), _prop.getB(), _prop.getA()};

            Mosaic::colorEditorRgba( ui, _caption, &testValue );
        };

        auto uiEditorString = [this, ui, _node]( const Char * _caption, String & _prop )
        {
            Mosaic::Key key = Detail::makeKey_( _caption );

            Mosaic::Scope scope = Mosaic::scope( ui, key );

            Mosaic::Scope row = this->PropertyRow( _caption );

            Mosaic::String testValue( _prop.c_str(), _prop.size() );

            if( Mosaic::inputText( ui, _caption, &testValue ).changed() == false )
            {
                return;
            }

            if( testValue.c_str() == _prop )
            {
            _prop.assign( testValue.c_str(), testValue.size() );
            _node->dirty = true;
            }
        };

        auto uiReadOnlyString = [this, ui]( const Char * _caption, const String & _prop )
        {
            Mosaic::Key key = Detail::makeKey_( _caption );

            Mosaic::Scope scope = Mosaic::scope( ui, key );
            Mosaic::Scope style = Mosaic::styleScope( ui, this->ReadOnlyTheme() );

            Mosaic::Scope row = this->PropertyRow( _caption );

            Mosaic::String testValue( _prop.c_str(), _prop.size() );

            Mosaic::TextInputOptions options;
            options.readOnly = true;

            Mosaic::inputText( ui, _caption, &testValue, options );
        };

        auto uiReadOnlyUV4 = [this, ui, unitOptions]( const Char * _caption, const mt::uv4f & _prop )
        {
            Mosaic::Key key = Detail::makeKey_( _caption );

            Mosaic::Scope scope = Mosaic::scope( ui, key );
            Mosaic::Scope style = Mosaic::styleScope( ui, this->ReadOnlyTheme() );
            Mosaic::Scope disabled = Mosaic::disabledScope( ui, true );

            mt::uv4f testValue = _prop;

            mt::vec2f * points[] = {&testValue.p0, &testValue.p1, &testValue.p2, &testValue.p3};

            for( uint32_t pointIndex = 0; pointIndex != 4; ++pointIndex )
            {
                Mosaic::Scope pointScope = Mosaic::scope( ui, Mosaic::Key( pointIndex ) );
                Mosaic::Scope row = this->PropertyRow( _caption );

                float * valueData = points[pointIndex]->buff();

                Mosaic::FloatSpan values( valueData, 2 );

                Mosaic::dragFloatVector( ui, _caption, values, unitOptions );
            }
        };

        auto uiEditorListBox = [this, ui, _node]( const Char * _caption, uint32_t & _prop, const InitializerListMosaicStringView & _items )
        {
            Mosaic::Key key = Detail::makeKey_( _caption );

            Mosaic::Scope scope = Mosaic::scope( ui, key );

            Mosaic::Scope row = this->PropertyRow( _caption );

            int32_t testValue = (int32_t)_prop;

            Mosaic::StringViewSpan items( _items.begin(), _items.size() );

            if( Mosaic::listBox( ui, _caption, &testValue, items ).changed() == false )
            {
                return;
            }

            uint32_t updated = static_cast<uint32_t>(testValue);

            if( updated == _prop )
            {
                return;
            }

            _prop = updated;
            _node->dirty = true;
        };

        auto uiReadOnlyListBox = [this, ui]( const Char * _caption, uint32_t _prop, const InitializerListMosaicStringView & _items )
        {
            Mosaic::Key key = Detail::makeKey_( _caption );

            Mosaic::Scope scope = Mosaic::scope( ui, key );
            Mosaic::Scope style = Mosaic::styleScope( ui, this->ReadOnlyTheme() );
            Mosaic::Scope disabled = Mosaic::disabledScope( ui, true );

            Mosaic::Scope row = this->PropertyRow( _caption );

            int32_t testValue = (int32_t)_prop;

            Mosaic::StringViewSpan items( _items.begin(), _items.size() );

            Mosaic::listBox( ui, _caption, &testValue, items );
        };

        {
            Mosaic::TreeScope header = Mosaic::collapsingHeader( ui, "Node:", true );

            if( header.expanded() == true )
            {
                uiReadOnlyString( "Name:", _node->name );
                uiReadOnlyString( "Type:", _node->type );

                Char uid_text[64 + 1] = {'\0'};
                MENGINE_SNPRINTF( uid_text, 64, "%u", _node->uid );

                uiReadOnlyString( "UID:", String( uid_text ) );

                Char hhash_text[64 + 1] = {'\0'};
                MENGINE_SNPRINTF( hhash_text, 64, "%" MENGINE_PRIu64, _node->hhash );

                uiReadOnlyString( "HHASH:", String( hhash_text ) );

                uiEditorBool( "Enable", _node->enable );
            }
        }

        {
            Mosaic::TreeScope header = Mosaic::collapsingHeader( ui, "Transformation:", true );

            if( header.expanded() == true )
            {
                NodeTransformation & transformation = _node->transformation;

                if( _node->transformationProxy == false )
                {
                    uiEditorVec3f( "Position", transformation.position );
                    uiEditorVec3f( "Origin", transformation.origin );
                    uiEditorVec2f( "Skew", transformation.skew );
                    uiEditorVec3f( "Scale", transformation.scale );
                    uiEditorVec3f( "Orientation", transformation.orientation );
                }

                uiReadOnlyVec3f( "World Position", transformation.worldPosition );
                uiReadOnlyVec3f( "World Scale", transformation.worldScale );
                uiReadOnlyVec3f( "World Orientation", transformation.worldOrientation );
            }
        }

        if( _node->hasRender == true )
        {
            Mosaic::TreeScope header = Mosaic::collapsingHeader( ui, "Render:", true );

            if( header.expanded() == true )
            {
                uiReadOnlyBool( "Enable", _node->render.enable );
                uiEditorBool( "Hide", _node->render.hide );

                if( Mosaic::button( ui, "Z Group Reset" ).clicked() == true )
                {
                    _node->render.z_group = MENGINE_RENDER_ZGROUP_DEFAULT;
                    _node->dirty = true;
                }

                uiEditorVec1I( "Z Order", _node->render.z_group );

                if( Mosaic::button( ui, "Z Index Reset" ).clicked() == true )
                {
                    _node->render.z_index = MENGINE_RENDER_ZINDEX_DEFAULT;
                    _node->dirty = true;
                }

                uiEditorVec1I( "Z Index", _node->render.z_index );
                uiReadOnlyVec1I( "Total Z Group", _node->render.total_z_group == MENGINE_RENDER_ZGROUP_DEFAULT ? 0 : _node->render.total_z_group );
                uiReadOnlyVec1I( "Total Z Index", _node->render.total_z_index == MENGINE_RENDER_ZINDEX_DEFAULT ? 0 : _node->render.total_z_index );
                uiEditorColor( "Local Color", _node->render.local_color );
                uiEditorColor( "Personal Color", _node->render.personal_color );

                if( _node->render.HasExtraRelationRender == true )
                {
                    Mosaic::TreeScope extra = Mosaic::collapsingHeader( ui, "Extra Relation Render:", true );

                    if( extra.expanded() == true )
                    {
                        uiReadOnlyString( "Base Node Name:", _node->name );
                        uiReadOnlyString( "Base Node Type:", _node->type );

                        Char uid_text[64 + 1] = {'\0'};
                        MENGINE_SNPRINTF( uid_text, 64, "%u", _node->uid );

                        uiReadOnlyString( "Base Node UID:", String( uid_text ) );
                    }
                }

                if( _node->render.camera.exist == true )
                {
                    Mosaic::TreeScope camera = Mosaic::collapsingHeader( ui, "Render Camera:", true );

                    if( camera.expanded() == true )
                    {
                        uiReadOnlyString( "Camera Name", _node->render.camera.Name );
                        uiReadOnlyString( "Camera Type", _node->render.camera.Type );

                        uiReadOnlyString( "Relelation Camera Name", _node->render.camera.RelationName );
                        uiReadOnlyString( "Relelation Camera Type", _node->render.camera.RelationType );
                    }
                }

                if( _node->render.viewport.exist == true )
                {
                    Mosaic::TreeScope viewport = Mosaic::collapsingHeader( ui, "Render Viewport:", true );

                    if( viewport.expanded() == true )
                    {
                        uiReadOnlyVec2f( "begin", _node->render.viewport.begin );
                        uiReadOnlyVec2f( "end", _node->render.viewport.end );
                    }
                }

                if( _node->render.scissor.exist == true )
                {
                    Mosaic::TreeScope scissor = Mosaic::collapsingHeader( ui, "Render Scissor:", true );

                    if( scissor.expanded() == true )
                    {
                        uiReadOnlyVec2f( "begin", _node->render.scissor.begin );
                        uiReadOnlyVec2f( "end", _node->render.scissor.end );
                    }
                }
            }
        }

        if( _node->hasAnimation == true )
        {
            Mosaic::TreeScope header = Mosaic::collapsingHeader( ui, "Animation:", true );

            if( header.expanded() == true )
            {
                uiEditorBool( "loop", _node->animation.loop );
                uiReadOnlyBool( "play", _node->animation.play );
                uiReadOnlyBool( "pause", _node->animation.pause );
                uiEditorVec1f( "time", _node->animation.time );
                uiReadOnlyVec1f( "duration", _node->animation.duration );
            }
        }

        if( _node->hasComponentSurface == true )
        {
            Mosaic::TreeScope header = Mosaic::collapsingHeader( ui, "Component Surface:", true );

            if( header.expanded() == true )
            {
                uiReadOnlyString( "name", _node->componentSurface.Name );
                uiReadOnlyString( "type", _node->componentSurface.Type );

                if( _node->componentSurface.Compile == true )
                {
                    uiReadOnlyVec2f( "max size", _node->componentSurface.MaxSize );
                    uiReadOnlyVec2f( "size", _node->componentSurface.Size );
                    uiReadOnlyVec2f( "offset", _node->componentSurface.Offset );
                }

                if( _node->componentSurface.hasAnimation == true )
                {
                    Mosaic::TreeScope animation = Mosaic::collapsingHeader( ui, "Surface Animation:", true );

                    if( animation.expanded() == true )
                    {
                        uiEditorBool( "loop", _node->componentSurface.animation.loop );
                        uiReadOnlyBool( "play", _node->componentSurface.animation.play );
                        uiReadOnlyBool( "pause", _node->componentSurface.animation.pause );
                        uiEditorVec1f( "time", _node->componentSurface.animation.time );
                        uiReadOnlyVec1f( "duration", _node->componentSurface.animation.duration );
                    }
                }

                if( _node->componentSurface.isTypeSurfaceImage == true )
                {
                    Mosaic::TreeScope image = Mosaic::collapsingHeader( ui, "Surface Image:", true );

                    if( image.expanded() == true )
                    {
                        uiReadOnlyString( "resource name", _node->componentSurface.surfaceImage.ResourceName );
                        uiReadOnlyString( "resource type", _node->componentSurface.surfaceImage.ResourceType );
                        uiReadOnlyUV4( "UV image", _node->componentSurface.surfaceImage.UVImage );

                        if( _node->componentSurface.surfaceImage.isContent == true )
                        {
                            Mosaic::TreeScope content = Mosaic::collapsingHeader( ui, "Surface Image Content:", true );

                            if( content.expanded() == true )
                            {
                                uiReadOnlyString( "file group", _node->componentSurface.surfaceImage.content.FileGroup );
                                uiReadOnlyString( "file path", _node->componentSurface.surfaceImage.content.FilePath );
                                uiReadOnlyString( "codec", _node->componentSurface.surfaceImage.content.CodecType );
                                uiReadOnlyString( "converter", _node->componentSurface.surfaceImage.content.ConverterType );
                            }
                        }
                    }
                }

                if( _node->componentSurface.hasAtlas == true )
                {
                    Mosaic::TreeScope atlas = Mosaic::collapsingHeader( ui, "Atlas:", true );

                    if( atlas.expanded() == true )
                    {
                        uiReadOnlyString( "resource name", _node->componentSurface.atlas.ResourceName );
                        uiReadOnlyString( "resource type", _node->componentSurface.atlas.ResourceType );

                        if( _node->componentSurface.atlas.isContent == true )
                        {
                            Mosaic::TreeScope content = Mosaic::collapsingHeader( ui, "Atlas Content:", true );

                            if( content.expanded() == true )
                            {
                                uiReadOnlyString( "file group", _node->componentSurface.atlas.content.FileGroup );
                                uiReadOnlyString( "file path", _node->componentSurface.atlas.content.FilePath );
                                uiReadOnlyString( "codec", _node->componentSurface.atlas.content.CodecType );
                                uiReadOnlyString( "converter", _node->componentSurface.atlas.content.ConverterType );
                            }
                        }
                    }
                }
            }
        }

        if( _node->isTypeTextField == true )
        {
            Mosaic::TreeScope header = Mosaic::collapsingHeader( ui, "TextField:", true );

            if( header.expanded() == true )
            {
                uiEditorBool( "Wrap", _node->textField.Wrap );

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

                {
                    Mosaic::Scope row = Mosaic::row( ui, Mosaic::Key( "MaxLength" ) );

                    uiEditorBoolHidden( "HasMaxLength", _node->textField.HasMaxLength );
                    uiEditorVec1f( "MaxLength", _node->textField.MaxLength );
                }

                uiEditorVec1f( "TotalMaxLength", _node->textField.TotalMaxLength );

                {
                    Mosaic::Scope row = Mosaic::row( ui, Mosaic::Key( "MaxHeight" ) );

                    uiEditorBoolHidden( "HasMaxHeight", _node->textField.HasMaxHeight );
                    uiEditorVec1f( "MaxHeight", _node->textField.MaxHeight );
                }

                uiEditorVec1f( "TotalMaxHeight", _node->textField.TotalMaxHeight );

                {
                    Mosaic::Scope row = Mosaic::row( ui, Mosaic::Key( "AutoScale" ) );

                    uiEditorBoolHidden( "HasAutoScale", _node->textField.HasAutoScale );
                    uiEditorBool( "AutoScale", _node->textField.AutoScale );
                }

                uiEditorBool( "TotalAutoScale", _node->textField.TotalAutoScale );

                {
                    Mosaic::Scope row = Mosaic::row( ui, Mosaic::Key( "Justify" ) );

                    uiEditorBoolHidden( "HasJustify", _node->textField.HasJustify );
                    uiEditorBool( "Justify", _node->textField.Justify );
                }

                uiEditorBool( "TotalJustify", _node->textField.TotalJustify );

                {
                    Mosaic::Scope row = Mosaic::row( ui, Mosaic::Key( "FontColor" ) );

                    uiEditorBoolHidden( "HasFontColor", _node->textField.HasFontColor );
                    uiEditorColor( "FontColor", _node->textField.FontColor, _node->textField.HasFontColor );
                }

                uiReadOnlyColor( "TotalFontColor", _node->textField.TotalFontColor );

                {
                    Mosaic::Scope row = Mosaic::row( ui, Mosaic::Key( "LineOffset" ) );

                    uiEditorBoolHidden( "HasLineOffset", _node->textField.HasLineOffset );
                    uiEditorVec1f( "LineOffset", _node->textField.LineOffset, _node->textField.HasLineOffset );
                }

                uiReadOnlyVec1f( "TotalLineOffset", _node->textField.TotalLineOffset );

                {
                    Mosaic::Scope row = Mosaic::row( ui, Mosaic::Key( "CharOffset" ) );

                    uiEditorBoolHidden( "HasCharOffset", _node->textField.HasCharOffset );
                    uiEditorVec1f( "CharOffset", _node->textField.CharOffset, _node->textField.HasCharOffset );
                }

                uiReadOnlyVec1f( "TotalCharOffset", _node->textField.TotalCharOffset );

                {
                    Mosaic::Scope row = Mosaic::row( ui, Mosaic::Key( "CharScale" ) );

                    uiEditorBoolHidden( "HasCharScale", _node->textField.HasCharScale );
                    uiEditorVec1f( "CharScale", _node->textField.CharScale, _node->textField.HasCharScale );
                }

                uiReadOnlyVec1f( "TotalCharScale", _node->textField.TotalCharScale );

                uiEditorListBox( "HorizontAlign", _node->textField.HorizontAlign, {"Left", "Center", "Right", "None"} );
                uiReadOnlyListBox( "TotalHorizontAlign", _node->textField.TotalHorizontAlign, {"Left", "Center", "Right", "None"} );
                uiEditorListBox( "VerticalAlign", _node->textField.VerticalAlign, {"Bottom", "Center", "Top", "None"} );
                uiReadOnlyListBox( "TotalVerticalAlign", _node->textField.TotalVerticalAlign, {"Bottom", "Center", "Top", "None"} );
                uiEditorVec1U( "MaxCharCount", _node->textField.MaxCharCount );
                uiEditorBool( "Pixelsnap", _node->textField.Pixelsnap );
            }
        }

        if( _node->isTypeMovie2 == true )
        {
            Mosaic::TreeScope header = Mosaic::collapsingHeader( ui, "Movie2:", true );

            if( header.expanded() == true )
            {
                uiReadOnlyString( "Composition Name", _node->movie2.CompositionName );
                uiEditorString( "AliasEnvironment", _node->movie2.TextAliasEnvironment );
            }
        }

        if( _node->isTypeSpine == true )
        {
            Mosaic::TreeScope header = Mosaic::collapsingHeader( ui, "Spine:", true );

            if( header.expanded() == true )
            {
                uiReadOnlyString( "Resource Name", _node->spine.ResourceName );
                uiReadOnlyString( "Resource Type", _node->spine.ResourceType );

                if( _node->spine.isContent == true )
                {
                    Mosaic::TreeScope content = Mosaic::collapsingHeader( ui, "Spine Resource Content:", true );

                    if( content.expanded() == true )
                    {
                        uiReadOnlyString( "file group", _node->spine.content.FileGroup );
                        uiReadOnlyString( "file path", _node->spine.content.FilePath );
                        uiReadOnlyString( "codec", _node->spine.content.CodecType );
                        uiReadOnlyString( "converter", _node->spine.content.ConverterType );
                    }
                }
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
    void NodeDebuggerApp::OnMuteButton()
    {
        m_muteRequested = true;
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
    void NodeDebuggerApp::SendMuteRequest()
    {
        this->SendGameControlCommand( "mute" );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::SendResolutionRequest( uint32_t _width, uint32_t _height )
    {
        pugi::xml_document doc;

        pugi::xml_node packetNode = doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "Resolutions" );

        pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

        pugi::xml_node xmlNode = payloadNode.append_child( "Resolution" );

        xmlNode.append_attribute( "width" ).set_value( _width );
        xmlNode.append_attribute( "height" ).set_value( _height );

        this->SendXML( doc );
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerApp::SendSetting( const String & _setting, const String & _key, const Char * _value )
    {
        pugi::xml_document doc;

        pugi::xml_node packetNode = doc.append_child( "Packet" );
        packetNode.append_attribute( "type" ).set_value( "Settings" );

        pugi::xml_node payloadNode = packetNode.append_child( "Payload" );

        pugi::xml_node xmlNode = payloadNode.append_child( "Setting" );

        xmlNode.append_attribute( "name" ).set_value( _setting.c_str() );
        xmlNode.append_attribute( "key" ).set_value( _key.c_str() );
        xmlNode.append_attribute( "value" ).set_value( _value );

        this->SendXML( doc );
    }
    //////////////////////////////////////////////////////////////////////////
}
