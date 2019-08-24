/*
* Copyright (C) 2017-2019, Yuriy Levchenko <irov13@mail.ru>
*
* This software may be modified and distributed under the terms
* of the MIT license.  See the LICENSE file for details.
*/

#pragma once

#include <array>

namespace GOAP
{
    template<class T, size_t Count>
    using Array = std::array<T, Count>;
}
