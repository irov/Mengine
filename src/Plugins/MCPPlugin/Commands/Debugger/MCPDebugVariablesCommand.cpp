#include "MCPDebugVariablesCommand.h"

#include "../../Contexts/MCPScriptContext.h"

#include "pybind/exception.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPDebugVariablesCommand::MCPDebugVariablesCommand( MCPScriptContext * _scriptContext )
        : m_scriptContext( _scriptContext )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPDebugVariablesCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        try
        {
            int64_t signedHandle = _request.params.get( "handle", MENGINE_INT64_C(0) );
            uint64_t handle = signedHandle > 0 ? (uint64_t)signedHandle : 0;
            String error;
            pybind::object object;
            bool resolved = m_scriptContext->resolveHandle( handle, &object, &error );

            if( resolved == false )
            {
                _response->errorCode = "stale_handle";
                _response->errorMessage = error;

                return EMCPCommandStatus::FAILURE;
            }

            uint32_t start = _request.params.get( "start", MENGINE_UINT32_C(0) );
            uint32_t count = _request.params.get( "count", MENGINE_UINT32_C(100) );
            jpp::object value;
            bool serialized = m_scriptContext->scriptToJSON( object, 1, count, start, &value, &error );

            if( serialized == false )
            {
                _response->errorMessage = error;

                return EMCPCommandStatus::FAILURE;
            }

            _response->result.set( "start", start );
            _response->result.set( "count", count );
            _response->result.set( "value", value );

            return EMCPCommandStatus::SUCCESS;
        }
        catch( const pybind::pybind_exception & ex )
        {
            _response->errorMessage = ex.what();

            return EMCPCommandStatus::FAILURE;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPDebugVariablesCommand::availableWhilePaused( const jpp::object & _params ) const
    {
        MENGINE_UNUSED( _params );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
