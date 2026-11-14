#pragma once

#include "Kernel/Node.h"
#include "Kernel/String.h"
#include "Kernel/Vector.h"

#include "jpp/jpp.hpp"
#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<NodePtr> VectorMCPNodes;
    //////////////////////////////////////////////////////////////////////////
    class MCPSceneContext
    {
    public:
        MCPSceneContext();

        void finalize();
        void refreshGeneration();

        uint32_t getGeneration() const;

        String makeNodeHandle( const NodePtr & _node ) const;
        NodePtr resolveNode( const Char * _handle, String * const _error ) const;
        bool findNodes( const jpp::object & _query, uint32_t _limit, VectorMCPNodes * const _nodes, String * const _error ) const;
        bool getNodeScreenPoint( const NodePtr & _node, mt::vec2f * const _point ) const;
        jpp::object makeNodePropertiesJSON( const NodePtr & _node ) const;
        jpp::object makeNodeJSON( const NodePtr & _node, uint32_t _depth, uint32_t _maxDepth, bool _includeDisabled, uint32_t * const _nodeCount ) const;

    protected:
        void findNodes_( const NodePtr & _node, const String & _path, const ConstString & _name, const ConstString & _type, const String & _queryPath, uint32_t _limit, VectorMCPNodes * const _nodes ) const;
        void makeNodePath_( const NodePtr & _node, String * const _path ) const;

    protected:
        UniqueId m_sceneIdentity;
        uint32_t m_sceneGeneration;
    };
    //////////////////////////////////////////////////////////////////////////
}
