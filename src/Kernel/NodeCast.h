#pragma once

#include "Kernel/Node.h"

#ifdef MENGINE_DEBUG
#   include <type_traits>
#   include <stdexcept>
#endif

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_DEBUG
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
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Node, std::remove_pointer_t<T>>, "reinterpret node cast use on non 'Nodeable' type");

            if( _node == nullptr )
            {
                return nullptr;
            }

            if( dynamic_cast<typename Detail::reinterpret_node_cast_void_t<T>::type>(static_cast<T>(_node)) == nullptr )
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
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Node, std::remove_pointer_t<T>>, "static node cast use on non 'Nodeable' type");

            if( _node == nullptr )
            {
                return nullptr;
            }

            if( dynamic_cast<T>(_node) == nullptr )
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
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Node, std::remove_pointer_t<T>>, "static node cast use on non 'Nodeable' type");

            if( _node == nullptr )
            {
                return nullptr;
            }

            if( dynamic_cast<T>(_node) == nullptr )
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
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Node, std::remove_pointer_t<typename T::value_type>>, "static node cast use on non 'Nodeable' type");

            if( _node == nullptr )
            {
                return nullptr;
            }

            if( stdex::intrusive_dynamic_cast<T>(_node) == nullptr )
            {
                throw std::runtime_error( "static node cast" );
            }
#endif

            T t = stdex::intrusive_static_cast<T>(_node);

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicNodeCast( Node * _node )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Node, std::remove_pointer_t<T>>, "dynamic node cast use on non 'Nodeable' type");
#endif

            T t = dynamic_cast<T>(_node);

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicNodeCast( const Node * _node )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Node, std::remove_pointer_t<T>>, "dynamic node cast use on non 'Nodeable' type");
#endif

            T t = dynamic_cast<T>(_node);

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicNodeCast( const NodePtr & _node )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Node, std::remove_pointer_t<typename T::value_type>>, "dynamic node cast use on non 'Nodeable' type");
#endif

            T t = stdex::intrusive_dynamic_cast<T>(_node);

            return t;
        }
    }
}