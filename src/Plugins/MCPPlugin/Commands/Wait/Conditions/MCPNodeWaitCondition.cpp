#include "MCPNodeWaitCondition.h"

#include "../../../Contexts/MCPSceneContext.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPNodeWaitCondition::MCPNodeWaitCondition( MCPSceneContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPNodeWaitCondition::evaluate( const MCPWaitConditionRequest & _request, MCPWaitConditionResponse * const _response )
    {
        m_context->refreshGeneration();

        jpp::object query;
        if( _request.condition.exist( "query", &query ) == false )
        {
            query = jpp::make_object();
        }

        if( query.is_type_object() == false )
        {
            _response->errorMessage = "scene query must be an object";

            return false;
        }

        VectorMCPNodes nodes;
        String error;
        if( m_context->findNodes( query, 1, &nodes, &error ) == false )
        {
            _response->errorMessage = error;

            return false;
        }

        size_t countValue = nodes.size();
        uint32_t count = (uint32_t)countValue;

        _response->satisfied = count != 0;
        _response->details.set( "count", count );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
