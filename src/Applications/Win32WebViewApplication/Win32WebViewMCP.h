#pragma once

#include "Plugins/MCPPlugin/MCPProtocol.h"

#include "Interface/LoggerInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/JSON.h"

#include <winsock2.h>
#include <ws2tcpip.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32WebViewMCP
        : public Factorable
    {
        DECLARE_FACTORABLE( Win32WebViewMCP );

    public:
        Win32WebViewMCP();
        ~Win32WebViewMCP() override;

    public:
        bool initialize( HWND _window, int _argc, WChar ** _argv );
        void finalize();
        void tick();

    protected:
        void onRequest_( const MCPFrame & _frame );
        void sendJSON_( EMCPFrameType _type, uint32_t _id, const jpp::object & _value );
        void reply_( uint32_t _id, const jpp::object & _value );
        void error_( uint32_t _id, const Char * _message );
        void queueFrame_( const Data & _frame );
        bool connect_( const String & _host, const String & _port );
        jpp::object status_() const;

    protected:
        HWND m_window;
        SOCKET m_socket;
        MCPFrameDecoder m_decoder;
        Data m_output;
        size_t m_outputOffset;
        bool m_connecting;
        bool m_stopping;

        LoggerInterfacePtr m_logCollector;
        ThreadMutexInterfacePtr m_logMutex;
        jpp::array m_logs;
        uint64_t m_logSequence;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32WebViewMCP> Win32WebViewMCPPtr;
    //////////////////////////////////////////////////////////////////////////
}
