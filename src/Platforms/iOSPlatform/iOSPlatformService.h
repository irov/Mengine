#pragma once

#include "Interface/PlatformServiceInterface.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/ConverterInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/PluginInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/ModuleInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/FactoryInterface.h"
#include "Interface/AnalyticsEventProviderInterface.h"

#include "Environment/SDL2/SDL2PlatformServiceExtensionInterface.h"

#import "iOSMailComposeDelegate.h"

#include "Environment/SDL2/SDL2Includer.h"

#include "iOSInput.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/SHA1.h"
#include "Kernel/StaticString.h"

#include "Config/Timestamp.h"

namespace Mengine
{
    class iOSPlatformService
        : public ServiceBase<PlatformServiceInterface>
        , public SDL2PlatformServiceExtensionInterface
    {
        DECLARE_UNKNOWABLE();

    public:
        iOSPlatformService();
        ~iOSPlatformService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        bool _runService() override;
        void _stopService() override;

    protected:
        bool initializeFileService() override;
        void finalizeFileService() override;

    public:
        bool runPlatform()	override;
        void loopPlatform() override;
        bool updatePlatform() override;
        void tickPlatform( float _frameTime, bool _render, bool _flush, bool _pause ) override;
        void stopPlatform()	override;

    public:
        void freezePlatform( bool _tick, bool _render, bool _sound ) override;
        void unfreezePlatform( bool _tick, bool _render, bool _sound ) override;

    public:
        void setSleepMode( bool _sleepMode ) override;
        bool getSleepMode() const override;

    public:
        Timestamp getPlatfomTime() const override;

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
        int32_t dpToWidthPx( int32_t _dp ) const override;
        int32_t dpToHeightPx( int32_t _dp ) const override;

        int32_t pxToWidthDp( int32_t _px ) const override;
        int32_t pxToHeightDp( int32_t _px ) const override;

    public:
        DynamicLibraryInterfacePtr loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentInterfacePtr & _doc ) override;

    public:
        bool getDesktopResolution( Resolution * const _resolution ) const override;

        bool getNoFullscreen() const override;
        bool getAlwaysFullscreen() const override;

        bool isDebuggerPresent() const override;
        void debugBreak() override;

        size_t getCurrentPath( Char * const _currentPath ) const override;
        size_t getUserPath( Char * const _userPath ) const override;
        size_t getExtraPreferencesFolderName( Char * const _folderName ) const override;
        bool getUserLocaleLanguage( Char * const _userLocaleLanguage ) const override;

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
        void onEvent( const ConstString & _event, const Params & _params ) override;

    public:
        size_t getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const override;

    public:
        bool getMaxClientResolution( Resolution * const _resolution ) const override;

    public:
        bool openUrlInDefaultBrowser( const Char * _url ) override;
        bool openMail( const Char * _email, const Char * _subject, const Char * _body ) override;
        bool openDeleteAccount() override;

    public:
        bool updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath ) override;

    public:
        bool createDirectoryUserPicture( const Char * _directoryPath, const Char * _file, const void * _data, size_t _size ) override;
        bool createDirectoryUserMusic( const Char * _directoryPath, const Char * _file, const void * _data, size_t _size ) override;

    protected:
        void messageBox( const Char * _caption, const Char * _format, ... ) const override;

    protected:
        bool setClipboardText( const Char * _value ) const override;
        bool getClipboardText( Char * _value, size_t _capacity ) const override;

    public:
        SDL_Window * getWindow() const override;

    public:
        UIWindow * getUIWindow() const override;

    public:
        SDL_GLContext getGLContext() const override;

    public:
        UniqueId addSDLEventHandler( const LambdaSDLEventHandler & _handler ) override;
        void removeSDLEventHandler( UniqueId _handlerId ) override;

    protected:
        bool changeWindow_( const Resolution & _resolution, bool _fullscreen );
        void setupWindow_();
        bool createWindow_( const Resolution & _resolution, bool _fullscreen );
        bool applyWindow_();
        void destroyWindow_();

    protected:
        bool processEvents_();
        
    protected:
        void pushQuitEvent_();

    protected:
        void setActive_( bool _active );

    protected:
        bool isNeedWindowRender() const;

    protected:
        Timestamp m_beginTime;

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

        SDL_GLContext m_glContext;

        iOSInputPtr m_sdlInput;

        StaticString<MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME> m_projectTitle;
        
        AnalyticsEventProviderInterfacePtr m_analyticsEventProvider;
        
        float m_mainScreenScale;

        double m_prevTime;

        float m_pauseUpdatingTime;

        bool m_active;
        bool m_sleepMode;
        
        AtomicInt32 m_freezedTick;
        AtomicInt32 m_freezedRender;
        AtomicInt32 m_freezedSound;

        bool m_desktop;
        bool m_touchpad;
    };
}
