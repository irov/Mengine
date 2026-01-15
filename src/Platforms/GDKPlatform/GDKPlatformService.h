#pragma once

#include "Interface/PlatformServiceInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/FactoryInterface.h"

#include "Environment/Windows/Win32PlatformServiceExtensionInterface.h"

#include "GDKDynamicLibrary.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/StaticString.h"
#include "Kernel/StaticWString.h"

#include "Config/Timestamp.h"
#include "Config/UniqueId.h"

namespace Mengine
{
    class GDKPlatformService
        : public ServiceBase<PlatformServiceInterface>
        , public Win32PlatformServiceExtensionInterface
    {
        DECLARE_UNKNOWABLE();

    public:
        GDKPlatformService();
        ~GDKPlatformService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        bool _runService() override;
        void _stopService() override;

    protected:
        bool initializeFileService() override;
        void finalizeFileService() override;

    public:
        Timestamp getPlatfomTime() const override;

    public:
        bool runPlatform() override;
        void loopPlatform() override;
        bool updatePlatform() override;
        void tickPlatform( float _frameTime ) override;
        bool renderPlatform() override;
        void stopPlatform() override;

    public:
        void setProjectTitle( const Char * _projectTitle ) override;
        size_t getProjectTitle( Char * const _projectTitle ) const override;

    public:
        bool alreadyRunningMonitor() override;

    public:
        bool createWindow( const Resolution & _resolution, bool _fullscreen ) override;

    public:
        bool atachWindow( HWND _hWnd, bool _fullscreen ) override;
        bool detachWindow() override;

    public:
        const Tags & getPlatformTags() const override;
        bool hasPlatformTag( const ConstString & _tag ) const override;

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

        size_t getCurrentPath( Char * const _currentPath ) const override;
        size_t getUserPath( Char * const _userPath ) const override;
        size_t getExtraPreferencesFolderName( Char * const _folderName ) const override;
        bool getUserLocaleLanguage( Char * const _userLocaleLanguage ) const override;

        void closeWindow() override;
        void minimizeWindow() override;

        void setCursorPosition( const mt::vec2f & _cursorPosition ) override;
        void setCursorIcon( const ConstString & _cursorIcon ) override;
        bool hasCursorIcon( const ConstString & _cursorIcon ) const override;

        void showKeyboard() override;
        void hideKeyboard() override;
        bool isShowKeyboard() const override;

        bool notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) override;
        void notifyVsyncChanged( bool _vsync ) override;
        void notifyCursorModeChanged( bool _cursorMode ) override;
        bool notifyCursorIconSetup( const ConstString & _name, const ContentInterfacePtr & _content, const MemoryInterfacePtr & _buffer ) override;

    public:
        void onEvent( const ConstString & _event, const Params & _params ) override;

    public:
        size_t getShortPathName( const Char * _path, Char * const _short ) const override;
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
        bool createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) override;
        bool createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) override;

    protected:
        void messageBox( const Char * _caption, const Char * _format, ... ) const override;

    protected:
        bool setClipboardText( const Char * _value ) const override;
        bool getClipboardText( Char * _value, size_t _capacity ) const override;

    public:
        bool isDebuggerPresent() const override;
        void debugBreak() override;

    public:
        bool setHWNDIcon( const WChar * _iconResource ) override;

    protected:
        DWORD getWindowStyle_( bool _fullscreen ) const;
        DWORD getWindowExStyle_( bool _fullscreen ) const;
        bool calcWindowsRect_( const Resolution & _resolution, bool _fullscreen, RECT * const _rect ) const;

    protected:
        HWND getWindowHandle() const override;

    protected:
        UniqueId addWin32ProcessHandler( const LambdaWin32ProcessHandler & _lambda, const DocumentInterfacePtr & _doc ) override;
        void removeWin32ProcessHandler( UniqueId _id ) override;

    protected:
        Timestamp m_beginTime;

        FactoryInterfacePtr m_factoryDynamicLibraries;

        StaticWString<MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME> m_projectTitle;

        Tags m_platformTags;
        Resolution m_windowResolution;
        bool m_fullscreen;

        bool m_active;
        bool m_close;

        bool m_desktop;
        bool m_touchpad;

        StaticWString<MENGINE_MAX_PATH> m_windowClassName;

        HWND m_hWnd;
    };
}
