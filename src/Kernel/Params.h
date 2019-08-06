#pragma once

#include "Kernel/ConstString.h"

#include "Config/Map.h"
#include "Config/String.h"

namespace Mengine
{
    typedef Map<ConstString, String> MapParams;
    typedef Map<ConstString, WString> MapWParams;
}