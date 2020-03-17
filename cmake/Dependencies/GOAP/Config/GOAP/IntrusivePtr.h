/*
* Copyright (C) 2017-2019, Yuriy Levchenko <irov13@mail.ru>
*
* This software may be modified and distributed under the terms
* of the MIT license.  See the LICENSE file for details.
*/

#pragma once

#include "GOAP/Config.h"

#include "stdex/intrusive_ptr.h"

namespace GOAP
{
    template<class T, class D = void>
    using IntrusivePtr = stdex::intrusive_ptr<T, D>;
}