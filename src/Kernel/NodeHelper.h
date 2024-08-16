#pragma once

#include "Kernel/Node.h"

#include "Config/DynamicCast.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T findParentNodeT( Node * _node )
        {
            Node * parent = _node->getParent();

            while( parent != nullptr )
            {
                if( Helper::dynamicCast<T>( parent ) != nullptr )
                {
                    return static_cast<T>(parent);
                }

                parent = parent->getParent();
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T findParentNodeT( const Node * _node )
        {
            const Node * parent = _node->getParent();

            while( parent != nullptr )
            {
                if( Helper::dynamicCast<T>( parent ) != nullptr )
                {
                    return static_cast<T>(parent);
                }

                parent = parent->getParent();
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}