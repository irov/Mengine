#pragma once

#include "Kernel/UnorderedMap.h"
#include "Kernel/ConstString.h"
#include "Kernel/Hashgen.h"

namespace Mengine
{
    template<class T>
    using UnorderedConstStringMap = UnorderedMap<ConstString, T, Hashgen<ConstString>>;
}