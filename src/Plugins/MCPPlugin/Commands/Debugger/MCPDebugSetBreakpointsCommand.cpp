#include "MCPDebugSetBreakpointsCommand.h"

#include "../../Contexts/MCPDebuggerContext.h"

#include "Config/StdUtility.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPDebugSetBreakpointsCommand::MCPDebugSetBreakpointsCommand( MCPDebuggerContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPDebugSetBreakpointsCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        const Char * filenameValue = _request.params.get( "filename", "" );
        jpp::object values;

        if( filenameValue[0] == '\0' || _request.params.exist( "breakpoints", &values ) == false || values.is_type_array() == false )
        {
            _response->errorMessage = "filename and breakpoints array are required";

            return EMCPCommandStatus::FAILURE;
        }

        MCPDebuggerContext::VectorBreakpoints breakpoints;
        jpp::array verified = jpp::make_array();
        jpp::array valuesArray( values );

        for( const jpp::object & value : valuesArray )
        {
            MCPDebuggerContext::Breakpoint breakpoint;
            breakpoint.id = m_context->allocateBreakpointId();
            breakpoint.line = value.get( "line", MENGINE_UINT32_C(0) );
            breakpoint.enabled = value.get( "enabled", true );
            breakpoint.condition = value.get( "condition", "" );
            breakpoint.hitTarget = value.get( "hitCount", MENGINE_UINT32_C(0) );
            breakpoint.hits = 0;
            breakpoint.logMessage = value.get( "logMessage", "" );

            if( breakpoint.line == 0 )
            {
                _response->errorMessage = "breakpoint line must be positive";

                return EMCPCommandStatus::FAILURE;
            }

            jpp::object item = jpp::make_object();
            item.set( "id", breakpoint.id );
            item.set( "line", breakpoint.line );
            item.set( "enabled", breakpoint.enabled );
            verified.push_back( item );
            breakpoints.emplace_back( StdUtility::move( breakpoint ) );
        }

        String filename = filenameValue;
        m_context->setBreakpoints( filename, StdUtility::move( breakpoints ) );
        _response->result.set( "filename", filename );
        _response->result.set( "breakpoints", verified );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPDebugSetBreakpointsCommand::availableWhilePaused( const jpp::object & _params ) const
    {
        MENGINE_UNUSED( _params );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
