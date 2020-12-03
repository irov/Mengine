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
    }
}