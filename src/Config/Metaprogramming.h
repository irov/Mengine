#pragma once

#include "Config/Char.h"

#include <type_traits>

namespace Mengine
{
    namespace mpl
    {
        template <class T, class A, size_t N, size_t... Is>
        std::array<T, N> array_cast( const std::array<A, N> & arr, std::index_sequence<Is...> )
        {
            return std::array<T, N>{T{arr[Is]}...};
        }

        template <class T, class A, size_t N>
        std::array<T, N> array_cast( const std::array<A, N> & arr )
        {
            return array_cast<T>(arr, std::make_index_sequence<N>{});
        }

        template<class T>
        T decltype_string( T t );

        const Char * decltype_string( const Char * t );

        // SFINAE has method c_str
        template <typename T>
        class has_c_str
        {
            typedef char one;
            struct two
            {
                char x[2];
            };

            template <typename C> static one test( decltype(&C::c_str) );
            template <typename C> static two test( ... );

        public:
            enum
            {
                value = sizeof( test<T>( 0 ) ) == sizeof( char )
            };
        };
    }
}