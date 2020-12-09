#pragma once

#include "Interface/PlatformInterface.h"
#include "Interface/Win32PlatformExtensionInterface.h"
#include "Interface/LoggerInterface.h"

#include "Win32MouseEvent.h"
#include "Win32AlreadyRunningMonitor.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    class Win32Platform
        : public ServiceBase<PlatformInterface>
        , public Win32PlatformExtensionInterface
    {
    public:
        Win32Platform();
        ~Win32Platform() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        void _runService() override;

    public:
        uint32_t addTimer( float _milliseconds, const LambdaTimer & _lambda, const DocumentPtr & _doc ) override;
        void removeTimer( uint32_t _id ) override;

    public:
        uint64_t getTicks() const override;

    public:
        bool runPlatform() override;
        void updatePlatform() override;
        void stopPlatform() override;

    public:
        void setIcon( uint32_t _icon ) override;
        uint32_t getIcon() const override;

        void setProjectTitle( const Char * _projectTitle ) override;
        size_t getProjectTitle( Char * const _projectTitle ) const override;

    public:
        bool createWindow( const Resolution & _resolution, bool _fullscreen ) override;

    protected:
        ATOM registerClass_( WNDPROC _wndProc, int32_t _clsExtra, int32_t _wndExtra
            , HINSTANCE _hInstance, DWORD _hIcon, HBRUSH _hbrBackground
            , const WChar * _className );

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

        void closeWindow() override;
        void minimizeWindow() override;

        void setCursorPosition( const mt::vec2f & _pos ) override;

        void showKeyboard() override;
        void hideKeyboard() override;

        bool notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) override;
        void notifyVsyncChanged( bool _vsync ) override;
        void notifyCursorModeChanged( bool _mode ) override;
        bool notifyCursorIconSetup( const ConstString & _name, const ContentInterface * _content, const MemoryInterfacePtr & _buffer ) override;

    public:
        void onEvent( const ConstString & _event, const MapWParams & _params ) override;

    public:
        float getJoystickAxis( uint32_t _index ) const override;

    public:
        size_t getShortPathName( const Char * _path, Char * const _short ) const override;
        size_t getSystemFontPath( const Char * _fontName, Char * const _fontPath ) const override;

    public:
        bool getMaxClientResolution( Resolution * const _resolution ) const override;

    public:
        bool openUrlInDefaultBrowser( const Char * _url ) override;

    public:
        bool existDirectory( const Char * _directoryPath ) const override;
        bool createDirectory( const Char * _directoryPath ) override;

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
        bool createDirectory_( const WChar * _filePath );

    public:
        DateTimeProviderInterfacePtr createDateTimeProvider( const DocumentPtr & _doc ) override;

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
        bool getClipboardText( Char * _value, size_t _capacity ) const override;

    protected:
        UnknownPointer getPlatformExtention() override;

    protected:
        HWND getWindowHandle() const override;

    protected:
        uint32_t addWin32ProcessHandler( const LambdaWin32ProcessHandler & _lambda, const DocumentPtr & _doc ) override;
        void removeWin32ProcessHandler( uint32_t _id ) override;

    protected:
        time_t getFileUnixTime( const FILETIME * filetime ) const override;

    protected:
        bool getErrorMessage( DWORD _messageId, Char * const _out, size_t _capacity ) const override;

    protected:
        bool getCallstack( ThreadHandle _thread, Char * const _stack, size_t _capacity, PCONTEXT _context ) const override;

    protected:
        bool getLastErrorMessage( DWORD * const _error, Char * const _out, size_t _capacity ) const;

    protected:
        bool initializeFileService_();
        void finalizeFileService_();

    protected:
        WChar m_windowClassName[MENGINE_MAX_PATH] = {L'\0'};

        HINSTANCE m_hInstance;

        HWND m_hWnd;

        LARGE_INTEGER m_performanceFrequency;
        bool m_performanceSupport;

        FactoryPtr m_factoryDynamicLibraries;
        FactoryPtr m_factoryDateTimeProviders;

        Win32AlreadyRunningMonitorPtr m_alreadyRunningMonitor;

        Win32MouseEvent m_mouseEvent;

        struct Win32ProcessDesc
        {
            uint32_t id;
            LambdaWin32ProcessHandler lambda;

#if MENGINE_DOCUMENT_ENABLE
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

        uint32_t m_icon;
        WChar m_projectTitle[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME] = {'\0'};

        uint64_t m_prevTime;

        Tags m_platformTags;
        Resolution m_windowResolution;
        bool m_fullscreen;

        bool m_active;
        bool m_update;
        bool m_close;

        float m_pauseUpdatingTime;

        bool m_cursorInArea;
        bool m_clickOutArea[3] = {false};
        bool m_cursorMode;

        HCURSOR m_cursor;

        typedef Map<ConstString, HCURSOR> MapCursors;
        MapCursors m_cursors;

        bool m_lastMouse;
        int32_t m_lastMouseX;
        int32_t m_lastMouseY;

        bool m_isDoubleClick[3] = {false};
        bool m_touchpad;
        bool m_desktop;
        bool m_sessionLock;
    };
}
