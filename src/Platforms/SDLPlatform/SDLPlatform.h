#pragma once

#include "Interface/ApplicationInterface.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
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
#include "Interface/ConfigServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/SDLPlatformExtensionInterface.h"

#include "Kernel/FileLogger.h"
#include "Kernel/Factory.h"
#include "Kernel/ServiceBase.h"

#include "SDLInput.h"

#include "Environment/SDL2/SDL2Includer.h"

namespace Mengine
{
    class SDLPlatform
        : public ServiceBase<PlatformInterface>
        , public SDLPlatformExtensionInterface
    {
        DECLARE_UNKNOWABLE();

    public:
        SDLPlatform();
        ~SDLPlatform() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        void _runService() override;

    public:
        bool runPlatform()	override;
        void updatePlatform() override;
        void stopPlatform()	override;

    public:
        uint32_t addTimer( float _milliseconds, const LambdaTimer & _lambda, const DocumentPtr & _doc ) override;
        void removeTimer( uint32_t _id ) override;

    public:
        uint64_t getTicks() const override;

    public:
        void setIcon( uint32_t _icon ) override;
        uint32_t getIcon() const override;

        void setProjectTitle( const Char * _projectTitle ) override;
        size_t getProjectTitle( Char * const _projectTitle ) const override;

    public:
        bool alreadyRunningMonitor() override;

    public:
        bool createWindow( const Resolution & _resolution, bool _fullscreen ) override;

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
        bool createProcessDump( const Char * _dumpPath, void * _pExceptionPointers, bool _full ) override;

        size_t getCurrentPath( Char * const _currentPath ) const override;
        size_t getUserPath( Char * const _userPath ) const override;
        size_t getUserName( Char * const _userName ) const override;

        void closeWindow() override;
        void minimizeWindow() override;

        void setCursorPosition( const mt::vec2f & _pos ) override;
        void setCursorIcon( const ConstString & _icon ) override;
        bool hasCursorIcon( const ConstString & _icon ) const override;

        void showKeyboard() override;
        void hideKeyboard() override;

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
    public:
        virtual HWND getWindowHandle() const = 0;
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
        bool createWindow_( const Resolution & _resolution, bool _fullscreen );
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
        SDL_malloc_func m_old_SDL_malloc_func;
        SDL_calloc_func m_old_SDL_calloc_func;
        SDL_realloc_func m_old_SDL_realloc_func;
        SDL_free_func m_old_SDL_free_func;

    protected:
        Tags m_platformTags;
        Resolution m_windowResolution;
        bool m_fullscreen;

        SDL_Window * m_sdlWindow;

        SDL_Joystick * m_sdlAccelerometer;

        struct SDLEventHandlerDesc
        {
            uint32_t id;
            LambdaSDLEventHandler handler;
        };

        typedef Vector<SDLEventHandlerDesc> VectorSDLEventHandlers;
        VectorSDLEventHandlers m_sdlEventHandlers;

        struct TimerDesc
        {
            uint32_t id;
            float milliseconds;
            float time;
            LambdaTimer lambda;

#if MENGINE_DOCUMENT_ENABLE
            DocumentPtr doc;
#endif
        };

        uint32_t m_enumerator;

        typedef Vector<TimerDesc> VectorTimers;
        VectorTimers m_timers;

        FactoryPtr m_factoryDynamicLibraries;
        FactoryPtr m_factoryDateTimeProviders;

#if defined( MENGINE_ENVIRONMENT_RENDER_OPENGL )
        SDL_GLContext m_glContext;
#endif

        SDLInputPtr m_sdlInput;

        uint32_t m_icon;
        Char m_projectTitle[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME] = {'\0'};

        uint64_t m_prevTime;

        float m_pauseUpdatingTime;

        bool m_active;
        bool m_shouldQuit;
        bool m_running;

        bool m_desktop;
        bool m_touchpad;
    };
}
