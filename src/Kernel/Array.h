#pragma once

#include <array>

namespace Mengine
{
    template<class T, size_t Count>
    using Array = std::array<T, Count>;
}