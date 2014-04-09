#	pragma once

#	include "stdex/allocator.h"

#	define METABUF_MALLOC(S) stdex_malloc(S)
#	define METABUF_FREE(B, S) stdex_free(B)

#	include "stdex/memorycopy.h"

#	define METABUF_MEMCPY(Dest, Src, Size) stdex::memorycopy(Dest, Src, Size)