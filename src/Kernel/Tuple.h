#pragma once

#include <tuple>

namespace Mengine
{
    template<class ... Args>
    using Tuple = std::tuple<Args...>;
}