#pragma once

#include "Core/String.h"
#include "Core/ConstString.h"

#include "stdex/stl_map.h"

namespace Mengine
{
	typedef stdex::map<ConstString, String> TMapParams;
	typedef stdex::map<ConstString, WString> TMapWParams;
}