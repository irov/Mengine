#include "NodeUniqueFinder.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        NodePtr findUniqueNode( const NodePtr & _root, const VectorNodePath & _nodePath )
        {
            NodePtr finder = _root;

            for( uint32_t id : _nodePath )
            {
                NodePtr child_finder = finder->findUniqueChild( id );

                if( child_finder == nullptr )
                {
                    return nullptr;
                }

                finder = child_finder;
            }

            return finder;
        }
        //////////////////////////////////////////////////////////////////////////
        void findPathToRootFromParent( const NodePtr & _root, const NodePtr & _currentNode, VectorNodePath * _nodePath )
        {
            UniqueId currentNodeId = _currentNode->getUniqueIdentity();
            UniqueId rootNodeId = _root->getUniqueIdentity();

            _nodePath->emplace_back( currentNodeId );

            if( currentNodeId == rootNodeId )
            {
                return;
            }

            Node * parentNode = nullptr;

            while( currentNodeId != rootNodeId )
            {
                parentNode = _currentNode->getParent();

                currentNodeId = parentNode->getUniqueIdentity();

                _nodePath->emplace_back( currentNodeId );
            }
        }
        //////////////////////////////////////////////////////////////////////////
    }
}