/*
* Copyright (C) 2017-2019, Yuriy Levchenko <irov13@mail.ru>
*
* This software may be modified and distributed under the terms
* of the MIT license.  See the LICENSE file for details.
*/

#pragma once

#include "GOAP/StlAllocator.h"

#include <vector>

namespace GOAP
{
    template<class T, class A = StlAllocator<T>>
    using Vector = std::vector<T, A>;
}
