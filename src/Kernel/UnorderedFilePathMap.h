#pragma once

#include "Kernel/UnorderedMap.h"
#include "Kernel/FilePath.h"
#include "Kernel/Hashgen.h"

namespace Mengine
{
    template<class T>
    using UnorderedFilePathMap = UnorderedMap<FilePath, T, Hashgen<FilePath>>;
}