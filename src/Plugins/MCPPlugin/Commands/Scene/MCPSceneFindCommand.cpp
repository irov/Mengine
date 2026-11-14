#include "MCPSceneFindCommand.h"

#include "../../Contexts/MCPSceneContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPSceneFindCommand::MCPSceneFindCommand( MCPSceneContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPSceneFindCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        m_context->refreshGeneration();

        jpp::object query;
        if( _request.params.exist( "query", &query ) == false )
        {
            query = jpp::make_object();
        }

        if( query.is_type_object() == false )
        {
            _response->errorMessage = "scene query must be an object";

            return EMCPCommandStatus::FAILURE;
        }

        uint32_t limit = _request.params.get( "limit", MENGINE_UINT32_C(100) );

        VectorMCPNodes foundNodes;
        String error;
        if( m_context->findNodes( query, limit, &foundNodes, &error ) == false )
        {
            _response->errorMessage = error;

            return EMCPCommandStatus::FAILURE;
        }

        jpp::array nodes = jpp::make_array();

        for( const NodePtr & node : foundNodes )
        {
            jpp::object nodeProperties = m_context->makeNodePropertiesJSON( node );
            nodes.push_back( nodeProperties );
        }

        uint32_t generation = m_context->getGeneration();
        size_t countValue = nodes.size();
        uint32_t count = (uint32_t)countValue;
        _response->result.set( "generation", generation );
        _response->result.set( "nodes", nodes );
        _response->result.set( "count", count );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
}
