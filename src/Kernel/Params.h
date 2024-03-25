#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/Map.h"
#include "Kernel/String.h"

namespace Mengine
{
    typedef Map<ConstString, String> MapParams;
    typedef Map<ConstString, WString> MapWParams;
    typedef Map<ConstString, ConstString> MapCParams;
}