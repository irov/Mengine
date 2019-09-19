#pragma once

#include "Interface/PlatformInterface.h"

#include "Win32MouseEvent.h"
#include "Win32AlreadyRunningMonitor.h"
#include "Win32FPSMonitor.h"
#include "Win32AntifreezeMonitor.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"

namespace Mengine
{
    class Win32Platform
        : public ServiceBase<PlatformInterface>
    {
    public:
        Win32Platform();
        ~Win32Platform() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        void _runService() override;

    public:
        uint32_t addTimer( float _milliseconds, const LambdaTimer & _lambda ) override;
        void removeTimer( uint32_t _id ) override;

    public:
        bool runPlatform() override;
        void updatePlatform() override;
        void stopPlatform() override;

    public:
        void setIcon( uint32_t _icon ) override;
        uint32_t getIcon() const override;

        void setProjectTitle( const Char * _projectTitle, size_t _projectTitleSize ) override;
        size_t getProjectTitle( Char * _projectTitle ) const override;

    public:
        bool createWindow( const Resolution & _resolution, bool _fullscreen ) override;
        Pointer getWindowHandle() const override;

    protected:
        ATOM registerClass_( WNDPROC _wndProc, int32_t _clsExtra, int32_t _wndExtra
            , HINSTANCE _hInstance, DWORD _hIcon, HBRUSH _hbrBackground
            , const WChar * _className );

    public:
        const Tags & getPlatformTags() const override;
        bool hasPlatformTag( const ConstString & _tag ) const override;
        bool hasTouchpad() const override;

    public:
        DynamicLibraryInterfacePtr loadDynamicLibrary( const Char * _dynamicLibraryName ) override;

    public:
        bool getDesktopResolution( Resolution & _resolution ) const override;

        bool getNoFullscreen() const override;
        bool getAlwaysFullscreen() const override;

        size_t getCurrentPath( Char * _currentPath ) const override;
        size_t getUserPath( Char * _userPath ) const override;

        void minimizeWindow() override;

        void setCursorPosition( const mt::vec2f & _pos ) override;

        void showKeyboard() override;
        void hideKeyboard() override;

        void notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) override;
        void notifyVsyncChanged( bool _vsync ) override;
        void notifyCursorModeChanged( bool _mode ) override;
        bool notifyCursorIconSetup( const ConstString & _name, const Content * _content, const MemoryInterfacePtr & _buffer ) override;

    public:
        void onEvent( const ConstString & _event, const MapWParams & _params ) override;

    public:
        float getJoystickAxis( uint32_t _index ) const override;

    public:
        size_t getShortPathName( const Char * _path, Char * _short, size_t _len ) const override;
        size_t getSystemFontPath( const Char * _fontName, Char * _fontPath ) const override;

    public:
        void getMaxClientResolution( Resolution & _resolution ) const override;

    public:
        bool openUrlInDefaultBrowser( const Char * _url ) override;

    public:
        bool existDirectory( const Char * _directoryPath ) const override;
        bool createDirectory( const Char * _directoryPath ) override;

    public:
        bool existFile( const Char * _utf8Path ) override;
        bool removeFile( const Char * _filePath ) override;

    public:
        bool findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const override;

    protected:
        bool existFile_( const WChar * _filePath );
        bool createDirectory_( const WChar * _filePath );

    public:
        uint64_t getFileTime( const Char * _filePath ) const override;

    public:
        DateTimeProviderInterfacePtr createDateTimeProvider( const Char * _doc ) override;

    public:
        bool createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) override;
        bool createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) override;

    protected:
        bool createDirectoryUser_( const WChar * _userPath, const WChar * _directoryPath, const WChar * _filePath, const void * _data, size_t _size );

    protected:
        bool getErrorMessage( uint32_t _messageId, Char * _out, size_t _capacity ) const override;

    protected:
        void sleep( uint32_t _ms ) override;
        bool getLocalMachineRegValue( const Char * _path, const Char * _key, Char * _value, size_t _size ) override;
        bool createProcess( const Char * _process, const Char * _command, bool _wait, uint32_t * _exitCode ) override;
        void abort() override;

    protected:
        bool setProcessDPIAware() override;

    public:
        bool isDebuggerPresent() const override;
        bool createProcessDump( const Char * _dumpPath, void * _pExceptionPointers, bool _full ) override;

    public:
        LRESULT wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
        bool wndProcInput( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT & _result );

    protected:
        bool calcCursorPosition_( mt::vec2f & _point ) const;
        DWORD getWindowStyle_( bool _fullsreen ) const;
        RECT getWindowsRect_( const Resolution & _resolution, bool _fullsreen ) const;

    protected:
        void setActive_( bool _active );

    protected:
        HINSTANCE m_hInstance;

        HWND m_hWnd;

        FactoryPtr m_factoryDynamicLibraries;
        FactoryPtr m_factoryDateTimeProviders;

        Win32AlreadyRunningMonitorPtr m_alreadyRunningMonitor;
        Win32FPSMonitorPtr m_fpsMonitor;
        Win32AntifreezeMonitorPtr m_antifreezeMonitor;

        Win32MouseEvent m_mouseEvent;

        struct TimerDesc
        {
            uint32_t id;
            float milliseconds;
            float time;
            LambdaTimer lambda;
        };

        uint32_t m_enumerator;

        typedef Vector<TimerDesc> VectorTimers;
        VectorTimers m_timers;

        uint32_t m_icon;
        WChar m_projectTitle[MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME];

        uint64_t m_prevTime;

        Tags m_platformTags;
        Resolution m_windowResolution;
        bool m_fullscreen;

        float m_activeFrameTime;
        float m_inactiveFrameTime;

        bool m_active;
        bool m_update;
        bool m_close;

        float m_pauseUpdatingTime;

        bool m_vsync;

        bool m_cursorInArea;
        bool m_clickOutArea;
        bool m_cursorMode;

        HCURSOR m_cursor;

        typedef Map<ConstString, HCURSOR> MapCursors;
        MapCursors m_cursors;

        bool m_lastMouse;
        int32_t m_lastMouseX;
        int32_t m_lastMouseY;

        bool m_isDoubleClick;
        bool m_touchpad;
    };
}
