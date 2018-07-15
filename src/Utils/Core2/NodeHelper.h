#pragma once

#include "Kernel/Node.h"

namespace Mengine
{
    namespace Helper
    {
        template<class T>
        T findParentNodeT( Node * _node )
        {
            Node * parent = _node->getParent();

            while( parent != nullptr )
            {
                if( dynamic_cast<T>(parent) != nullptr )
                {
                    return static_cast<T>(parent);
                }

                parent = parent->getParent();
            }

            return nullptr;
        }
    }
}