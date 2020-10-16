#pragma once

#include "stdex/intrusive_list.h"

namespace Mengine
{
    template<class T>
    using IntrusiveList = stdex::intrusive_list<T>;
}
