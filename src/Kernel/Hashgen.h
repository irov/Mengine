#pragma once

#include "Kernel/HashType.h"

namespace Mengine
{
    template<class C>
    struct Hashgen;

    template<>
    struct Hashgen<uint32_t>
    {
        HashType operator()( uint32_t _value ) const
        {
            return (HashType)_value;
        }
    };

    template<>
    struct Hashgen<uint64_t>
    {
        HashType operator()( uint64_t _value ) const
        {
            return (HashType)_value;
        }
    };
}