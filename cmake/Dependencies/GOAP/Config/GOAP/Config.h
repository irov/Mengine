/*
* Copyright (C) 2017-2019, Yuriy Levchenko <irov13@mail.ru>
*
* This software may be modified and distributed under the terms
* of the MIT license.  See the LICENSE file for details.
*/

#pragma once

#include <stdint.h>
#include <cstddef>

#define GOAP_UNUSED(expr) ((void)(expr))

#include "stdex/allocator.h"

#define GOAP_MALLOC(SIZE) (stdex_malloc(SIZE, "GOAP"))
#define GOAP_FREE(PTR, SIZE) (stdex_free(PTR, "GOAP"))

#include <utility>