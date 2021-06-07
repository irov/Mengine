#pragma once

#include <type_traits>

#include <cfloat>

namespace Mengine
{
    namespace Helper
    {
        template<class T>
        constexpr size_t decimalDigitsCount()
        {
            static_assert(std::is_arithmetic_v<T> == true);

            if constexpr( std::is_integral_v<T> == true )
            {
                if constexpr( std::is_signed_v<T> == true )
                {
                    return ((sizeof( T ) * 616) >> 8) + 2;
                }
                else
                {
                    return ((sizeof( T ) * 616) >> 8) + 1;
                }
            }
            else if constexpr( std::is_floating_point_v<T> == true )
            {
                if constexpr( std::is_same_v<T, float> == true )
                {
                    return (FLT_MAX_10_EXP)+10;
                }
                else if constexpr( std::is_same_v<T, double> == true )
                {
                    return (DBL_MAX_10_EXP)+10;
                }
            }
            else
            {
                static_assert(false);
            }
        }
    }
}