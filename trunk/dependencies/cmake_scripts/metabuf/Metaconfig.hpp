#	pragma once

#	ifndef METABUF_MALLOC
#	include "stdex/allocator.h"
#	define METABUF_MALLOC(S) stdex_malloc(S)
#	endif

#	ifndef METABUF_FREE
#	include "stdex/allocator.h"
#	define METABUF_FREE(B, S) stdex_free(B)
#	endif 

#	ifndef METABUF_MEMCPY
#	include "stdex/memorycopy.h"
#	define METABUF_MEMCPY(Dest, Src, Size) stdex::memorycopy(Dest, Src, Size)
#	endif