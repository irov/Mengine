#pragma once

#ifndef METABUF_MALLOC
#	include "../../stdex/include/stdex/allocator.h"
#	define METABUF_MALLOC(S) stdex_malloc(S, "metabuf")
#endif

#ifndef METABUF_FREE
#	include "../../stdex/include/stdex/allocator.h"
#	define METABUF_FREE(B, S) stdex_free(B, "metabuf")
#endif 

#ifndef METABUF_MEMCPY
#	include <algorithm>
#	define METABUF_MEMCPY(Dest, Src, Size) std::copy((const uint8_t *)Src, (const uint8_t *)Src + Size, (uint8_t *)Dest)
#endif

#include <vector>
#include <exception>

namespace Metabuf
{
    template<class Type, class Allocator = std::allocator<Type>>
    using Vector = std::vector<Type, Allocator>;

    using Exception = std::exception;
}