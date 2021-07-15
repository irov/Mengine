#pragma once

#include "Kernel/StlAllocator.h"

#include <regex>

namespace Mengine
{
    using Regex = std::basic_regex<Char>;
    using WRegex = std::basic_regex<WChar>;

    template<class T, class A = StlAllocator<std::sub_match<T>>>
    using MatchResults = std::match_results<T, A>;

    template<class Elem>
    using BasicRegex = std::basic_regex<Elem>;
}