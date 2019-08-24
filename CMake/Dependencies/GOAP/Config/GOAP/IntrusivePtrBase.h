/*
* Copyright (C) 2017-2019, Yuriy Levchenko <irov13@mail.ru>
*
* This software may be modified and distributed under the terms
* of the MIT license.  See the LICENSE file for details.
*/

#pragma once

#include "GOAP/Config.h"

namespace GOAP
{
    class IntrusivePtrBase
    {
    public:
        virtual uint32_t incref() = 0;
        virtual void decref() = 0;
        virtual uint32_t getrefcount() const = 0;
    };
}