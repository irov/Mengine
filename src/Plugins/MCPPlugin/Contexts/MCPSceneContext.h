#pragma once

#include "Kernel/Map.h"
#include "Kernel/Node.h"
#include "Kernel/String.h"

#include "jpp/jpp.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPSceneContext
    {
    public:
        MCPSceneContext();

        void finalize();
        void refreshGeneration();
        void clearNodes();

        uint32_t getGeneration() const;
        size_t getNodeCount() const;
        const Map<UniqueId, NodePtr> & getNodes() const;

        String makeNodeHandle( const NodePtr & _node ) const;
        NodePtr resolveNode( const Char * _handle, String * const _error ) const;
        jpp::object makeNodePropertiesJSON( const NodePtr & _node ) const;
        jpp::object makeNodeJSON( const NodePtr & _node, uint32_t _depth, uint32_t _maxDepth, bool _includeDisabled );

    protected:
        UniqueId m_sceneIdentity;
        uint32_t m_sceneGeneration;
        Map<UniqueId, NodePtr> m_sceneNodes;
    };
    //////////////////////////////////////////////////////////////////////////
}
