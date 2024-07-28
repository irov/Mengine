#pragma once

#include "Config/Config.h"

namespace Mengine
{
    namespace TypeTraits
    {
        //////////////////////////////////////////////////////////////////////////
        template <class B, class D>
        constexpr bool is_base_of = std::is_base_of_v<B, std::remove_pointer_t<D>>;
        //////////////////////////////////////////////////////////////////////////
    }
}