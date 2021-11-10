#pragma once

#include "Config/Config.h"

#include "stdex/container_writer.h"

namespace Mengine
{
    template<class T>
    using ContainerWriter = stdex::container_writer<T>;
}