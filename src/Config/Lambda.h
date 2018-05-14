#pragma once

#include <functional>

namespace Mengine
{
    template<class T = void()>
    using Lambda = std::function<T>;
}