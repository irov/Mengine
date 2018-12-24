#pragma once

#include "Kernel/Node.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<uint32_t> VectorNodePath;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        NodePtr findUniqueNode( const NodePtr & _root, const VectorNodePath & _nodePath );
    }
}