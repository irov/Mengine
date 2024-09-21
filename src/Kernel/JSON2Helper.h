#pragma once

#include "Kernel/Data.h"

#include "json/json.hpp"

namespace Mengine
{
    namespace Helper
    {
        js_element_t * createJSON2();
        void destroyJSON2( js_element_t * j );

        bool writeJSON2DataCompact( js_element_t * _j, Data * const _data );
    }
}
