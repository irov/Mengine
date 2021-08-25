#pragma once

#include "Kernel/Surface.h"

#ifdef MENGINE_DEBUG
#   include <type_traits>
#   include <stdexcept>
#endif

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
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Surface, std::remove_pointer_t<T>>, "reinterpret surface cast use on non 'Surfacable' type");

            if( _surface == nullptr )
            {
                return nullptr;
            }

            try
            {
                if( dynamic_cast<typename Detail::reinterpret_surface_cast_void_t<T>::type>(static_cast<T>(_surface)) == nullptr )
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
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Surface, std::remove_pointer_t<T>>, "static surface cast use on non 'Surfacable' type");

            if( _surface == nullptr )
            {
                return nullptr;
            }

            if( dynamic_cast<T>(_surface) == nullptr )
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
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Surface, std::remove_pointer_t<T>>, "static surface cast use on non 'Surfacable' type");

            if( _surface == nullptr )
            {
                return nullptr;
            }

            if( dynamic_cast<T>(_surface) == nullptr )
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
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Surface, std::remove_pointer_t<T>>, "dynamic surface cast use on non 'Surfacable' type");
#endif

            T t = dynamic_cast<T>(_surface);

            return t;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T dynamicSurfaceCast( const Surface * _surface )
        {
#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<Surface, std::remove_pointer_t<T>>, "dynamic surface cast use on non 'Surfacable' type");
#endif

            T t = dynamic_cast<T>(_surface);

            return t;
        }
    }
}
