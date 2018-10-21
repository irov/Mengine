#pragma once

#include "stdex/stl_allocator.h"

#include <regex>

namespace Mengine
{
    template<class Type, class Allocator = stdex::stl_allocator<std::sub_match<Type> > >
    using MatchResults = std::match_results<Type, Allocator>;
    
    template<class Elem>
    using BasicRegex = std::basic_regex<Elem>;
}