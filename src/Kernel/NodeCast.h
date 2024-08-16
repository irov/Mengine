#pragma once

#include "Kernel/Node.h"

#include "Config/DynamicCast.h"

#if defined(MENGINE_DEBUG)
#   include "Config/TypeTraits.h"
#endif

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DEBUG)
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            template<class T>
            struct reinterpret_node_cast_void_t
            {
                typedef void * type;
            };
            //////////////////////////////////////////////////////////////////////////
            template<class U>
            struct reinterpret_node_cast_void_t<const U *>
            {
                typedef const void * type;
            };
        }
        //////////////////////////////////////////////////////////////////////////
#endif
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T reinterpretNodeCast( void * _node )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Node, T>, "reinterpret node cast use on non 'Nodeable' type");

            if( _node == nullptr )
            {
                return nullptr;
            }

            if( Helper::dynamicCast<typename Detail::reinterpret_node_cast_void_t<T>::type>( static_cast<T>(_node) ) == nullptr )
            {
                throw std::runtime_error( "reinterpret node cast" );
            }
#endif

            return reinterpret_cast<T>(_node);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T staticNodeCast( Node * _node )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Node, T>, "static node cast use on non 'Nodeable' type");

            if( _node == nullptr )
            {
                return nullptr;
            }

            if( Helper::dynamicCast<T>( _node ) == nullptr )
            {
                throw std::runtime_error( "static node cast" );
            }
#endif

            return static_cast<T>(_node);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T staticNodeCast( const Node * _node )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Node, T>, "static node cast use on non 'Nodeable' type");

            if( _node == nullptr )
            {
                return nullptr;
            }

            if( Helper::dynamicCast<T>( _node ) == nullptr )
            {
                throw std::runtime_error( "static node cast" );
            }
#endif

            T t = static_cast<T>(_node);

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T staticNodeCast( const NodePtr & _node )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Node, typename T::value_type>, "static node cast use on non 'Nodeable' type");

            if( _node == nullptr )
            {
                return nullptr;
            }

            if( T::dynamic_from( _node ) == nullptr )
            {
                throw std::runtime_error( "static node cast" );
            }
#endif

            T t = T::from( _node );

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicNodeCast( Node * _node )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Node, T>, "dynamic node cast use on non 'Nodeable' type");
#endif

            T t = Helper::dynamicCast<T>( _node );

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicNodeCast( const Node * _node )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Node, T>, "dynamic node cast use on non 'Nodeable' type");
#endif

            T t = Helper::dynamicCast<T>( _node );

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicNodeCast( const NodePtr & _node )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Node, typename T::value_type>, "dynamic node cast use on non 'Nodeable' type");
#endif

            T t = T::dynamic_from( _node );

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}