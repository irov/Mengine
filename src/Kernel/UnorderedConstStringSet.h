#pragma once

#include "Kernel/UnorderedSet.h"
#include "Kernel/ConstString.h"
#include "Kernel/Hashgen.h"

namespace Mengine
{
    typedef UnorderedSet<ConstString, Hashgen<ConstString>> UnorderedConstStringSet;
}