#pragma once

#include "Kernel/StlAllocator.h"

#include <regex>

namespace Mengine
{
    template<class T, class A = StlAllocator<std::sub_match<T>>>
    using MatchResults = std::match_results<T, A>;

    template<class Elem>
    using BasicRegex = std::basic_regex<Elem>;

    namespace Regex
    {
        using std::regex_search;
        using std::sub_match;
    }
}