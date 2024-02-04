#pragma once

#include "Interface/InputStreamInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/String.h"
#include "Kernel/Data.h"

#include "json/json.hpp"

namespace Mengine
{
    namespace Helper
    {
        js_element_t * createJSON2();

        bool writeJSON2DataCompact( js_element_t * _j, Data * const _data );
    }
}
