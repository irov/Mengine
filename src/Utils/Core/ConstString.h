#   pragma once

#   include "stdex/const_string.h"
#	include "stdex/const_string_holder_local.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
    typedef stdex::const_string ConstString;
	typedef stdex::const_string_holder_local ConstStringHolderLocal;

	typedef stdex::vector<ConstString> TVectorConstString;
} 