#pragma once

#include "Config/String.h"
#include "Config/Regex.h"

namespace Mengine
{
    typedef MatchResults<String::const_iterator> StringMatchResults;
    typedef BasicRegex<Char> StringRegex;
}