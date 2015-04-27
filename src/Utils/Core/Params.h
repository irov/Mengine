#	pragma once

#	include "Core/String.h"
#	include "Core/ConstString.h"

#	include <stdex/stl_map.h>

namespace Menge
{
	typedef stdex::map<ConstString, WString> TMapParams;
}