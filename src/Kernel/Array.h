#pragma once

#include <array>
#include <cstddef>

namespace Mengine
{
    template<class T, std::size_t Count>
    using Array = std::array<T, Count>;
}
