#include "MCPDebuggerContext.h"

#include "../Commands/MCPCommandInterface.h"

#include "Config/StdUtility.h"

#include "Kernel/Logger.h"
#include "Kernel/Stringstream.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Interface/ScriptProviderServiceInterface.h"
#   include "Interface/ThreadSystemInterface.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPDebuggerContext::MCPDebuggerContext()
        : m_host( nullptr )
        , m_nextBreakpointId( 1 )
        , m_pauseGeneration( 0 )
        , m_stepDepth( 0 )
        , m_exceptionPolicy( "none" )
        , m_pauseRequested( false )
        , m_callbackActive( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPDebuggerContext::initialize( MCPCommandHostInterface * _host )
    {
        m_host = _host;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPDebuggerContext::finalize()
    {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();
        kernel->debugger_set_trace( nullptr, nullptr );

        m_breakpoints.clear();
        m_frames.clear();
#endif

        if( m_host != nullptr )
        {
            m_host->setMCPDebuggerPaused( false );
        }

        m_host = nullptr;
        m_nextBreakpointId = 1;
        m_pauseGeneration = 0;
        m_stepDepth = 0;
        m_exceptionPolicy = "none";
        m_stepKind.clear();
        m_pauseReason.clear();
        m_pauseRequested = false;
        m_callbackActive = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPDebuggerContext::disconnect()
    {
        m_pauseRequested = false;
        m_stepKind.clear();

        if( m_host != nullptr )
        {
            m_host->setMCPDebuggerPaused( false );
        }

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        m_frames.clear();
        m_pauseReason.clear();
        this->updateHooks_();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MCPDebuggerContext::allocateBreakpointId()
    {
        uint32_t id = m_nextBreakpointId++;

        if( m_nextBreakpointId == 0 )
        {
            m_nextBreakpointId = 1;
        }

        return id;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPDebuggerContext::setBreakpoints( const String & _filename, VectorBreakpoints && _breakpoints )
    {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        m_breakpoints[_filename] = StdUtility::move( _breakpoints );
        this->updateHooks_();
#else
        MENGINE_UNUSED( _filename );
        MENGINE_UNUSED( _breakpoints );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPDebuggerContext::transferBreakpoints( const String & _filename, uint32_t _lineCount, jpp::array * const _transferred, jpp::array * const _disabled )
    {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        Map<String, VectorBreakpoints>::iterator it = m_breakpoints.find( _filename );

        if( it == m_breakpoints.end() )
        {
            return;
        }

        for( Breakpoint & breakpoint : it->second )
        {
            jpp::object item = jpp::make_object();
            item.set( "id", breakpoint.id );
            item.set( "line", breakpoint.line );

            if( breakpoint.line > _lineCount )
            {
                breakpoint.enabled = false;
                item.set( "reason", "line is outside the reloaded source" );
                _disabled->push_back( item );

                continue;
            }

            _transferred->push_back( item );
        }

        this->updateHooks_();
#else
        MENGINE_UNUSED( _filename );
        MENGINE_UNUSED( _lineCount );
        MENGINE_UNUSED( _transferred );
        MENGINE_UNUSED( _disabled );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPDebuggerContext::setExceptionPolicy( const String & _policy )
    {
        m_exceptionPolicy = _policy;

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        this->updateHooks_();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    const String & MCPDebuggerContext::getExceptionPolicy() const
    {
        const String & policy = m_exceptionPolicy;

        return policy;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPDebuggerContext::requestPause()
    {
        m_pauseRequested = true;

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        this->updateHooks_();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPDebuggerContext::continueExecution()
    {
        if( this->isPaused() == false )
        {
            return false;
        }

        m_stepKind.clear();
        m_host->setMCPDebuggerPaused( false );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPDebuggerContext::startStep( const String & _kind )
    {
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        if( this->isPaused() == false || m_frames.empty() == true )
        {
            return false;
        }

        m_stepKind = _kind;
        const pybind::object & frame = m_frames.begin()->second;
        PyObject * frameObject = frame.ptr();
        size_t depth = this->getFrameDepth_( frameObject );
        m_stepDepth = (uint32_t)depth;
        this->updateHooks_();
        m_host->setMCPDebuggerPaused( false );

        return true;
#else
        MENGINE_UNUSED( _kind );

        return false;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPDebuggerContext::isPaused() const
    {
        if( m_host == nullptr )
        {
            return false;
        }

        bool paused = m_host->isMCPDebuggerPaused();

        return paused;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MCPDebuggerContext::getPauseGeneration() const
    {
        uint32_t generation = m_pauseGeneration;

        return generation;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & MCPDebuggerContext::getPauseReason() const
    {
        const String & reason = m_pauseReason;

        return reason;
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    const MCPDebuggerContext::MapFrames & MCPDebuggerContext::getFrames() const
    {
        const MapFrames & frames = m_frames;

        return frames;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPDebuggerContext::resolveFrame( const Char * _frameId, pybind::object * const _frame, String * const _error ) const
    {
        uint32_t generation = 0;
        uint32_t index = 0;
        Char separator = '\0';
        IStringstream stream( _frameId );
        stream >> generation >> separator >> index;

        if( stream.fail() == true || separator != ':' || generation != m_pauseGeneration || this->isPaused() == false )
        {
            *_error = "frame handle is stale";

            return false;
        }

        MapFrames::const_iterator it = m_frames.find( index );

        if( it == m_frames.end() )
        {
            *_error = "frame handle is stale";

            return false;
        }

        pybind::object frame = it->second;
        *_frame = frame;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    jpp::object MCPDebuggerContext::makeFrameJSON( const pybind::object & _frame, uint32_t _index ) const
    {
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();
        PyObject * frameObject = _frame.ptr();
        PyObject * codeValue = kernel->debugger_frame_code( frameObject );
        pybind::object code( kernel, codeValue );
        pybind::object filename = code.get_attr( "co_filename" );
        pybind::object name = code.get_attr( "co_name" );
        const Char * filenameString = filename.is_string() == true ? (const Char *)filename.extract() : "";
        const Char * nameString = name.is_string() == true ? (const Char *)name.extract() : "";
        uint32_t line = kernel->debugger_frame_line( frameObject );
        size_t depthValue = this->getFrameDepth_( frameObject );
        uint32_t depth = (uint32_t)depthValue;
        String frameId = this->makeFrameId_( _index );

        jpp::object result = jpp::make_object();
        result.set( "id", frameId );
        result.set( "filename", filenameString );
        result.set( "name", nameString );
        result.set( "line", line );
        result.set( "depth", depth );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPDebuggerContext::debuggerTrace_( void * _userData, const pybind::debugger_event_t & _event )
    {
        MCPDebuggerContext * context = static_cast<MCPDebuggerContext *>(_userData);
        context->onDebuggerTrace_( _event );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MCPDebuggerContext::getFrameDepth_( PyObject * _frame ) const
    {
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();
        size_t depth = 0;

        for( PyObject * frame = _frame; frame != nullptr; frame = kernel->debugger_frame_back( frame ) )
        {
            ++depth;
        }

        return depth;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPDebuggerContext::updateHooks_()
    {
        bool active = m_pauseRequested.load() == true
            || m_exceptionPolicy != "none"
            || m_stepKind.empty() == false
            || this->isPaused() == true;

        if( active == false )
        {
            for( const Map<String, VectorBreakpoints>::value_type & value : m_breakpoints )
            {
                if( value.second.empty() == false )
                {
                    active = true;

                    break;
                }
            }
        }

        pybind::pybind_debugger_handler_f handler = active == true ? &MCPDebuggerContext::debuggerTrace_ : nullptr;
        void * userData = active == true ? this : nullptr;
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();
        kernel->debugger_set_trace( handler, userData );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPDebuggerContext::shouldPause_( const pybind::debugger_event_t & _event, String * const _reason )
    {
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();

        if( m_pauseRequested.exchange( false ) == true )
        {
            *_reason = "pause";

            return true;
        }

        size_t depth = this->getFrameDepth_( _event.frame );

        if( _event.event == pybind::debugger_event_e::line && m_stepKind.empty() == false )
        {
            bool stop = m_stepKind == "in"
                || (m_stepKind == "over" && depth <= m_stepDepth)
                || (m_stepKind == "out" && depth < m_stepDepth);

            if( stop == true )
            {
                *_reason = "step";
                m_stepKind.clear();

                return true;
            }
        }

        if( _event.event == pybind::debugger_event_e::exception )
        {
            PyObject * previousFrame = kernel->debugger_frame_back( _event.frame );
            bool uncaught = previousFrame == nullptr;
            bool pause = m_exceptionPolicy == "all" || (m_exceptionPolicy == "uncaught" && uncaught == true);

            if( pause == true )
            {
                *_reason = "exception";

                return true;
            }

            return false;
        }

        if( _event.event != pybind::debugger_event_e::line )
        {
            return false;
        }

        PyObject * code = kernel->debugger_frame_code( _event.frame );
        PyObject * filenameObject = kernel->get_attrstring( code, "co_filename" );

        if( filenameObject == nullptr || kernel->string_check( filenameObject ) == false )
        {
            return false;
        }

        const Char * filenameValue = kernel->string_to_char( filenameObject );
        String filename = filenameValue;
        Map<String, VectorBreakpoints>::iterator it = m_breakpoints.find( filename );

        if( it == m_breakpoints.end() )
        {
            return false;
        }

        uint32_t line = kernel->debugger_frame_line( _event.frame );

        for( Breakpoint & breakpoint : it->second )
        {
            if( breakpoint.enabled == false || breakpoint.line != line )
            {
                continue;
            }

            ++breakpoint.hits;

            if( breakpoint.hitTarget != 0 && breakpoint.hits < breakpoint.hitTarget )
            {
                continue;
            }

            if( breakpoint.condition.empty() == false )
            {
                PyObject * globals = kernel->debugger_frame_globals( _event.frame );
                PyObject * locals = kernel->debugger_frame_locals( _event.frame );
                const Char * conditionExpression = breakpoint.condition.c_str();
                PyObject * conditionValue = kernel->eval_string( conditionExpression, globals, locals );
                pybind::object condition( kernel, conditionValue );
                PyObject * conditionObject = condition.ptr();

                if( condition.is_invalid() == true || kernel->is_true( conditionObject ) == false )
                {
                    continue;
                }
            }

            if( breakpoint.logMessage.empty() == false )
            {
                const Char * filenameString = filename.c_str();
                const Char * logMessageString = breakpoint.logMessage.c_str();

                LOGGER_INFO( "mcp", "Python logpoint %s:%u: %s"
                    , filenameString
                    , line
                    , logMessageString
                );

                continue;
            }

            Stringstream stream;
            stream << "breakpoint:" << breakpoint.id;
            String reason = stream.str();
            *_reason = reason;

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPDebuggerContext::onDebuggerTrace_( const pybind::debugger_event_t & _event )
    {
        if( m_callbackActive == true )
        {
            return;
        }

        m_callbackActive = true;
        String reason;
        bool pause = this->shouldPause_( _event, &reason );

        if( pause == true )
        {
            this->enterPause_( _event.frame, reason );
        }

        m_callbackActive = false;
        this->updateHooks_();
    }
    //////////////////////////////////////////////////////////////////////////
    String MCPDebuggerContext::makeFrameId_( uint32_t _index ) const
    {
        Stringstream stream;
        stream << m_pauseGeneration << ':' << _index;
        String frameId = stream.str();

        return frameId;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPDebuggerContext::enterPause_( PyObject * _frame, const String & _reason )
    {
        pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();

        ++m_pauseGeneration;

        if( m_pauseGeneration == 0 )
        {
            ++m_pauseGeneration;
        }

        m_frames.clear();
        uint32_t index = 0;

        for( PyObject * frame = _frame; frame != nullptr; frame = kernel->debugger_frame_back( frame ) )
        {
            pybind::object frameObject( kernel, frame );
            m_frames.emplace( index, frameObject );
            ++index;
        }

        m_pauseReason = _reason;
        m_host->setMCPDebuggerPaused( true );

        while( m_host->isMCPDebuggerPaused() == true && m_host->isMCPConnected() == true )
        {
            m_host->processMCPDebuggerRequests();
            THREAD_SYSTEM()->sleep( 1 );
        }

        m_frames.clear();
        m_pauseReason.clear();
        m_host->setMCPDebuggerPaused( false );
    }
    //////////////////////////////////////////////////////////////////////////
#endif
}
