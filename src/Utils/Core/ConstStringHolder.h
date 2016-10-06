#   pragma once

#   include "Factory/FactorableLinked.h"

#   include "stdex/const_string2_holder.h"

namespace Menge
{
    class ConstStringHolder
        : public FactorableLinked
        , public stdex::const_string2_holder
    {
    };
} 