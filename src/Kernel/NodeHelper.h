#pragma once

#include "Kernel/Node.h"

#include "Config/DynamicCast.h"

#if defined(MENGINE_DEBUG)
#   include "Config/TypeTraits.h"
#endif

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T findParentNodeT( Node * _node )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Node, T>, "find parent node use on non 'Node' type");
#endif

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
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Node, T>, "find parent node use on non 'Node' type");
#endif

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