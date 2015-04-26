#	pragma once

#	include "Config/Typedef.h"
#	include "pybind/bindable.hpp"

namespace Menge
{
	class Scriptable
		: public pybind::bindable
	{
	};
}
