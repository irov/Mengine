#include "MCPScriptEvalCommand.h"

#include "../../Contexts/MCPDebuggerContext.h"
#include "../../Contexts/MCPScriptContext.h"

#include "Interface/ScriptProviderServiceInterface.h"

#include "pybind/exception.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPScriptEvalCommand::MCPScriptEvalCommand( MCPScriptContext * _scriptContext, MCPDebuggerContext * _debuggerContext )
        : m_scriptContext( _scriptContext )
        , m_debuggerContext( _debuggerContext )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPScriptEvalCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        try
        {
            const Char * expression = _request.params.get( "expression", "" );
            const Char * scopeValue = _request.params.get( "scope", "module" );
            const Char * module = _request.params.get( "module", "" );
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

            PyObject * evaluatedValue = kernel->eval_string( expression, globals, locals );
            pybind::object value( kernel, evaluatedValue );
            jpp::object serialized;
            bool successful = m_scriptContext->scriptToJSON( value, 2, 100, 0, &serialized, &error );

            if( successful == false )
            {
                _response->errorMessage = error;

                return EMCPCommandStatus::FAILURE;
            }

            _response->result.set( "value", serialized );

            return EMCPCommandStatus::SUCCESS;
        }
        catch( const pybind::pybind_exception & ex )
        {
            _response->errorMessage = ex.what();

            return EMCPCommandStatus::FAILURE;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPScriptEvalCommand::availableWhilePaused( const jpp::object & _params ) const
    {
        const Char * scopeValue = _params.get( "scope", "module" );
        String scope = scopeValue;
        bool available = scope == "frame";

        return available;
    }
    //////////////////////////////////////////////////////////////////////////
}
