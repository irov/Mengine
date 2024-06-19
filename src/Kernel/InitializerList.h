#pragma once

#include "Config/Config.h"

#include <initializer_list>

namespace Mengine
{
    template<class C>
    using InitializerList = std::initializer_list<C>;
}