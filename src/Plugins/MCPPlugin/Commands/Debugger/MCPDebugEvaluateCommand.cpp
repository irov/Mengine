#include "MCPDebugEvaluateCommand.h"

#include "../../Contexts/MCPDebuggerContext.h"
#include "../../Contexts/MCPScriptContext.h"

#include "Interface/ScriptProviderServiceInterface.h"

#include "pybind/exception.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPDebugEvaluateCommand::MCPDebugEvaluateCommand( MCPDebuggerContext * _debuggerContext, MCPScriptContext * _scriptContext )
        : m_debuggerContext( _debuggerContext )
        , m_scriptContext( _scriptContext )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPDebugEvaluateCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        try
        {
            const Char * frameId = _request.params.get( "frameId", "" );
            String error;
            pybind::object frame;
            bool resolved = m_debuggerContext->resolveFrame( frameId, &frame, &error );

            if( resolved == false )
            {
                _response->errorCode = "stale_handle";
                _response->errorMessage = error;

                return EMCPCommandStatus::FAILURE;
            }

            pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();
            PyObject * frameObject = frame.ptr();
            PyObject * globals = kernel->debugger_frame_globals( frameObject );
            PyObject * locals = kernel->debugger_frame_locals( frameObject );
            const Char * expression = _request.params.get( "expression", "" );
            PyObject * evaluatedValue = kernel->eval_string( expression, globals, locals );
            pybind::object value( kernel, evaluatedValue );
            kernel->debugger_frame_sync_locals( frameObject );

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
    bool MCPDebugEvaluateCommand::availableWhilePaused( const jpp::object & _params ) const
    {
        MENGINE_UNUSED( _params );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
