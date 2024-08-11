#pragma once

#include "Kernel/Surface.h"

#include "Config/DynamicCast.h"

#if defined(MENGINE_DEBUG)
#   include "Config/TypeTraits.h"
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
            struct reinterpret_surface_cast_void_t
            {
                typedef void * type;
            };
            //////////////////////////////////////////////////////////////////////////
            template<class U>
            struct reinterpret_surface_cast_void_t<const U *>
            {
                typedef const void * type;
            };
        }
        //////////////////////////////////////////////////////////////////////////
#endif
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T reinterpretSurfaceCast( void * _surface )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Surface, T>, "reinterpret surface cast use on non 'Surface' type");

            if( _surface == nullptr )
            {
                return nullptr;
            }

            try
            {
                if( Helper::dynamicCast<typename Detail::reinterpret_surface_cast_void_t<T>::type>( static_cast<T>(_surface) ) == nullptr )
                {
                    throw std::runtime_error( "reinterpret surface cast" );
                }
            }
            catch( const std::exception & )
            {
                throw;
            }
#endif

            return reinterpret_cast<T>(_surface);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T staticSurfaceCast( Surface * _surface )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Surface, T>, "static surface cast use on non 'Surface' type");

            if( _surface == nullptr )
            {
                return nullptr;
            }

            if( Helper::dynamicCast<T>( _surface ) == nullptr )
            {
                throw std::runtime_error( "static surface cast" );
            }
#endif

            return static_cast<T>(_surface);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T staticSurfaceCast( const Surface * _surface )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Surface, T>, "static surface cast use on non 'Surface' type");

            if( _surface == nullptr )
            {
                return nullptr;
            }

            if( Helper::dynamicCast<T>( _surface ) == nullptr )
            {
                throw std::runtime_error( "static surface cast" );
            }
#endif

            return static_cast<T>(_surface);
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicSurfaceCast( Surface * _surface )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Surface, T>, "dynamic surface cast use on non 'Surface' type");
#endif

            T t = Helper::dynamicCast<T>( _surface );

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicSurfaceCast( const Surface * _surface )
        {
#if defined(MENGINE_DEBUG)
            static_assert(TypeTraits::is_base_of<Surface, T>, "dynamic surface cast use on non 'Surface' type");
#endif

            T t = Helper::dynamicCast<T>( _surface );

            return t;
        }
        ////////////////////////////////////////////////////////////////////////
    }
}
