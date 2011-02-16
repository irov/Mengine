#	include "ConstString.h"

namespace Menge
{
#	if defined(MENGE_CONST_STRING)
	ConstString ConstString::none("");
#	endif
}