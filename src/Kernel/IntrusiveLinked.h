#pragma once

#include "Config/Config.h"

#include "stdex/intrusive_linked.h"

namespace Mengine
{
    template<class T>
    using IntrusiveLinked = stdex::intrusive_linked<T>;
}
