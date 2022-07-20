#pragma once

#include "Config/Config.h"

#include <functional>

namespace Mengine
{
    template<class T = void()>
    using Lambda = std::function<T>;
}