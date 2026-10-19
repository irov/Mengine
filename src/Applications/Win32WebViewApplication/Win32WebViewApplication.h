#pragma once

#include "Interface/LoggerInterface.h"

#if defined(MENGINE_WEBVIEW_MCP)
#include "Win32WebViewMCP.h"
#endif

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    class Win32WebViewApplication
    {
    public:
        Win32WebViewApplication();
        ~Win32WebViewApplication();

    public:
        bool initialize( HINSTANCE _instance, int _argc, WChar ** _argv );
        int loop();
        void finalize();
        LRESULT handleMessage( HWND _window, UINT _message, WPARAM _wparam, LPARAM _lparam );
        static LRESULT CALLBACK windowProcedure( HWND _window, UINT _message, WPARAM _wparam, LPARAM _lparam );

    protected:
        HWND m_window;
        HINSTANCE m_instance;
        ULONGLONG m_previousTime;
        bool m_running;
        LoggerInterfacePtr m_logger;
#if defined(MENGINE_WEBVIEW_MCP)
        Win32WebViewMCPPtr m_mcp;
#endif
    };
}
