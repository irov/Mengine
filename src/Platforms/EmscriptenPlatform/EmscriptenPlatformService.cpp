#include "EmscriptenPlatformService.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/RenderServiceInterface.h"

#include "EmscriptenPlatformBridge.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/InputServiceHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/StringLength.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/VocabularyHelper.h"

#include "Config/StdIO.h"
#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PlatformService, Mengine::EmscriptenPlatformService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        struct KeyCodeDesc
        {
            const Char * code;
            EKeyCode key;
        };
        //////////////////////////////////////////////////////////////////////////
        static const KeyCodeDesc KEY_CODES[] = {
            {"Escape", KC_ESCAPE},
            {"Digit0", KC_0},
            {"Digit1", KC_1},
            {"Digit2", KC_2},
            {"Digit3", KC_3},
            {"Digit4", KC_4},
            {"Digit5", KC_5},
            {"Digit6", KC_6},
            {"Digit7", KC_7},
            {"Digit8", KC_8},
            {"Digit9", KC_9},
            {"Minus", KC_OEM_MINUS},
            {"Equal", KC_OEM_PLUS},
            {"Backspace", KC_BACK},
            {"Tab", KC_TAB},
            {"KeyA", KC_A},
            {"KeyB", KC_B},
            {"KeyC", KC_C},
            {"KeyD", KC_D},
            {"KeyE", KC_E},
            {"KeyF", KC_F},
            {"KeyG", KC_G},
            {"KeyH", KC_H},
            {"KeyI", KC_I},
            {"KeyJ", KC_J},
            {"KeyK", KC_K},
            {"KeyL", KC_L},
            {"KeyM", KC_M},
            {"KeyN", KC_N},
            {"KeyO", KC_O},
            {"KeyP", KC_P},
            {"KeyQ", KC_Q},
            {"KeyR", KC_R},
            {"KeyS", KC_S},
            {"KeyT", KC_T},
            {"KeyU", KC_U},
            {"KeyV", KC_V},
            {"KeyW", KC_W},
            {"KeyX", KC_X},
            {"KeyY", KC_Y},
            {"KeyZ", KC_Z},
            {"BracketLeft", KC_OEM_4},
            {"BracketRight", KC_OEM_6},
            {"Enter", KC_RETURN},
            {"ControlLeft", KC_LCONTROL},
            {"ControlRight", KC_RCONTROL},
            {"Semicolon", KC_OEM_1},
            {"Quote", KC_OEM_7},
            {"Backquote", KC_OEM_3},
            {"ShiftLeft", KC_LSHIFT},
            {"ShiftRight", KC_RSHIFT},
            {"Backslash", KC_OEM_5},
            {"IntlBackslash", KC_OEM_102},
            {"Comma", KC_OEM_COMMA},
            {"Period", KC_OEM_PERIOD},
            {"Slash", KC_OEM_2},
            {"AltLeft", KC_LMENU},
            {"AltRight", KC_RMENU},
            {"MetaLeft", KC_LWIN},
            {"MetaRight", KC_RWIN},
            {"ContextMenu", KC_APPS},
            {"Space", KC_SPACE},
            {"CapsLock", KC_CAPITAL},
            {"F1", KC_F1},
            {"F2", KC_F2},
            {"F3", KC_F3},
            {"F4", KC_F4},
            {"F5", KC_F5},
            {"F6", KC_F6},
            {"F7", KC_F7},
            {"F8", KC_F8},
            {"F9", KC_F9},
            {"F10", KC_F10},
            {"F11", KC_F11},
            {"F12", KC_F12},
            {"NumLock", KC_NUMLOCK},
            {"ScrollLock", KC_SCROLL},
            {"Pause", KC_PAUSE},
            {"PrintScreen", KC_SNAPSHOT},
            {"Numpad0", KC_NUMPAD0},
            {"Numpad1", KC_NUMPAD1},
            {"Numpad2", KC_NUMPAD2},
            {"Numpad3", KC_NUMPAD3},
            {"Numpad4", KC_NUMPAD4},
            {"Numpad5", KC_NUMPAD5},
            {"Numpad6", KC_NUMPAD6},
            {"Numpad7", KC_NUMPAD7},
            {"Numpad8", KC_NUMPAD8},
            {"Numpad9", KC_NUMPAD9},
            {"NumpadMultiply", KC_MULTIPLY},
            {"NumpadAdd", KC_ADD},
            {"NumpadSubtract", KC_SUBTRACT},
            {"NumpadDecimal", KC_DECIMAL},
            {"NumpadDivide", KC_DIVIDE},
            {"NumpadEnter", KC_RETURN},
            {"Insert", KC_INSERT},
            {"Delete", KC_DELETE},
            {"Home", KC_HOME},
            {"End", KC_END},
            {"PageUp", KC_PRIOR},
            {"PageDown", KC_NEXT},
            {"ArrowLeft", KC_LEFT},
            {"ArrowUp", KC_UP},
            {"ArrowRight", KC_RIGHT},
            {"ArrowDown", KC_DOWN},
        };
        //////////////////////////////////////////////////////////////////////////
        static EKeyCode getKeyCode( const Char * _code )
        {
            for( const KeyCodeDesc & desc : KEY_CODES )
            {
                if( StdString::strcmp( desc.code, _code ) != 0 )
                {
                    continue;
                }

                return desc.key;
            }

            return KC_UNASSIGNED;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    EmscriptenPlatformService::EmscriptenPlatformService()
        : m_context( 0 )
        , m_ratio( 1.0 )
        , m_beginTime( 0.0 )
        , m_running( true )
        , m_active( true )
        , m_focused( true )
        , m_hidden( false )
        , m_contextLost( false )
        , m_resetFrame( true )
        , m_started( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EmscriptenPlatformService::~EmscriptenPlatformService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderPlatformInterface * EmscriptenPlatformService::getRenderPlatform()
    {
        return this;
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp EmscriptenPlatformService::getPlatfomTime() const
    {
        double now = emscripten_get_now();

        return (Timestamp)(now - m_beginTime);
    }
    //////////////////////////////////////////////////////////////////////////
    Timestamp EmscriptenPlatformService::getInputTimestamp() const
    {
        Timestamp timestamp = Helper::getPlatformTimestamp();

        return timestamp;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::setProjectTitle( const Char * _projectTitle )
    {
        m_title = _projectTitle;

        emscripten_set_window_title( _projectTitle );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t EmscriptenPlatformService::getProjectTitle( Char * const _projectTitle ) const
    {
        const Char * title = m_title.c_str();
        StdString::strcpy_safe( _projectTitle, title, MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME );

        size_t size = m_title.size();

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & EmscriptenPlatformService::getPlatformTags() const
    {
        return m_tags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::hasPlatformTag( const ConstString & _tag ) const
    {
        if( m_tags.hasTag( _tag ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::isDesktop() const
    {
        if( mengineIsCoarsePointer() == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::hasTouchpad() const
    {
        if( mengineHasTouchPoints() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::getDesktopResolution( Resolution * const _resolution ) const
    {
        double width = 0.0;
        double height = 0.0;
        emscripten_get_element_css_size( "#canvas", &width, &height );

        double ratio = emscripten_get_device_pixel_ratio();

        double scaledWidth = width * ratio;
        double scaledHeight = height * ratio;

        uint32_t pixelWidth = (uint32_t)MENGINE_MAX( 1.0, scaledWidth );
        uint32_t pixelHeight = (uint32_t)MENGINE_MAX( 1.0, scaledHeight );

        _resolution->setWidth( pixelWidth );
        _resolution->setHeight( pixelHeight );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::getMaxClientResolution( Resolution * const _resolution ) const
    {
        if( this->getDesktopResolution( _resolution ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::getNoFullscreen() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::getAlwaysFullscreen() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::getSafeAreaViewport( Viewport * const _viewport ) const
    {
        float width = m_resolution.getWidthF();
        float height = m_resolution.getHeightF();

        _viewport->begin.set( 0.f, 0.f );
        _viewport->end.set( width, height );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::setSafeAreaViewportChangedCallback( const LambdaSafeAreaViewportChanged & _callback )
    {
        m_safeAreaCallback = _callback;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::getDisplayCutoutViewport( Viewport * const _viewport ) const
    {
        MENGINE_UNUSED( _viewport );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t EmscriptenPlatformService::getCurrentPath( Char * const _filePath ) const
    {
        const Char * path = "/app/";

        StdString::strcpy_safe( _filePath, path, MENGINE_MAX_PATH );

        size_t length = Helper::stringLength( path );

        return length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t EmscriptenPlatformService::getUserPath( Char * const _filePath ) const
    {
        const Char * path = "/user/";

        StdString::strcpy_safe( _filePath, path, MENGINE_MAX_PATH );

        size_t length = Helper::stringLength( path );

        return length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t EmscriptenPlatformService::getTemporaryPath( Char * const _filePath ) const
    {
        const Char * path = "/tmp/";

        StdString::strcpy_safe( _filePath, path, MENGINE_MAX_PATH );

        size_t length = Helper::stringLength( path );

        return length;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t EmscriptenPlatformService::getExtraPreferencesFolderName( Char * const _folderName ) const
    {
        _folderName[0] = '\0';

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::setCursorPosition( const mt::vec2f & _cursorPosition )
    {
        MENGINE_UNUSED( _cursorPosition );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::setCursorCapture( bool _capture )
    {
        MENGINE_UNUSED( _capture );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::setCursorIcon( const ConstString & _icon )
    {
        MENGINE_UNUSED( _icon );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::hasCursorIcon( const ConstString & _icon ) const
    {
        MENGINE_UNUSED( _icon );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t EmscriptenPlatformService::getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const
    {
        MENGINE_UNUSED( _groupName );
        MENGINE_UNUSED( _fontName );

        _fontPath[0] = '\0';

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::setClipboardText( const Char * _value ) const
    {
        if( mengineWriteClipboard( _value ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::getClipboardText( Char * const _value, size_t _capacity ) const
    {
        MENGINE_UNUSED( _value );
        MENGINE_UNUSED( _capacity );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::_initializeService()
    {
        m_beginTime = emscripten_get_now();

        const Char * optionPlatforms[MENGINE_OPTIONS_VALUES_MAX];
        uint32_t optionPlatformsCount = 0;
        if( OPTIONS_SERVICE()
            ->getOptionValues( "platform", optionPlatforms, &optionPlatformsCount ) == true )
        {
            for( uint32_t index = 0; index != optionPlatformsCount; ++index )
            {
                ConstString tag = Helper::stringizeString( optionPlatforms[index] );
                m_tags.addTag( tag );
            }
        }
        else
        {
            m_tags.addTag( STRINGIZE_STRING_LOCAL( "EMSCRIPTEN" ) );

            if( this->isDesktop() == true )
            {
                m_tags.addTag( STRINGIZE_STRING_LOCAL( "DESKTOP" ) );
            }
            else
            {
                m_tags.addTag( STRINGIZE_STRING_LOCAL( "MOBILE" ) );
            }
        }

        for( int32_t & identifier : m_touchIds )
        {
            identifier = -1;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::_finalizeService()
    {
        emscripten_html5_remove_all_event_listeners();
        m_safeAreaCallback = nullptr;
        m_tags.clear();

        if( m_context == 0 )
        {
            return;
        }

        emscripten_webgl_make_context_current( 0 );
        emscripten_webgl_destroy_context( m_context );
        m_context = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::initializeFileService()
    {
        const FilePath & rootPath = FilePath::none();
        FileGroupInterfacePtr defaultFileGroup = nullptr;
        if( FILE_SERVICE()
            ->mountFileGroup( ConstString::none(), nullptr, nullptr, rootPath, STRINGIZE_STRING_LOCAL( "dir" ), &defaultFileGroup, false, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "failed to mount application directory" );

            return false;
        }

        VOCABULARY_SET( FileGroupInterface, STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none(), defaultFileGroup, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        FileGroupInterfacePtr developmentFileGroup = nullptr;
        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ), nullptr, nullptr, rootPath, STRINGIZE_STRING_LOCAL( "dir" ), &developmentFileGroup, false, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "failed to mount development directory" );

            return false;
        }

        VOCABULARY_SET( FileGroupInterface, STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ), developmentFileGroup, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::finalizeFileService()
    {
#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        FILE_SERVICE()
            ->unmountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ) );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "dev" ) );
#endif

        FILE_SERVICE()
            ->unmountFileGroup( ConstString::none() );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::runPlatform()
    {
        EmscriptenVisibilityChangeEvent visibility;
        if( emscripten_get_visibility_status( &visibility ) == EMSCRIPTEN_RESULT_SUCCESS )
        {
            m_hidden = visibility.hidden;
        }

        m_focused = mengineDocumentHasFocus();

        this->setActive_();

        m_started = true;

        this->resize_();

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_RUN );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::loopPlatform()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::updatePlatform()
    {
        if( m_running == false )
        {
            return false;
        }

        if( m_context == 0 )
        {
            return true;
        }

        if( emscripten_is_webgl_context_lost( m_context ) == true )
        {
            EmscriptenPlatformService::onContext_( EMSCRIPTEN_EVENT_WEBGLCONTEXTLOST, nullptr, this );

            return true;
        }

        this->resize_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::tickPlatform( float _frameTime )
    {
        if( m_active == false )
        {
            m_resetFrame = true;

            return;
        }

        if( m_contextLost == true )
        {
            m_resetFrame = true;

            return;
        }

        if( m_resetFrame == true )
        {
            _frameTime = 0.f;
            m_resetFrame = false;
        }

        bool updating = APPLICATION_SERVICE()
            ->beginUpdate();

        if( updating == true )
        {
            APPLICATION_SERVICE()
                ->tick( _frameTime );
        }

        APPLICATION_SERVICE()
            ->endUpdate();
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::stopPlatform()
    {
        m_running = false;

        if( m_started == false )
        {
            return;
        }

        m_started = false;

        NOTIFICATION_NOTIFY( NOTIFICATOR_PLATFORM_STOP );
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::alreadyRunningMonitor()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::createWindow( const Resolution & _resolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _fullscreen );

        uint32_t width = _resolution.getWidth();
        uint32_t height = _resolution.getHeight();
        emscripten_set_canvas_element_size( "#canvas", width, height );

        EmscriptenWebGLContextAttributes attributes;
        emscripten_webgl_init_context_attributes( &attributes );

        attributes.minorVersion = 0;
        attributes.alpha = false;
        attributes.depth = true;
        attributes.stencil = true;
        attributes.antialias = false;
        attributes.enableExtensionsByDefault = true;

        EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context = 0;

        for( int32_t version = MENGINE_EMSCRIPTEN_MAX_WEBGL_VERSION; version >= MENGINE_EMSCRIPTEN_MIN_WEBGL_VERSION; --version )
        {
            attributes.majorVersion = version;
            context = emscripten_webgl_create_context( "#canvas", &attributes );

            if( context <= 0 )
            {
                continue;
            }

            break;
        }

        if( context <= 0 )
        {
            LOGGER_ERROR( "invalid create WebGL context [%d]"
                , (int32_t)context
            );

            return false;
        }

        if( emscripten_webgl_make_context_current( context ) != EMSCRIPTEN_RESULT_SUCCESS )
        {
            LOGGER_ERROR( "invalid make WebGL context current" );

            emscripten_webgl_destroy_context( context );

            return false;
        }

        m_context = context;
        m_resolution = _resolution;

        mengineSetupCanvas();

        emscripten_set_webglcontextlost_callback( "#canvas", this, false, &EmscriptenPlatformService::onContext_ );
        emscripten_set_webglcontextrestored_callback( "#canvas", this, false, &EmscriptenPlatformService::onContext_ );

        emscripten_set_mousedown_callback( "#canvas", this, false, &EmscriptenPlatformService::onMouse_ );
        emscripten_set_mouseup_callback( EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, &EmscriptenPlatformService::onMouse_ );
        emscripten_set_mousemove_callback( EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, &EmscriptenPlatformService::onMouse_ );
        emscripten_set_mouseenter_callback( "#canvas", this, false, &EmscriptenPlatformService::onMouse_ );
        emscripten_set_mouseleave_callback( "#canvas", this, false, &EmscriptenPlatformService::onMouse_ );

        emscripten_set_keydown_callback( "#canvas", this, false, &EmscriptenPlatformService::onKey_ );
        emscripten_set_keyup_callback( "#canvas", this, false, &EmscriptenPlatformService::onKey_ );

        emscripten_set_wheel_callback( "#canvas", this, false, &EmscriptenPlatformService::onWheel_ );

        emscripten_set_touchstart_callback( "#canvas", this, false, &EmscriptenPlatformService::onTouch_ );
        emscripten_set_touchend_callback( "#canvas", this, false, &EmscriptenPlatformService::onTouch_ );
        emscripten_set_touchmove_callback( "#canvas", this, false, &EmscriptenPlatformService::onTouch_ );
        emscripten_set_touchcancel_callback( "#canvas", this, false, &EmscriptenPlatformService::onTouch_ );

        emscripten_set_focus_callback( EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, &EmscriptenPlatformService::onFocus_ );
        emscripten_set_blur_callback( EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, &EmscriptenPlatformService::onFocus_ );
        emscripten_set_blur_callback( "#canvas", this, false, &EmscriptenPlatformService::onCanvasBlur_ );

        emscripten_set_visibilitychange_callback( this, false, &EmscriptenPlatformService::onVisibility_ );

        emscripten_set_resize_callback( EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, &EmscriptenPlatformService::onResize_ );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::closeWindow()
    {
        m_running = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::minimizeWindow()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t EmscriptenPlatformService::dpToWidthPx( int32_t _dp ) const
    {
        return (int32_t)(_dp * m_ratio);
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t EmscriptenPlatformService::dpToHeightPx( int32_t _dp ) const
    {
        return (int32_t)(_dp * m_ratio);
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t EmscriptenPlatformService::pxToWidthDp( int32_t _px ) const
    {
        return (int32_t)(_px / m_ratio);
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t EmscriptenPlatformService::pxToHeightDp( int32_t _px ) const
    {
        return (int32_t)(_px / m_ratio);
    }
    //////////////////////////////////////////////////////////////////////////
    DynamicLibraryInterfacePtr EmscriptenPlatformService::loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _dynamicLibraryName );
        MENGINE_UNUSED( _doc );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::createProcess( const Char * _executable, const ArgumentStrings & _arguments, uint32_t * const _exitCode ) const
    {
        MENGINE_UNUSED( _executable );
        MENGINE_UNUSED( _arguments );
        MENGINE_UNUSED( _exitCode );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::isDebuggerPresent() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::debugBreak()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::showKeyboard()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::hideKeyboard()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::isShowKeyboard() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _resolution );
        MENGINE_UNUSED( _fullscreen );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::notifyCursorModeChanged( bool _mode )
    {
        MENGINE_UNUSED( _mode );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::notifyCursorIconSetup( const ConstString & _name, const ContentInterfacePtr & _content, const MemoryInterfacePtr & _memory )
    {
        MENGINE_UNUSED( _name );
        MENGINE_UNUSED( _content );
        MENGINE_UNUSED( _memory );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::onEvent( const ConstString & _event, const Params & _params )
    {
        MENGINE_UNUSED( _event );
        MENGINE_UNUSED( _params );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::openUrlInDefaultBrowser( const Char * _url )
    {
        if( mengineOpenUrl( _url ) == false )
        {
            LOGGER_ERROR( "failed to open URL '%s'"
                , _url
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::openMail( const Char * _email, const Char * _subject, const Char * _body, const Char * _technically )
    {
        MENGINE_UNUSED( _email );
        MENGINE_UNUSED( _subject );
        MENGINE_UNUSED( _body );
        MENGINE_UNUSED( _technically );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::openDeleteAccount( const LambdaDeleteAccountAccepted & _accepted, const LambdaDeleteAccountCanceled & _canceled )
    {
        MENGINE_UNUSED( _accepted );
        MENGINE_UNUSED( _canceled );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::completeDeleteAccount( EDeleteAccountResult _result )
    {
        MENGINE_UNUSED( _result );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::isNetworkAvailable() const
    {
        if( mengineIsOnline() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _data );
        MENGINE_UNUSED( _size );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size )
    {
        MENGINE_UNUSED( _directoryPath );
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _data );
        MENGINE_UNUSED( _size );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::messageBox( const Char * _caption, const Char * _format, ... ) const
    {
        constexpr size_t messageCapacity = 4096;

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        Char message[messageCapacity] = {'\0'};
        MENGINE_VSNPRINTF( message, messageCapacity, _format, args );
        MENGINE_VA_LIST_END( args );

        LOGGER_ERROR( "%s: %s"
            , _caption
            , message
        );
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::showSystemDialog( const Char * _title, const Char * _message, const LambdaSystemDialog & _callback )
    {
        MENGINE_UNUSED( _title );
        MENGINE_UNUSED( _message );
        MENGINE_UNUSED( _callback );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::renderPlatform()
    {
        if( m_active == false )
        {
            return false;
        }

        if( m_contextLost == true )
        {
            return false;
        }

        if( APPLICATION_SERVICE()
            ->render() == false )
        {
            return false;
        }

        APPLICATION_SERVICE()
            ->flush();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::notifyVsyncChanged( bool _vsync )
    {
        MENGINE_UNUSED( _vsync );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::setActive_()
    {
        bool nopause = APPLICATION_SERVICE()
            ->getNopause();

        bool focused = m_focused == true || nopause == true;
        bool active = focused == true && m_hidden == false;

        if( m_active == active )
        {
            return;
        }

        if( active == false )
        {
            this->releaseInput_();
        }

        m_active = active;
        m_resetFrame = true;

        Timestamp timestamp = this->getInputTimestamp();

        const mt::vec2f & point = INPUT_SERVICE()
            ->getCursorPosition( TC_TOUCH0 );

        if( active == false )
        {
            Helper::pushMouseLeaveEvent( timestamp, TC_TOUCH0, point.x, point.y, 0.f );
        }
        else
        {
            Helper::pushMouseEnterEvent( timestamp, TC_TOUCH0, point.x, point.y, 0.f );
        }

        APPLICATION_SERVICE()
            ->setFocus( active );

        INPUT_SERVICE()
            ->onFocus( active );

        APPLICATION_SERVICE()
            ->turnSound( active );
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::releaseInput_()
    {
        Timestamp timestamp = this->getInputTimestamp();

        const mt::vec2f & point = INPUT_SERVICE()
            ->getCursorPosition( TC_TOUCH0 );

        for( uint32_t index = 0; index != MENGINE_INPUT_MAX_KEY_CODE; ++index )
        {
            if( m_keyDown[index] == false )
            {
                continue;
            }

            m_keyDown[index] = false;

            EKeyCode key = (EKeyCode)index;
            Helper::pushKeyEvent( timestamp, point.x, point.y, 0.f, key, false, false );
        }

        for( uint32_t index = 0; index != MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE; ++index )
        {
            if( m_mouseButtonDown[index] == false )
            {
                continue;
            }

            m_mouseButtonDown[index] = false;

            EMouseButtonCode button = (EMouseButtonCode)index;
            Helper::pushMouseButtonEvent( timestamp, TC_TOUCH0, point.x, point.y, button, 0.f, false );
        }

        for( uint32_t slot = 0; slot != MENGINE_INPUT_MAX_TOUCH; ++slot )
        {
            if( m_touchIds[slot] == -1 )
            {
                continue;
            }

            m_touchIds[slot] = -1;

            const mt::vec2f & touchPoint = m_touchPositions[slot];

            ETouchCode code = (ETouchCode)slot;
            Helper::pushMouseButtonEvent( timestamp, code, touchPoint.x, touchPoint.y, MC_LBUTTON, 0.f, false );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::resize_()
    {
        if( m_context == 0 )
        {
            return;
        }

        if( m_started == false )
        {
            return;
        }

        if( m_contextLost == true )
        {
            return;
        }

        if( emscripten_is_webgl_context_lost( m_context ) == true )
        {
            return;
        }

        Resolution resolution;
        this->getDesktopResolution( &resolution );

        m_ratio = emscripten_get_device_pixel_ratio();

        if( m_resolution == resolution )
        {
            return;
        }

        m_resolution = resolution;

        uint32_t width = resolution.getWidth();
        uint32_t height = resolution.getHeight();
        emscripten_set_canvas_element_size( "#canvas", width, height );

        APPLICATION_SERVICE()
            ->setWindowResolution( resolution );

        if( m_safeAreaCallback == nullptr )
        {
            return;
        }

        Viewport viewport;
        this->getSafeAreaViewport( &viewport );

        m_safeAreaCallback( viewport );
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenPlatformService::getPointer_( double _clientX, double _clientY, mt::vec2f * const _point ) const
    {
        double left = 0.0;
        double top = 0.0;
        mengineGetCanvasOrigin( &left, &top );

        double width = 0.0;
        double height = 0.0;
        emscripten_get_element_css_size( "#canvas", &width, &height );

        double canvasWidth = MENGINE_MAX( width, 1.0 );
        double canvasHeight = MENGINE_MAX( height, 1.0 );

        float x = (float)((_clientX - left) / canvasWidth);
        float y = (float)((_clientY - top) / canvasHeight);

        _point->set( x, y );
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::onMouse_( int _type, const EmscriptenMouseEvent * _event, void * _user )
    {
        EmscriptenPlatformService * platform = static_cast<EmscriptenPlatformService *>(_user);

        mt::vec2f point;
        platform->getPointer_( _event->clientX, _event->clientY, &point );

        Timestamp timestamp = platform->getInputTimestamp();

        if( _type == EMSCRIPTEN_EVENT_MOUSEENTER )
        {
            Helper::pushMouseEnterEvent( timestamp, TC_TOUCH0, point.x, point.y, 0.f );

            return false;
        }

        if( _type == EMSCRIPTEN_EVENT_MOUSELEAVE )
        {
            Helper::pushMouseLeaveEvent( timestamp, TC_TOUCH0, point.x, point.y, 0.f );

            return false;
        }

        if( _type == EMSCRIPTEN_EVENT_MOUSEMOVE )
        {
            bool inside = point.x >= 0.f && point.x <= 1.f && point.y >= 0.f && point.y <= 1.f;

            if( inside == false )
            {
                if( _event->buttons == 0 )
                {
                    return false;
                }
            }

            const mt::vec2f & previous = INPUT_SERVICE()
                ->getCursorPosition( TC_TOUCH0 );

            float dx = point.x - previous.x;
            float dy = point.y - previous.y;

            Helper::pushMouseMoveEvent( timestamp, TC_TOUCH0, point.x, point.y, dx, dy, 0.f, 0.f );

            return false;
        }

        if( _event->button >= MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE )
        {
            return false;
        }

        bool down = _type == EMSCRIPTEN_EVENT_MOUSEDOWN;

        if( down == false )
        {
            if( platform->m_mouseButtonDown[_event->button] == false )
            {
                return false;
            }
        }

        if( down == true )
        {
            mengineFocusCanvas();
        }

        platform->m_mouseButtonDown[_event->button] = down;

        EMouseButtonCode button = (EMouseButtonCode)_event->button;
        Helper::pushMouseButtonEvent( timestamp, TC_TOUCH0, point.x, point.y, button, 1.f, down );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::onKey_( int _type, const EmscriptenKeyboardEvent * _event, void * _user )
    {
        EmscriptenPlatformService * platform = static_cast<EmscriptenPlatformService *>(_user);

        Timestamp timestamp = platform->getInputTimestamp();

        const mt::vec2f & point = INPUT_SERVICE()
            ->getCursorPosition( TC_TOUCH0 );

        bool down = _type == EMSCRIPTEN_EVENT_KEYDOWN;

        EKeyCode key = Detail::getKeyCode( _event->code );

        if( key != KC_UNASSIGNED )
        {
            platform->m_keyDown[key] = down;

            Helper::pushKeyEvent( timestamp, point.x, point.y, 0.f, key, down, _event->repeat );
        }

        if( _event->ctrlKey == true )
        {
            return false;
        }

        if( _event->metaKey == true )
        {
            return false;
        }

        if( down == false )
        {
            return true;
        }

        if( _event->altKey == true )
        {
            return true;
        }

        size_t size = StdString::strlen( _event->key );

        WChar text[MENGINE_INPUTTEXTEVENT_TEXT_MAX_SIZE + 1] = {L'\0'};
        size_t count = 0;
        if( Helper::utf8ToUnicode( _event->key, size, text, MENGINE_INPUTTEXTEVENT_TEXT_MAX_SIZE, &count ) == false )
        {
            return true;
        }

        if( count != 1 )
        {
            return true;
        }

        Helper::pushTextEvent( timestamp, point.x, point.y, 0.f, text );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::onWheel_( int _type, const EmscriptenWheelEvent * _event, void * _user )
    {
        MENGINE_UNUSED( _type );

        EmscriptenPlatformService * platform = static_cast<EmscriptenPlatformService *>(_user);

        if( _event->deltaY == 0.0 )
        {
            return false;
        }

        mt::vec2f point;
        platform->getPointer_( _event->mouse.clientX, _event->mouse.clientY, &point );

        Timestamp timestamp = platform->getInputTimestamp();

        int32_t scroll = _event->deltaY > 0.0 ? -1 : 1;
        Helper::pushMouseWheelEvent( timestamp, point.x, point.y, 0.f, WC_CENTRAL, scroll );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::onTouch_( int _type, const EmscriptenTouchEvent * _event, void * _user )
    {
        EmscriptenPlatformService * platform = static_cast<EmscriptenPlatformService *>(_user);

        Timestamp timestamp = platform->getInputTimestamp();

        for( int32_t index = 0; index != _event->numTouches; ++index )
        {
            const EmscriptenTouchPoint & touch = _event->touches[index];

            if( touch.isChanged == false )
            {
                continue;
            }

            uint32_t slot = MENGINE_INPUT_MAX_TOUCH;

            for( uint32_t candidate = 0; candidate != MENGINE_INPUT_MAX_TOUCH; ++candidate )
            {
                if( platform->m_touchIds[candidate] != touch.identifier )
                {
                    continue;
                }

                slot = candidate;

                break;
            }

            if( slot == MENGINE_INPUT_MAX_TOUCH )
            {
                if( _type != EMSCRIPTEN_EVENT_TOUCHSTART )
                {
                    continue;
                }

                for( uint32_t candidate = 0; candidate != MENGINE_INPUT_MAX_TOUCH; ++candidate )
                {
                    if( platform->m_touchIds[candidate] != -1 )
                    {
                        continue;
                    }

                    slot = candidate;
                    platform->m_touchIds[slot] = touch.identifier;

                    break;
                }
            }

            if( slot == MENGINE_INPUT_MAX_TOUCH )
            {
                continue;
            }

            mt::vec2f point;
            platform->getPointer_( touch.clientX, touch.clientY, &point );

            ETouchCode code = (ETouchCode)slot;

            if( _type == EMSCRIPTEN_EVENT_TOUCHMOVE )
            {
                const mt::vec2f & previous = platform->m_touchPositions[slot];

                float dx = point.x - previous.x;
                float dy = point.y - previous.y;

                Helper::pushMouseMoveEvent( timestamp, code, point.x, point.y, dx, dy, 1.f, 0.f );
            }
            else
            {
                bool down = _type == EMSCRIPTEN_EVENT_TOUCHSTART;

                Helper::pushMouseButtonEvent( timestamp, code, point.x, point.y, MC_LBUTTON, 1.f, down );

                if( down == false )
                {
                    platform->m_touchIds[slot] = -1;
                }
            }

            platform->m_touchPositions[slot] = point;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::onFocus_( int _type, const EmscriptenFocusEvent * _event, void * _user )
    {
        MENGINE_UNUSED( _event );

        EmscriptenPlatformService * platform = static_cast<EmscriptenPlatformService *>(_user);

        platform->m_focused = _type == EMSCRIPTEN_EVENT_FOCUS;

        platform->setActive_();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::onCanvasBlur_( int _type, const EmscriptenFocusEvent * _event, void * _user )
    {
        MENGINE_UNUSED( _type );
        MENGINE_UNUSED( _event );

        EmscriptenPlatformService * platform = static_cast<EmscriptenPlatformService *>(_user);

        platform->releaseInput_();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::onVisibility_( int _type, const EmscriptenVisibilityChangeEvent * _event, void * _user )
    {
        MENGINE_UNUSED( _type );

        EmscriptenPlatformService * platform = static_cast<EmscriptenPlatformService *>(_user);

        platform->m_hidden = _event->hidden;

        platform->setActive_();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::onResize_( int _type, const EmscriptenUiEvent * _event, void * _user )
    {
        MENGINE_UNUSED( _type );
        MENGINE_UNUSED( _event );

        EmscriptenPlatformService * platform = static_cast<EmscriptenPlatformService *>(_user);

        platform->resize_();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenPlatformService::onContext_( int _type, const void * _event, void * _user )
    {
        MENGINE_UNUSED( _event );

        EmscriptenPlatformService * platform = static_cast<EmscriptenPlatformService *>(_user);

        platform->m_resetFrame = true;

        if( _type == EMSCRIPTEN_EVENT_WEBGLCONTEXTLOST )
        {
            if( platform->m_contextLost == true )
            {
                return true;
            }

            platform->m_contextLost = true;

            RENDER_SERVICE()
                ->onDeviceLostPrepare();

            return true;
        }

        if( platform->m_contextLost == false )
        {
            return false;
        }

        emscripten_webgl_make_context_current( platform->m_context );

        if( RENDER_SERVICE()
            ->onDeviceLostRestore() == false )
        {
            LOGGER_ERROR( "invalid restore WebGL device" );

            platform->closeWindow();

            return false;
        }

        platform->m_contextLost = false;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
