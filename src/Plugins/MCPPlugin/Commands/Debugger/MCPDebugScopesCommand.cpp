#include "MCPDebugScopesCommand.h"

#include "../../Contexts/MCPDebuggerContext.h"
#include "../../Contexts/MCPScriptContext.h"

#include "Interface/ScriptProviderServiceInterface.h"

#include "pybind/exception.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPDebugScopesCommand::MCPDebugScopesCommand( MCPDebuggerContext * _debuggerContext, MCPScriptContext * _scriptContext )
        : m_debuggerContext( _debuggerContext )
        , m_scriptContext( _scriptContext )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPDebugScopesCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
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
            PyObject * localsValue = kernel->debugger_frame_locals( frameObject );
            PyObject * globalsValue = kernel->debugger_frame_globals( frameObject );
            PyObject * builtinsValue = kernel->get_builtins();
            pybind::object locals( kernel, localsValue );
            pybind::object globals( kernel, globalsValue );
            pybind::object builtins( kernel, builtinsValue );

            const struct ScopeDesc
            {
                const Char * name;
                pybind::object * value;
            } scopeValues[] = {
                {"locals", &locals},
                {"globals", &globals},
                {"builtins", &builtins}
            };

            jpp::array scopes = jpp::make_array();

            for( const ScopeDesc & scopeValue : scopeValues )
            {
                uint64_t handle = 0;
                bool retained = m_scriptContext->retainObject( *scopeValue.value, &handle, &error );

                if( retained == false )
                {
                    _response->errorMessage = error;

                    return EMCPCommandStatus::FAILURE;
                }

                jpp::object scope = jpp::make_object();
                scope.set( "name", scopeValue.name );
                scope.set( "handle", (int64_t)handle );
                scopes.push_back( scope );
            }

            _response->result.set( "scopes", scopes );

            return EMCPCommandStatus::SUCCESS;
        }
        catch( const pybind::pybind_exception & ex )
        {
            _response->errorMessage = ex.what();

            return EMCPCommandStatus::FAILURE;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPDebugScopesCommand::availableWhilePaused( const jpp::object & _params ) const
    {
        MENGINE_UNUSED( _params );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
