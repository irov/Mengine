#include "MCPScriptHandlersCommand.h"

#include "../../Contexts/MCPHandlerRegistry.h"

#include "Kernel/VectorConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPScriptHandlersCommand::MCPScriptHandlersCommand( const MCPHandlerRegistry * _registry )
        : m_registry( _registry )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MCPScriptHandlersCommand::~MCPScriptHandlersCommand()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPScriptHandlersCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        MENGINE_UNUSED( _request );

        VectorConstString names;
        m_registry->getHandlerNames( &names );

        jpp::array handlers = jpp::make_array();

        for( const ConstString & name : names )
        {
            const Char * nameString = name.c_str();

            handlers.push_back( nameString );
        }

        _response->result.set( "handlers", handlers );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPScriptHandlersCommand::availableWhilePaused( const jpp::object & _params ) const
    {
        MENGINE_UNUSED( _params );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
