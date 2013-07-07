#	pragma once

#	ifdef _MSC_VER
#	pragma warning(disable:4250) // remove inherits dominance
#	endif

#	include "Config.h"

#   include <stddef.h>

namespace Menge
{
	typedef void * WindowHandle;
    
#   ifdef MENGE_COMPILER_MSVC
	typedef unsigned __int64 uint64;
	typedef unsigned __int32 uint32;
	typedef unsigned __int16 uint16;
	typedef unsigned __int8 uint8;
#   else
	typedef unsigned long long uint64;
	typedef unsigned int uint32;
	typedef unsigned short uint16;
	typedef unsigned char uint8;
#   endif	   
}
