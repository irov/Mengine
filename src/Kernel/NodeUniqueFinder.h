#pragma once

#include "Kernel/Node.h"
#include "Kernel/Vector.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<UniqueId> VectorNodePath;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        NodePtr findUniqueNode( const NodePtr & _root, const VectorNodePath & _nodePath );
        void findPathToRootFromParent( const NodePtr & _root, const NodePtr & _currentNode, VectorNodePath * _nodePath );
    }
    //////////////////////////////////////////////////////////////////////////
}