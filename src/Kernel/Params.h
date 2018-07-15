#pragma once

#include "Kernel/String.h"
#include "Kernel/ConstString.h"

#include "Config/Map.h"

namespace Mengine
{
	typedef Map<ConstString, String> TMapParams;
	typedef Map<ConstString, WString> TMapWParams;
}