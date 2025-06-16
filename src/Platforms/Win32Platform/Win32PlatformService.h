#pragma once

#include "Interface/PlatformServiceInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/FactoryInterface.h"

#include "Environment/Windows/Win32PlatformServiceExtensionInterface.h"

#include "Win32MouseEvent.h"
#include "Win32AlreadyRunningMonitor.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/MouseButtonCode.h"
#include "Kernel/StaticString.h"
#include "Kernel/StaticWString.h"
#include "Kernel/SHA1.h"

#include "Config/Timestamp.h"
#include "Config/UniqueId.h"

namespace Mengine
{
    class Win32PlatformService
        : public ServiceBase<PlatformServiceInterface>
        , public Win32PlatformServiceExtensionInterface
    {
        DECLARE_UNKNOWABLE();

    public:
        Win32PlatformService();
        ~Win32PlatformService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        bool _runService() override;
        void _stopService() override;

    public:
        void setSleepMode( bool _sleepMode ) override;
        bool getSleepMode() const override;

    public:
        Timestamp getPlatfomTime() const override;

    public:
        bool runPlatform() override;
        void loopPlatform() override;
        bool updatePlatform() override;
        void tickPlatform( float _frameTime, bool _render, bool _flush, bool _pause ) override;
        void stopPlatform() override;

    public:
        bool setHWNDIcon( const WChar * _iconResource ) override;

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

    protected:
        HCURSOR loadCursorICO_( const FilePath & _filePath, const MemoryInterfacePtr & _buffer ) const;
        HCURSOR getCursorICO_( const ConstString & _name, const FilePath & _filePath, const MemoryInterfacePtr & _buffer );

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

    protected:
        bool getSpecialFolderPath_( DWORD _flag, WChar * const _path ) const;

    public:
        bool updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath ) override;

    public:
        bool createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) override;
        bool createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) override;

    protected:
        bool createDirectoryUser_( const WChar * _userPath, const WChar * _directoryPath, const WChar * _filePath, const void * _data, size_t _size );

    protected:
        bool setProcessDPIAware();

    public:
        bool isDebuggerPresent() const override;
        void debugBreak() override;

    public:
        LRESULT wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
        bool wndProcInput( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT * const _result );

    protected:
        bool calcCursorPosition_( mt::vec2f * const _position ) const;
        bool adaptCursorPosition_( POINT _point, mt::vec2f * const _position ) const;
        bool getCursorPosition_( POINT _point, mt::vec2f * const _position ) const;

    protected:
        DWORD getWindowStyle_( bool _fullsreen ) const;
        DWORD getWindowExStyle_( bool _fullsreen ) const;
        bool calcWindowsRect_( const Resolution & _resolution, bool _fullsreen, RECT * const _rect ) const;

    protected:
        bool sendChar_( WPARAM wParam );

    protected:
        void setActive_( bool _active );

    protected:
        void messageBox( const Char * _caption, const Char * _format, ... ) const override;

    protected:
        bool setClipboardText( const Char * _value ) const override;
        bool getClipboardText( Char * _value, size_t _capacity ) const override;

    protected:
        HWND getWindowHandle() const override;

    protected:
        UniqueId addWin32ProcessHandler( const LambdaWin32ProcessHandler & _lambda, const DocumentInterfacePtr & _doc ) override;
        void removeWin32ProcessHandler( UniqueId _id ) override;

    protected:
        bool isNeedWindowRender() const;

    protected:
        bool initializeFileService() override;
        void finalizeFileService() override;

    protected:
        void updateWndMessage_();

    protected:
        Timestamp m_beginTime;

        StaticWString<MENGINE_MAX_PATH> m_windowClassName;        
        
        HINSTANCE m_hInstance;

        HWND m_hWnd;

        UINT m_xDpi;
        UINT m_yDpi;

        LARGE_INTEGER m_performanceFrequency;
        bool m_performanceSupport;

        FactoryInterfacePtr m_factoryDynamicLibraries;

        Win32AlreadyRunningMonitorPtr m_alreadyRunningMonitor;

        Win32MouseEvent m_mouseEvent;

        struct Win32ProcessDesc
        {
            UniqueId id;
            LambdaWin32ProcessHandler lambda;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
#endif
        };

        typedef Vector<Win32ProcessDesc> VectorWin32ProcessHandler;
        VectorWin32ProcessHandler m_win32ProcessHandlers;

        HICON m_hIcon;
        StaticWString<MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME> m_projectTitle;

        double m_prevTime;

        Tags m_platformTags;
        Resolution m_windowResolution;
        bool m_fullscreen;

        bool m_active;
        bool m_close;

        bool m_sleepMode;
        bool m_windowExposed;

        float m_pauseUpdatingTime;

        bool m_cursorInArea;
        bool m_clickOutArea[MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE];
        bool m_cursorMode;

        HCURSOR m_cursor;

        struct CursorDesc
        {
            HCURSOR cursor;
            bool shared;
        };

        typedef Map<ConstString, CursorDesc> MapCursors;
        MapCursors m_cursors;

        bool m_lastMouse;
        int32_t m_lastMouseX;
        int32_t m_lastMouseY;

        bool m_isDoubleClick[MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE];
        bool m_touchpad;
        bool m_desktop;

#if defined(MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA)
        bool m_sessionNotification;
        bool m_sessionLock;
#endif
    };
}
