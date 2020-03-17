#pragma once

#include "Kernel/String.h"
#include "Kernel/Regex.h"

namespace Mengine
{
    typedef MatchResults<String::const_iterator> StringMatchResults;
    typedef BasicRegex<Char> StringRegex;
}