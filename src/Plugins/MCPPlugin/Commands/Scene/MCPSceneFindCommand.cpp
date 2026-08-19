#include "MCPSceneFindCommand.h"

#include "../../Contexts/MCPSceneContext.h"

#include "Interface/SceneServiceInterface.h"

#include "Kernel/ConstStringHelper.h"

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
        m_context->clearNodes();

        const ScenePtr & scene = SCENE_SERVICE()->getCurrentScene();

        if( scene == nullptr )
        {
            _response->errorMessage = "there is no current scene";

            return EMCPCommandStatus::FAILURE;
        }

        jpp::object root = m_context->makeNodeJSON( scene, 0, 256, true );
        MENGINE_UNUSED( root );

        jpp::object query;

        if( _request.params.exist( "query", &query ) == false || query.is_type_object() == false )
        {
            query = jpp::make_object();
        }

        const Char * name = query.get( "name", "" );
        const Char * type = query.get( "type", "" );
        ConstString queryName = Helper::stringizeString( name );
        ConstString queryType = Helper::stringizeString( type );
        uint32_t limit = _request.params.get( "limit", MENGINE_UINT32_C(100) );
        jpp::array nodes = jpp::make_array();
        const Map<UniqueId, NodePtr> & sceneNodes = m_context->getNodes();

        for( const Map<UniqueId, NodePtr>::value_type & value : sceneNodes )
        {
            const NodePtr & node = value.second;
            const ConstString & nodeName = node->getName();
            const ConstString & nodeType = Helper::getFactorableType( node );

            if( queryName.empty() == false && queryName != nodeName )
            {
                continue;
            }

            if( queryType.empty() == false && queryType != nodeType )
            {
                continue;
            }

            jpp::object nodeProperties = m_context->makeNodePropertiesJSON( node );
            nodes.push_back( nodeProperties );

            if( nodes.size() >= limit )
            {
                break;
            }
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
