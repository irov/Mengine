#pragma once

#include "Kernel/Map.h"
#include "Kernel/String.h"
#include "Kernel/Vector.h"

#include "Config/Atomic.h"

#include "jpp/jpp.hpp"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "pybind/object.hpp"
#   include "pybind/kernel_interface.hpp"
#endif

namespace Mengine
{
    class MCPCommandHostInterface;
    //////////////////////////////////////////////////////////////////////////
    class MCPDebuggerContext
    {
    public:
        struct Breakpoint
        {
            uint32_t id;
            uint32_t line;
            uint32_t hitTarget;
            uint32_t hits;
            bool enabled;
            String condition;
            String logMessage;
        };

        typedef Vector<Breakpoint> VectorBreakpoints;

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        typedef Map<uint32_t, pybind::object> MapFrames;
#endif

    public:
        MCPDebuggerContext();

        void initialize( MCPCommandHostInterface * _host );
        void finalize();
        void disconnect();

        uint32_t allocateBreakpointId();
        void setBreakpoints( const String & _filename, VectorBreakpoints && _breakpoints );
        void transferBreakpoints( const String & _filename, uint32_t _lineCount, jpp::array * const _transferred, jpp::array * const _disabled );

        void setExceptionPolicy( const String & _policy );
        const String & getExceptionPolicy() const;

        void requestPause();
        bool continueExecution();
        bool startStep( const String & _kind );

        bool isPaused() const;
        uint32_t getPauseGeneration() const;
        const String & getPauseReason() const;

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        const MapFrames & getFrames() const;
        bool resolveFrame( const Char * _frameId, pybind::object * const _frame, String * const _error ) const;
        jpp::object makeFrameJSON( const pybind::object & _frame, uint32_t _index ) const;
#endif

    protected:
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        static void debuggerTrace_( void * _userData, const pybind::debugger_event_t & _event );
        void onDebuggerTrace_( const pybind::debugger_event_t & _event );
        void updateHooks_();
        bool shouldPause_( const pybind::debugger_event_t & _event, String * const _reason );
        void enterPause_( PyObject * _frame, const String & _reason );
        String makeFrameId_( uint32_t _index ) const;
        size_t getFrameDepth_( PyObject * _frame ) const;
#endif

    protected:
        MCPCommandHostInterface * m_host;
        uint32_t m_nextBreakpointId;
        uint32_t m_pauseGeneration;
        uint32_t m_stepDepth;
        String m_exceptionPolicy;
        String m_stepKind;
        String m_pauseReason;
        AtomicBool m_pauseRequested;
        bool m_callbackActive;

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        Map<String, VectorBreakpoints> m_breakpoints;
        MapFrames m_frames;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
}
