#include "MCPDebugSetVariableCommand.h"

#include "../../Contexts/MCPDebuggerContext.h"
#include "../../Contexts/MCPScriptContext.h"

#include "Interface/ScriptProviderServiceInterface.h"

#include "pybind/exception.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPDebugSetVariableCommand::MCPDebugSetVariableCommand( MCPDebuggerContext * _debuggerContext, MCPScriptContext * _scriptContext )
        : m_debuggerContext( _debuggerContext )
        , m_scriptContext( _scriptContext )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPDebugSetVariableCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
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

            jpp::object valueObject;

            if( _request.params.exist( "value", &valueObject ) == false )
            {
                _response->errorMessage = "value is required";

                return EMCPCommandStatus::FAILURE;
            }

            pybind::object value;
            bool converted = m_scriptContext->jsonToScript( valueObject, &value, &error );

            if( converted == false )
            {
                _response->errorMessage = error;

                return EMCPCommandStatus::FAILURE;
            }

            const Char * name = _request.params.get( "name", "" );

            if( name[0] == '\0' )
            {
                _response->errorMessage = "failed to set debugger variable";

                return EMCPCommandStatus::FAILURE;
            }

            const Char * scopeValue = _request.params.get( "scope", "locals" );
            String scopeName = scopeValue;
            pybind::debugger_scope_e scope = scopeName == "globals"
                ? pybind::debugger_scope_e::globals
                : pybind::debugger_scope_e::locals;
            pybind::kernel_interface * kernel = SCRIPTPROVIDER_SERVICE()->getKernel();
            PyObject * frameObject = frame.ptr();
            PyObject * scriptValue = value.ptr();
            bool successful = kernel->debugger_frame_set( frameObject, scope, name, scriptValue );

            if( successful == false )
            {
                _response->errorMessage = "failed to set debugger variable";

                return EMCPCommandStatus::FAILURE;
            }

            _response->result.set( "set", true );
            _response->result.set( "name", name );

            return EMCPCommandStatus::SUCCESS;
        }
        catch( const pybind::pybind_exception & ex )
        {
            _response->errorMessage = ex.what();

            return EMCPCommandStatus::FAILURE;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPDebugSetVariableCommand::availableWhilePaused( const jpp::object & _params ) const
    {
        MENGINE_UNUSED( _params );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
