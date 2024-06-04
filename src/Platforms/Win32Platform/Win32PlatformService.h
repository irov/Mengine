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
        bool tickPlatform( float _frameTime, bool _render, bool _flush, bool _pause ) override;
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

    public:
        const Tags & getPlatformTags() const override;
        bool hasPlatformTag( const ConstString & _tag ) const override;

    public:
        bool isDesktop() const override;
        bool hasTouchpad() const override;

    public:
        DynamicLibraryInterfacePtr loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentInterfacePtr & _doc ) override;

    public:
        bool getDesktopResolution( Resolution * const _resolution ) const override;

        bool getNoFullscreen() const override;
        bool getAlwaysFullscreen() const override;

        size_t getCurrentPath( Char * const _currentPath ) const override;
        size_t getUserPath( Char * const _userPath ) const override;
        size_t getExtraPreferencesFolderName( Char * const _folderName ) const override;
        size_t getUserName( Char * const _userName ) const override;
        size_t getDeviceLanguage( Char * const _deviceLanguage ) const override;
        size_t getFingerprint( Char * const _fingerprint ) const override;

        size_t getDeviceModel( Char * const _deviceModel ) const override;
        size_t getOsFamily( Char * const _osFamily ) const override;
        size_t getOsVersion( Char * const _osVersion ) const override;
        size_t getBundleId( Char * const _osVersion ) const override;

        size_t getSessionId( Char * const _sessionId ) const override;
        size_t getInstallKey( Char * const _installKey ) const override;
        int64_t getInstallTimestamp() const override;
        size_t getInstallVersion( Char * const _installVersion ) const override;
        int64_t getInstallRND() const override;
        int64_t getSessionIndex() const override;

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

    protected:
        HCURSOR loadCursorICO_( const FilePath & _filePath, const MemoryInterfacePtr & _buffer ) const;
        HCURSOR getCursorICO_( const ConstString & _name, const FilePath & _filePath, const MemoryInterfacePtr & _buffer );

    public:
        void onEvent( const ConstString & _event, const Params & _params ) override;

    public:
        float getJoystickAxis( uint32_t _index ) const override;

    public:
        size_t getShortPathName( const Char * _path, Char * const _short ) const override;
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
        bool existFile( const Char * _utf8Path ) override;
        bool removeFile( const Char * _filePath ) override;
        bool moveFile( const Char * _oldFilePath, const Char * _newFilePath ) override;

    public:
        bool findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const override;

    public:
        uint64_t getFileTime( const Char * _filePath ) const override;

    protected:
        bool existFile_( const WChar * _filePath );
        bool createDirectory_( const WChar * _path, const WChar * _directory );

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
        void sleep( uint32_t _ms ) override;

    protected:
        bool setProcessDPIAware() override;

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
        void getOsInfo( OSVERSIONINFOEXW * const _osInfo ) const override;

    protected:
        UniqueId addWin32ProcessHandler( const LambdaWin32ProcessHandler & _lambda, const DocumentInterfacePtr & _doc ) override;
        void removeWin32ProcessHandler( UniqueId _id ) override;

    protected:
        time_t getFileUnixTime( const FILETIME * filetime ) const override;

    protected:
        bool isNeedWindowRender() const;

    protected:
        void generateFingerprint_();

    protected:
        bool initializeFileService() override;
        void finalizeFileService() override;

    protected:
        void updateWndMessage_();

    protected:
        const Char * getOsVersionName_() const;

    protected:
        Timestamp m_beginTime;

        StaticWString<MENGINE_MAX_PATH> m_windowClassName;

        OSVERSIONINFOEXW m_osInfo;
        
        HINSTANCE m_hInstance;

        HWND m_hWnd;

        LARGE_INTEGER m_performanceFrequency;
        bool m_performanceSupport;

        StaticString<MENGINE_SHA1_HEX_COUNT + 1> m_fingerprint; //SHA1

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

        StaticString<MENGINE_PLATFORM_DEVICE_MODEL_MAXNAME> m_deviceModel;
        StaticString<MENGINE_PLATFORM_OS_FAMILY_MAXNAME> m_osFamily;
        StaticString<MENGINE_PLATFORM_OS_VERSION_MAXNAME> m_osVersion;

        StaticString<MENGINE_PLATFORM_BUNDLEID_MAXNAME> m_bundleId;

        Timestamp m_prevTime;

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
