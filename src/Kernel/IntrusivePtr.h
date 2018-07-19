#pragma once

#include "stdex/intrusive_ptr.h"

namespace Mengine
{
    template<class T>
    using IntrusivePtr = stdex::intrusive_ptr<T>;
}
