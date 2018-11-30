#include "NodeUniqueFinder.h"

namespace Mengine
{
    NodePtr foundUniqueNode( const NodePtr & _root, const VectorNodePath & _nodePath )
    {
        NodePtr finder = _root;

        for( uint32_t id : _nodePath )
        {
            NodePtr child_finder;
            finder->foreachChildrenUnslugBreak( [&child_finder, id]( const NodePtr & _child )
            {
                uint32_t childUID = _child->getUniqueIdentity();

                if( childUID != id )
                {
                    return true;
                }

                child_finder = _child;

                return false;
            } );

            if( child_finder == nullptr )
            {
                return nullptr;
            }

            finder = child_finder;
        }

        return finder;
    }
}