#pragma once

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
    }
}