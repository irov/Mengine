#pragma once

#include "Config/Config.h"

#include <type_traits>

namespace Mengine
{
    namespace TypeTraits
    {
        using std::decay_t;

        //////////////////////////////////////////////////////////////////////////
        template <class B, class D>
        MENGINE_CONSTEXPR bool is_base_of = std::is_base_of_v<B, std::remove_pointer_t<D>>;
        //////////////////////////////////////////////////////////////////////////
    }
}