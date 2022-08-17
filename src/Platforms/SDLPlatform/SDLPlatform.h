#pragma once

#include "Interface/ApplicationInterface.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/CodecInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/PluginInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/ModuleInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/SDLPlatformExtensionInterface.h"
#include "Interface/FactoryInterface.h"

#if defined(MENGINE_PLATFORM_ANDROID)
#   include "Interface/AndroidPlatformExtensionInterface.h"
#endif

#include "Environment/SDL2/SDL2Includer.h"

#if defined(MENGINE_PLATFORM_ANDROID)
#   include "Environment/Android/AndroidIncluder.h"
#   include "Environment/Android/AndroidUtils.h"
#   include "Environment/Android/AndroidEventationHub.h"
#endif

#include "SDLInput.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/SHA1.h"

#include "Config/Time.h"

namespace Mengine
{
    class SDLPlatform
        : public ServiceBase<PlatformInterface>
        , public SDLPlatformExtensionInterface
#if defined(MENGINE_PLATFORM_ANDROID)
        , public AndroidPlatformExtensionInterface
#endif
    {
        DECLARE_UNKNOWABLE();

    public:
        SDLPlatform();
        ~SDLPlatform() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        bool _runService() override;
        void _stopService() override;

    public:
        bool runPlatform()	override;
        void loopPlatform() override;
        bool updatePlatform() override;
        bool tickPlatform( float _frameTime ) override;
        void stopPlatform()	override;

    public:
        UniqueId addUpdate( const LambdaTimer & _lambda, const DocumentPtr & _doc ) override;
        void removeUpdate( UniqueId _id ) override;

    public:
        uint32_t addTimer( float _milliseconds, const LambdaTimer & _lambda, const DocumentPtr & _doc ) override;
        void removeTimer( uint32_t _id ) override;

    public:
        void setSleepMode( bool _sleepMode ) override;
        bool getSleepMode() const override;

    public:
        TimeMilliseconds getPlatfomTime() const override;

    public:
        void setProjectTitle( const Char * _projectTitle ) override;
        size_t getProjectTitle( Char * const _projectTitle ) const override;

    public:
        bool alreadyRunningMonitor() override;

    public:
        bool createWindow( const Resolution & _resolution, bool _fullscreen ) override;
        bool attachWindow( const void * _hWND ) override;

    public:
        bool hasPlatformTag( const ConstString & _tag ) const override;
        const Tags & getPlatformTags() const override;

    public:
        bool isDesktop() const override;
        bool hasTouchpad() const override;

    public:
        DynamicLibraryInterfacePtr loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentPtr & _doc ) override;

    public:
        bool getDesktopResolution( Resolution * const _resolution ) const override;

        bool getNoFullscreen() const override;
        bool getAlwaysFullscreen() const override;

        bool setProcessDPIAware() override;

        bool isDebuggerPresent() const override;
        void debugBreak() override;
        bool createProcessDump( const Char * _dumpPath, void * _pExceptionPointers, bool _full ) override;

        size_t getCurrentPath( Char * const _currentPath ) const override;
        size_t getUserPath( Char * const _userPath ) const override;
        size_t getUserName( Char * const _userName ) const override;

        size_t getFingerprint( Char * const _fingerprint ) const override;

        void closeWindow() override;
        void minimizeWindow() override;

        void setCursorPosition( const mt::vec2f & _pos ) override;
        void setCursorIcon( const ConstString & _icon ) override;
        bool hasCursorIcon( const ConstString & _icon ) const override;

        void showKeyboard() override;
        void hideKeyboard() override;
        bool isShowKeyboard() const override;

        bool notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) override;
        void notifyVsyncChanged( bool _vsync ) override;
        void notifyCursorModeChanged( bool _mode ) override;
        bool notifyCursorIconSetup( const ConstString & _name, const ContentInterfacePtr & _content, const MemoryInterfacePtr & _buffer ) override;

    public:
        void onEvent( const ConstString & _event, const MapWParams & _params ) override;

    public:
        float getJoystickAxis( uint32_t _index ) const override;

    public:
        size_t getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const override;

    public:
        bool getMaxClientResolution( Resolution * const _resolution ) const override;

    public:
        bool openUrlInDefaultBrowser( const Char * _url ) override;
        bool openMail( const Char * _email, const Char * _subject, const Char * _body ) override;

    public:
        bool existDirectory( const Char * _path, const Char * _directory ) const override;
        bool createDirectory( const Char * _path, const Char * _directory ) override;

    public:
        bool existFile( const Char * _filePath ) override;
        bool removeFile( const Char * _filePath ) override;
        bool moveFile( const Char * _oldFilePath, const Char * _newFilePath ) override;

    public:
        bool findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const override;

    public:
        uint64_t getFileTime( const Char * _filePath ) const override;

    public:
        DateTimeProviderInterfacePtr createDateTimeProvider( const DocumentPtr & _doc ) override;

    public:
        bool updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath ) override;

    public:
        bool createDirectoryUserPicture( const Char * _directoryPath, const Char * _file, const void * _data, size_t _size ) override;
        bool createDirectoryUserMusic( const Char * _directoryPath, const Char * _file, const void * _data, size_t _size ) override;

    public:
        void sleep( uint32_t _ms ) override;

    protected:
        void messageBox( const Char * _caption, const Char * _format, ... ) const override;

    protected:
        bool setClipboardText( const Char * _value ) const override;
        bool getClipboardText( Char * _value, size_t _capacity ) const override;

    public:
        SDL_Window * getWindow() const override;

#if defined(MENGINE_PLATFORM_WINDOWS)
#   if defined(MENGINE_WINDOWS_UNIVERSAL)
    public:
        IInspectable * getWindowHandle() const override;
#   else
    public:
        HWND getWindowHWND() const override;
        HDC getWindowHDC() const override;
        HINSTANCE getWindowHinstance() const override;
#   endif
#elif defined(MENGINE_PLATFORM_LINUX)
    public:
        Window getWindowInstance() const override;
        Display * getWindowDisplay() const override;
#endif

#if defined(MENGINE_PLATFORM_ANDROID)
        JNIEnv * getJENV() const override;

        jclass getJClassActivity() const override;
        jobject getJObjectActivity() const override;

        String getAndroidId() const override;
        String getAndroidPackageName() const override;
#endif

#if defined( MENGINE_ENVIRONMENT_RENDER_OPENGL )
    public:
        SDL_GLContext getGLContext() const override;
#endif

    public:
        uint32_t addSDLEventHandler( const LambdaSDLEventHandler & _handler ) override;
        void removeSDLEventHandler( uint32_t _handlerId ) override;

    protected:
        bool changeWindow_( const Resolution & _resolution, bool _fullscreen );
        void setupWindow_();
        bool createWindow_( const Resolution & _resolution, bool _fullscreen );
        bool applyWindow_();
        void destroyWindow_();

    protected:
        bool processEvents_();

    protected:
        void setActive_( bool _active );

    protected:
        bool isNeedWindowRender() const;

    protected:
        bool initializeFileService_();
        void finalizeFileService_();

    protected:
        void notifyInitializeBaseServices_();
        void notifyCreateApplication_();

#if defined(MENGINE_PLATFORM_ANDROID)
    protected:
        int32_t androidOpenAssetFile( const Char * _path ) override;
        int32_t androidAvailableAssetFile( int32_t _fileId ) override;
        int32_t androidReadAssetFile( int32_t _fileId, int32_t _offset, int32_t _size, void * const _buffer ) override;
        int32_t androidSkipAssetFile( int32_t _fileId, int32_t _offset ) override;
        void androidResetAssetFile( int32_t _fileId ) override;
        void androidCloseAssetFile( int32_t _fileId ) override;

    protected:
        void addAndroidEventation( const AndroidEventationInterfacePtr & _eventation ) override;
        void removeAndroidEventation( const AndroidEventationInterfacePtr & _eventation ) override;
#endif

    protected:
        TimeMilliseconds m_beginTime;

        Tags m_platformTags;

        SDL_Window * m_sdlWindow;

        SDL_Joystick * m_sdlAccelerometer;

        struct SDLEventHandlerDesc
        {
            UniqueId id;
            LambdaSDLEventHandler handler;
        };

        typedef Vector<SDLEventHandlerDesc> VectorSDLEventHandlers;
        VectorSDLEventHandlers m_sdlEventHandlers;

        struct TimerDesc
        {
            UniqueId id;
            float milliseconds;
            float time;
            LambdaTimer lambda;

#ifdef MENGINE_DOCUMENT_ENABLE
            DocumentPtr doc;
#endif
        };

        typedef Vector<TimerDesc> VectorTimers;
        VectorTimers m_timers;

        struct UpdateDesc
        {
            UniqueId id;
            LambdaUpdate lambda;

#ifdef MENGINE_DEBUG
            DocumentPtr doc;
#endif
        };

        typedef Vector<UpdateDesc> VectorUpdates;
        VectorUpdates m_updates;

        FactoryInterfacePtr m_factoryDynamicLibraries;
        FactoryInterfacePtr m_factoryDateTimeProviders;

#if defined(MENGINE_ENVIRONMENT_RENDER_OPENGL)
        SDL_GLContext m_glContext;
#endif

#if defined(MENGINE_PLATFORM_ANDROID)
        JNIEnv * m_jenv;

        AndroidEventationHubPtr m_androidEventationHub;
#endif

        SDLInputPtr m_sdlInput;

        StaticString<MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME> m_projectTitle;
        StaticString<MENGINE_SHA1_HEX_COUNT + 1> m_fingerprint; //SHA1

        TimeMilliseconds m_prevTime;

        float m_pauseUpdatingTime;

        bool m_active;
        bool m_sleepMode;
        bool m_shouldQuit;

        bool m_desktop;
        bool m_touchpad;
    };
}
