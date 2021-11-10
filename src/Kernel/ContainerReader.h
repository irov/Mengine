#pragma once

#include "Config/Config.h"

#include "stdex/container_reader.h"

namespace Mengine
{
    template<class T>
    using ContainerReader = stdex::container_reader<T>;
}