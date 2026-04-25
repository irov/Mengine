#include "Win32MessagePipeBridge.h"

#include "Environment/Windows/Win32Helper.h"

#include "Kernel/Logger.h"
#include "Kernel/Assertion.h"

#include "Config/Atomic.h"
#include "Config/Path.h"
#include "Config/StdIO.h"
#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        // Bytes-on-the-wire upper bound for one command line. Generous on
        // purpose: protocol is plain ASCII and the bridge is dev-only.
        static const size_t LineCapacity = 256;
        // Server-side input buffer for CreateNamedPipe. Same order of
        // magnitude as LineCapacity so the OS can buffer one or two
        // pipelined commands without forcing the client to block.
        static const DWORD PipeBufferSize = 1024;
        // Default name (suffix omitted). Single-instance launches and the
        // existing AlreadyRunningMonitor make this safe.
        static const Char * DefaultPipeName = "\\\\.\\pipe\\Mengine.MessageBridge";
        //////////////////////////////////////////////////////////////////////////
        static void buildPipeName_( Char * const _out, size_t _capacity, const Char * _suffix )
        {
            if( _suffix == nullptr || _suffix[0] == '\0' )
            {
                StdString::strcpy_safe( _out, DefaultPipeName, _capacity );

                return;
            }

            StdString::strcpy_safe( _out, DefaultPipeName, _capacity );
            StdString::strcat_safe( _out, ".", _capacity );
            StdString::strcat_safe( _out, _suffix, _capacity );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Win32MessagePipeBridge::Win32MessagePipeBridge()
        : m_hwnd( NULL )
        , m_pipe( INVALID_HANDLE_VALUE )
        , m_thread( NULL )
        , m_stop( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32MessagePipeBridge::~Win32MessagePipeBridge()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32MessagePipeBridge::initialize( HWND _hwnd, const Char * _suffix )
    {
        MENGINE_ASSERTION_FATAL( _hwnd != NULL, "message pipe bridge hwnd is NULL" );

        m_hwnd = _hwnd;

        Char pipeName[MENGINE_MAX_PATH] = {'\0'};
        Detail::buildPipeName_( pipeName, MENGINE_MAX_PATH, _suffix );

        // Inbound only: the protocol is fire-and-forget; we never write back
        // to the client. PIPE_TYPE_BYTE + PIPE_READMODE_BYTE keeps the
        // implementation tolerant to chunked writes from any client style
        // (PowerShell, Python, raw sockets, etc.).
        HANDLE pipe = ::CreateNamedPipeA( pipeName
            , PIPE_ACCESS_INBOUND
            , PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT
            , 1                          // single client at a time
            , 0                          // outbound buffer (unused)
            , Detail::PipeBufferSize     // inbound buffer
            , 0                          // default timeout
            , NULL                       // default security: current user
        );

        if( pipe == INVALID_HANDLE_VALUE )
        {
            LOGGER_ERROR( "invalid create message pipe '%s' %ls"
                , pipeName
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        m_pipe = pipe;
        m_stop.store( false, StdAtomic::memory_order_release );

        m_thread = ::CreateThread( NULL, 0, &Win32MessagePipeBridge::threadEntry_, this, 0, NULL );

        if( m_thread == NULL )
        {
            LOGGER_ERROR( "invalid create message pipe thread for '%s' %ls"
                , pipeName
                , Helper::Win32GetLastErrorMessageW()
            );

            ::CloseHandle( m_pipe );
            m_pipe = INVALID_HANDLE_VALUE;

            return false;
        }

        LOGGER_MESSAGE( "message pipe bridge listening on '%s'"
            , pipeName
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32MessagePipeBridge::finalize()
    {
        m_stop.store( true, StdAtomic::memory_order_release );

        // Cancel any in-flight ConnectNamedPipe / ReadFile so the server
        // thread observes m_stop on its next loop iteration. Without this
        // the thread can sit indefinitely inside a blocking ConnectNamedPipe.
        if( m_pipe != INVALID_HANDLE_VALUE )
        {
            ::CancelIoEx( m_pipe, NULL );
            // Drop any half-connected client first so CloseHandle below
            // does not race with kernel-side state for that client.
            ::DisconnectNamedPipe( m_pipe );
        }

        if( m_thread != NULL )
        {
            ::WaitForSingleObject( m_thread, INFINITE );
            ::CloseHandle( m_thread );
            m_thread = NULL;
        }

        if( m_pipe != INVALID_HANDLE_VALUE )
        {
            ::CloseHandle( m_pipe );
            m_pipe = INVALID_HANDLE_VALUE;
        }

        m_hwnd = NULL;
    }
    //////////////////////////////////////////////////////////////////////////
    DWORD WINAPI Win32MessagePipeBridge::threadEntry_( LPVOID _userdata )
    {
        Win32MessagePipeBridge * self = static_cast<Win32MessagePipeBridge *>(_userdata);

        self->threadLoop_();

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32MessagePipeBridge::threadLoop_()
    {
        while( m_stop.load( StdAtomic::memory_order_acquire ) == false )
        {
            // Block until a client connects (or until CancelIoEx wakes us
            // from finalize()).
            BOOL connected = ::ConnectNamedPipe( m_pipe, NULL );

            if( connected == FALSE )
            {
                DWORD err = ::GetLastError();

                // ERROR_PIPE_CONNECTED: a client connected between
                // CreateNamedPipe and ConnectNamedPipe (this is a success).
                if( err != ERROR_PIPE_CONNECTED )
                {
                    if( m_stop.load( StdAtomic::memory_order_acquire ) == true )
                    {
                        break;
                    }

                    // Real connect failure (e.g. ERROR_OPERATION_ABORTED
                    // from CancelIoEx, ERROR_NO_DATA, ...): drop any
                    // half-state and retry the accept loop.
                    ::DisconnectNamedPipe( m_pipe );

                    continue;
                }
            }

            Char line[Detail::LineCapacity] = {'\0'};
            while( this->readLine_( line, Detail::LineCapacity ) == true )
            {
                this->processLine_( line );
            }

            ::DisconnectNamedPipe( m_pipe );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32MessagePipeBridge::readLine_( Char * const _line, size_t _capacity )
    {
        size_t offset = 0;

        while( offset + 1 < _capacity )
        {
            if( m_stop.load( StdAtomic::memory_order_acquire ) == true )
            {
                return false;
            }

            Char ch = '\0';
            DWORD readBytes = 0;

            BOOL ok = ::ReadFile( m_pipe, &ch, 1, &readBytes, NULL );

            if( ok == FALSE || readBytes == 0 )
            {
                // Client closed, ReadFile cancelled, or any other I/O error
                // - either way the current connection is over.
                return false;
            }

            if( ch == '\n' )
            {
                // Strip a trailing CR (CRLF clients).
                if( offset > 0 && _line[offset - 1] == '\r' )
                {
                    --offset;
                }

                _line[offset] = '\0';

                return true;
            }

            _line[offset] = ch;
            ++offset;
        }

        // Line longer than buffer: truncate, treat what we have as a line.
        _line[offset] = '\0';

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32MessagePipeBridge::processLine_( const Char * _line )
    {
        if( _line[0] == '\0' )
        {
            // Empty line: silently ignored, lets clients flush with "\n\n".
            return;
        }

        unsigned int      msg = 0;
        unsigned long long wparam = 0;
        unsigned long long lparam = 0;

        // %llx parses hex into 64-bit so WPARAM/LPARAM survive on x64.
        // Hex prefix "0x" is optional; both "post 200 ..." and
        // "post 0x0200 ..." work.
        int matched = MENGINE_SSCANF( _line, "post %x %llx %llx", &msg, &wparam, &lparam );

        if( matched != 3 )
        {
            LOGGER_ERROR( "message pipe bridge: malformed command '%s' (expected 'post <msg-hex> <wparam-hex> <lparam-hex>')"
                , _line
            );

            return;
        }

        WPARAM wp = static_cast<WPARAM>(wparam);
        LPARAM lp = static_cast<LPARAM>(lparam);

        if( ::PostMessageA( m_hwnd, static_cast<UINT>(msg), wp, lp ) == FALSE )
        {
            LOGGER_ERROR( "message pipe bridge: PostMessage(0x%04x, 0x%llx, 0x%llx) failed %ls"
                , msg
                , (unsigned long long)wp
                , (unsigned long long)lp
                , Helper::Win32GetLastErrorMessageW()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
