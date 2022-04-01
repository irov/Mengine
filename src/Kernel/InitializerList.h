#pragma once

#include "Config/Config.h"

#include <initializer_list>

namespace Mengine
{
    template<class C>
    using InitializeList = std::initializer_list<C>;
}