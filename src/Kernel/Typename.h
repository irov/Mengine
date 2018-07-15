#pragma once

#include "stdex/typename.h"

namespace Mengine
{
    template<class T>
    using Typename = stdex::type_name<T>;
}