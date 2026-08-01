#include "MCPScriptExecCommand.h"

#include "../../Contexts/MCPDebuggerContext.h"
#include "../../Contexts/MCPScriptContext.h"

#include "Interface/ScriptProviderServiceInterface.h"

#include "pybind/exception.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPScriptExecCommand::MCPScriptExecCommand( MCPScriptContext * _scriptContext, MCPDebuggerContext * _debuggerContext )
        : m_scriptContext( _scriptContext )
        , m_debuggerContext( _debuggerContext )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPScriptExecCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        try
        {
            const Char * code = _request.params.get( "code", "" );
            const Char * scopeValue = _request.params.get( "scope", "module" );
            const Char * module = _request.params.get( "module", "" );
            const Char * filename = _request.params.get( "filename", "<mcp>" );
            String scope = scopeValue;
            pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();
            pybind::object frame;
            PyObject * globals = nullptr;
            PyObject * locals = nullptr;
            String error;

            if( scope == "frame" )
            {
                const Char * frameId = _request.params.get( "frameId", "" );
                bool resolved = m_debuggerContext->resolveFrame( frameId, &frame, &error );

                if( resolved == false )
                {
                    _response->errorCode = "stale_handle";
                    _response->errorMessage = error;

                    return EMCPCommandStatus::FAILURE;
                }

                PyObject * frameObject = frame.ptr();
                globals = kernel->debugger_frame_globals( frameObject );
                locals = kernel->debugger_frame_locals( frameObject );
            }
            else
            {
                const Char * scopeString = scope.c_str();
                globals = m_scriptContext->getScope( scopeString, module, &error );
                locals = globals;
            }

            if( globals == nullptr )
            {
                _response->errorMessage = error;

                return EMCPCommandStatus::FAILURE;
            }

            PyObject * executedValue = kernel->exec_source( code, filename, globals, locals );
            pybind::object value( kernel, executedValue );

            if( value.is_invalid() == true )
            {
                _response->errorMessage = "script execution failed";

                return EMCPCommandStatus::FAILURE;
            }

            if( frame.is_invalid() == false )
            {
                PyObject * frameObject = frame.ptr();
                kernel->debugger_frame_sync_locals( frameObject );
            }

            _response->result.set( "executed", true );
            _response->result.set( "filename", filename );

            return EMCPCommandStatus::SUCCESS;
        }
        catch( const pybind::pybind_exception & ex )
        {
            _response->errorMessage = ex.what();

            return EMCPCommandStatus::FAILURE;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPScriptExecCommand::availableWhilePaused( const jpp::object & _params ) const
    {
        const Char * scopeValue = _params.get( "scope", "module" );
        String scope = scopeValue;
        bool available = scope == "frame";

        return available;
    }
    //////////////////////////////////////////////////////////////////////////
}
