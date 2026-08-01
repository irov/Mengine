#include "MCPSceneSnapshotCommand.h"

#include "../../Contexts/MCPSceneContext.h"

#include "Interface/SceneServiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPSceneSnapshotCommand::MCPSceneSnapshotCommand( MCPSceneContext * _context )
        : m_context( _context )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPSceneSnapshotCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        m_context->refreshGeneration();
        m_context->clearNodes();

        const ScenePtr & scene = SCENE_SERVICE()->getCurrentScene();

        if( scene == nullptr )
        {
            _response->errorMessage = "there is no current scene";

            return EMCPCommandStatus::FAILURE;
        }

        bool includeDisabled = _request.params.get( "includeDisabled", true );
        uint32_t maxDepth = _request.params.get( "maxDepth", MENGINE_UINT32_C(64) );
        uint32_t generation = m_context->getGeneration();
        jpp::object root = m_context->makeNodeJSON( scene, 0, maxDepth, includeDisabled );
        size_t nodeCountValue = m_context->getNodeCount();
        uint32_t nodeCount = (uint32_t)nodeCountValue;

        _response->result.set( "generation", generation );
        _response->result.set( "root", root );
        _response->result.set( "nodeCount", nodeCount );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
}
