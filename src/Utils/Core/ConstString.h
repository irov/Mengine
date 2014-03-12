#   pragma once

#   include "stdex/const_string.h"
#	include "stdex/const_string_holder_local.h"

#	include <vector>

namespace Menge
{
    typedef stdex::const_string ConstString;
	typedef stdex::const_string_holder_local ConstStringHolderLocal;

	typedef std::vector<ConstString> TVectorConstString;		
} 