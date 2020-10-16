#pragma once

#include "stdex/intrusive_linked.h"

namespace Mengine
{
    template<class T>
    using IntrusiveLinked = stdex::intrusive_linked<T>;
}
