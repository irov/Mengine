#	pragma once

#	include "math/quat.h"

namespace Menge
{
	class MotionModifier
	{
	public:
		virtual const mt::vec3f & getPosition() = 0;
		virtual const mt::quatf & getOrientation() = 0;
	};
};