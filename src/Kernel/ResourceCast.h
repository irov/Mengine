#pragma once

#include "Kernel/Resource.h"

#include "Config/DynamicCast.h"

#if defined(MENGINE_DEBUG)
#   include "Config/TypeTraits.h"
#   include "Config/StdException.h"
#endif

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
            struct reinterpret_resource_cast_void_t
            {
                typedef void * type;
            };
            //////////////////////////////////////////////////////////////////////////
            template<class U>
            struct reinterpret_resource_cast_void_t<const U *>
            {
                typedef const void * type;
            };
        }
        //////////////////////////////////////////////////////////////////////////
#endif
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T reinterpretResourceCast( void * _resource )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Resource, T>, "reinterpret resource cast use on non 'Resourcable' type");

            if( _resource == nullptr )
            {
                return nullptr;
            }

            if( Helper::dynamicCast<typename Detail::reinterpret_resource_cast_void_t<T>::type>( static_cast<T>(_resource) ) == nullptr )
            {
                throw StdException::runtime_error( "reinterpret resource cast" );
            }
#endif

            return reinterpret_cast<T>(_resource);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T staticResourceCast( Resource * _resource )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Resource, T>, "static resource cast use on non 'Resourcable' type");

            if( _resource == nullptr )
            {
                return nullptr;
            }

            if( Helper::dynamicCast<T>( _resource ) == nullptr )
            {
                throw StdException::runtime_error( "static resource cast" );
            }
#endif

            return static_cast<T>(_resource);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T staticResourceCast( const Resource * _resource )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Resource, T>, "static resource cast use on non 'Resourcable' type");

            if( _resource == nullptr )
            {
                return nullptr;
            }

            if( Helper::dynamicCast<T>( _resource ) == nullptr )
            {
                throw StdException::runtime_error( "static resource cast" );
            }
#endif

            return static_cast<T>(_resource);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T staticResourceCast( const ResourcePtr & _resource )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Resource, typename T::value_type>, "static resource cast use on non 'Resourcable' type");

            if( _resource == nullptr )
            {
                return nullptr;
            }

            if( T::dynamic_from( _resource ) == nullptr )
            {
                throw StdException::runtime_error( "static resource cast" );
            }
#endif

            T t = T::from( _resource );

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicResourceCast( Resource * _resource )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Resource, T>, "dynamic resource cast use on non 'Resourcable' type");
#endif

            T t = Helper::dynamicCast<T>( _resource );

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicResourceCast( const Resource * _resource )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Resource, T>, "dynamic resource cast use on non 'Resourcable' type");
#endif

            T t = Helper::dynamicCast<T>( _resource );

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicResourceCast( const ResourcePtr & _resource )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Resource, typename T::value_type>, "dynamic resource cast use on non 'Resourcable' type");
#endif

            T t = T::dynamic_from( _resource );

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}