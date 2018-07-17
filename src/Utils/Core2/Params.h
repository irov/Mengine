#pragma once

#include "Core/String.h"
#include "Core/ConstString.h"

#include "Config/Map.h"

namespace Mengine
{
	typedef Map<ConstString, String> TMapParams;
	typedef Map<ConstString, WString> TMapWParams;
}