#pragma once

#include <atomic>

namespace Mengine
{
    template<class T>
    using Atomic = std::atomic<T>;
}