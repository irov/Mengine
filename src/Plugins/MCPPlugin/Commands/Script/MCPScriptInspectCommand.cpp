#include "MCPScriptInspectCommand.h"

#include "../../Contexts/MCPScriptContext.h"

#include "pybind/exception.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPScriptInspectCommand::MCPScriptInspectCommand( MCPScriptContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPScriptInspectCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        jpp::object target;

        if( _request.params.exist( "target", &target ) == false )
        {
            _response->errorMessage = "missing script target";

            return EMCPCommandStatus::FAILURE;
        }

        try
        {
            String error;
            pybind::object object;
            bool resolved = m_context->resolveTarget( target, &object, &error );

            if( resolved == false )
            {
                _response->errorMessage = error;

                return EMCPCommandStatus::FAILURE;
            }

            uint32_t depth = _request.params.get( "depth", MENGINE_UINT32_C(2) );
            uint32_t maxItems = _request.params.get( "maxItems", MENGINE_UINT32_C(100) );
            jpp::object value;
            bool serialized = m_context->scriptToJSON( object, depth, maxItems, 0, &value, &error );

            if( serialized == false )
            {
                _response->errorMessage = error;

                return EMCPCommandStatus::FAILURE;
            }

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
}
