#pragma once

#include "Config/Char.h"
#include "Config/Atomic.h"
#include "Config/Typedef.h"

#include "Kernel/Factorable.h"

#include "Environment/Windows/WindowsIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    // Thin named-pipe -> PostMessage bridge intended for automated agents
    // (CI runners, LLM-driven test harnesses, drag-and-drop probes, ...)
    // that need to drive the engine through the ordinary Win32 WndProc path
    // without owning a real keyboard / mouse / foreground window.
    //
    // Pipe name:
    //   \\.\pipe\Mengine.MessageBridge              (no suffix)
    //   \\.\pipe\Mengine.MessageBridge.<suffix>     (suffix supplied by host)
    //
    // The suffix is opaque to the engine: callers (typically the launching
    // agent) generate it themselves and pass it through the --messagepipe
    // cmdline option so that the same agent process can deterministically
    // open the matching pipe afterwards.
    //
    // Wire protocol: line-delimited (LF) ASCII text. One command per line:
    //
    //   post <msg-hex> <wparam-hex> <lparam-hex>\n
    //
    // The bridge translates each accepted line into a single
    //   ::PostMessage( hwnd, msg, wParam, lParam )
    // call against the main window. Frame-pacing, button pairing
    // (DOWN/UP), capture lifetime and any timing between events are the
    // sole responsibility of the agent: the bridge is intentionally a
    // dumb transparent forwarder.
    //////////////////////////////////////////////////////////////////////////
    class Win32MessagePipeBridge
        : public Factorable
    {
        DECLARE_FACTORABLE( Win32MessagePipeBridge );

    public:
        Win32MessagePipeBridge();
        ~Win32MessagePipeBridge() override;

    public:
        // _hwnd: target window for translated PostMessage calls.
        // _suffix: optional pipe-name suffix; pass NULL or "" for default.
        bool initialize( HWND _hwnd, const Char * _suffix );
        void finalize();

    protected:
        // Win32 thread entry trampoline.
        static DWORD WINAPI threadEntry_( LPVOID _userdata );

        // Server loop: accept one client, drain commands, disconnect, repeat
        // until m_stop is signalled or the pipe handle has been closed.
        void threadLoop_();

        // Read one '\n'-terminated line from the connected pipe into
        // _line (NUL-terminated, trailing CR/LF stripped). Returns false
        // on stop / disconnect / read error.
        bool readLine_( Char * const _line, size_t _capacity );

        // Parse one accepted command and execute it. Unknown / malformed
        // commands are logged and ignored (no fatal error: we do not want
        // a typo from the agent to bring the engine down).
        void processLine_( const Char * _line );

    protected:
        HWND       m_hwnd;
        HANDLE     m_pipe;
        HANDLE     m_thread;
        AtomicBool m_stop;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32MessagePipeBridge> Win32MessagePipeBridgePtr;
    //////////////////////////////////////////////////////////////////////////
}
