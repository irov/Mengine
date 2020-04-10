#pragma once

#include <variant>

namespace Mengine
{
    template<class ... Types>
    using Variant = std::variant<Types ...>;
}
