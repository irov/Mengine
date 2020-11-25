#pragma once

#include <bitset>

namespace Mengine
{
    template<size_t Bits>
    using Bitset = std::bitset<Bits>;
}