#pragma once

#include "Kernel/StlAllocator.h"

#include <regex>

namespace Mengine
{
    using Regex = std::regex;
    using WRegex = std::wregex;

    template<class T, class A = StlAllocator<std::sub_match<T>>>
    using MatchResults = std::match_results<T, A>;

    template<class Elem>
    using BasicRegex = std::basic_regex<Elem>;
}