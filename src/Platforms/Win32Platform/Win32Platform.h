#pragma once

#include "Interface/PlatformInterface.h"
#include "Interface/Win32PlatformExtensionInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/FactoryInterface.h"

#include "Win32MouseEvent.h"
#include "Win32AlreadyRunningMonitor.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/MouseButtonCode.h"
#include "Kernel/StaticString.h"
#include "Kernel/StaticWString.h"
#include "Kernel/SHA1.h"

#include "Config/Time.h"
#include "Config/UniqueId.h"

namespace Mengine
{
    class Win32Platform
        : public ServiceBase<PlatformInterface>
        , public Win32PlatformExtensionInterface
    {
        DECLARE_UNKNOWABLE();

    public:
        Win32Platform();
        ~Win32Platform() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        bool _runService() override;

    public:
        UniqueId addUpdate( const LambdaTimer & _lambda, const DocumentPtr & _doc ) override;
        void removeUpdate( UniqueId _id ) override;

    public:
        UniqueId addTimer( float _milliseconds, const LambdaTimer & _lambda, const DocumentPtr & _doc ) override;
        void removeTimer( UniqueId _id ) override;

    public:
        void setSleepMode( bool _sleepMode ) override;
        bool getSleepMode() const override;

    public:
        TimeMilliseconds getPlatfomTime() const override;

    public:
        bool runPlatform() override;
        void loopPlatform() override;
        bool updatePlatform() override;
        bool tickPlatform( float _frameTime ) override;
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
        DynamicLibraryInterfacePtr loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentPtr & _doc ) override;

    public:
        bool getDesktopResolution( Resolution * const _resolution ) const override;

        bool getNoFullscreen() const override;
        bool getAlwaysFullscreen() const override;

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

    protected:
        HCURSOR loadCursorICO_( const FilePath & _filePath, const MemoryInterfacePtr & _buffer ) const;
        HCURSOR getCursorICO_( const ConstString & _name, const FilePath & _filePath, const MemoryInterfacePtr & _buffer );

    public:
        void onEvent( const ConstString & _event, const MapWParams & _params ) override;

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

    public:
        DateTimeProviderInterfacePtr createDateTimeProvider( const DocumentPtr & _doc ) override;

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
        bool getLocalMachineRegValue( const Char * _path, const Char * _key, Char * const _value, size_t _size ) override;
        bool createProcess( const Char * _process, const Char * _command, bool _wait, uint32_t * const _exitCode ) override;

    protected:
        bool setProcessDPIAware() override;

    public:
        bool isDebuggerPresent() const override;
        void debugBreak() override;
        bool createProcessDump( const Char * _dumpPath, void * const _pExceptionPointers, bool _full ) override;

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
        uint32_t addWin32ProcessHandler( const LambdaWin32ProcessHandler & _lambda, const DocumentPtr & _doc ) override;
        void removeWin32ProcessHandler( uint32_t _id ) override;

    protected:
        time_t getFileUnixTime( const FILETIME * filetime ) const override;

    protected:
        bool getCallstack( uint64_t _threadId, Char * const _stack, size_t _capacity, PCONTEXT _context ) const override;

    protected:
        bool isNeedWindowRender() const;

    protected:
        bool initializeFileService_();
        void finalizeFileService_();

    protected:
        void updateWndMessage_();

    protected:
        TimeMilliseconds m_beginTime;

        StaticWString<MENGINE_MAX_PATH> m_windowClassName;
        
        HINSTANCE m_hInstance;

        HWND m_hWnd;

        LARGE_INTEGER m_performanceFrequency;
        bool m_performanceSupport;

        StaticString<MENGINE_SHA1_HEX_COUNT + 1> m_fingerprint; //SHA1

        FactoryInterfacePtr m_factoryDynamicLibraries;
        FactoryInterfacePtr m_factoryDateTimeProviders;

        Win32AlreadyRunningMonitorPtr m_alreadyRunningMonitor;

        Win32MouseEvent m_mouseEvent;

        struct Win32ProcessDesc
        {
            UniqueId id;
            LambdaWin32ProcessHandler lambda;

#ifdef MENGINE_DOCUMENT_ENABLE
            DocumentPtr doc;
#endif
        };

        typedef Vector<Win32ProcessDesc> VectorWin32ProcessHandler;
        VectorWin32ProcessHandler m_win32ProcessHandlers;

        struct TimerDesc
        {
            UniqueId id;
            float milliseconds;
            float time;
            LambdaTimer lambda;

#ifdef MENGINE_DEBUG
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

        HICON m_hIcon;
        StaticWString<MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME> m_projectTitle;

        TimeMilliseconds m_prevTime;

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

#ifdef MENGINE_WINDOWS_SUPPORT_MIN_VERSION_VISTA
        bool m_sessionNotification;
        bool m_sessionLock;
#endif
    };
}
