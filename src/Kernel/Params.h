#pragma once

#include "Kernel/String.h"
#include "Kernel/ConstString.h"

#include "Config/Map.h"

namespace Mengine
{
    typedef Map<ConstString, String> MapParams;
    typedef Map<ConstString, WString> MapWParams;
}