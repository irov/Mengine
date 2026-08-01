#include "MCPDebugStackCommand.h"

#include "../../Contexts/MCPDebuggerContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPDebugStackCommand::MCPDebugStackCommand( MCPDebuggerContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPDebugStackCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        MENGINE_UNUSED( _request );

        if( m_context->isPaused() == false )
        {
            _response->errorMessage = "runtime is not paused";

            return EMCPCommandStatus::FAILURE;
        }

        jpp::array frames = jpp::make_array();
        const MCPDebuggerContext::MapFrames & contextFrames = m_context->getFrames();

        for( const MCPDebuggerContext::MapFrames::value_type & value : contextFrames )
        {
            jpp::object frame = m_context->makeFrameJSON( value.second, value.first );
            frames.push_back( frame );
        }

        uint32_t generation = m_context->getPauseGeneration();
        const String & reason = m_context->getPauseReason();
        _response->result.set( "generation", generation );
        _response->result.set( "reason", reason );
        _response->result.set( "frames", frames );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPDebugStackCommand::availableWhilePaused( const jpp::object & _params ) const
    {
        MENGINE_UNUSED( _params );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
