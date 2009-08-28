#	include "line.h"

namespace mt
{
	bool line_intersect_v2(const mt::vec2f& a0, const mt::vec2f& a1,
			   const mt::vec2f& b0, const mt::vec2f& b1, 
			   mt::vec2f& intersectionPoint) 
	{
		if (a0 == b0 || a0 == b1 || a1 == b0 || a1 == b1) 
		{
			return false;
		}

		float x1 = a0.x; float y1 = a0.y;
		float x2 = a1.x; float y2 = a1.y;
		float x3 = b0.x; float y3 = b0.y;
		float x4 = b1.x; float y4 = b1.y;
		
		if (std::max(x1,x2) < std::min(x3,x4) || std::max(x3,x4) < std::min(x1,x2) ) return false;
		if (std::max(y1,y2) < std::min(y3,y4) || std::max(y3,y4) < std::min(y1,y2) ) return false;
		
		float ua = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3));
		float ub = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3));
		float denom = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
		
		if (denom == 0.0f)
		{
			//lines are too close to parallel to call
			return false;
		}
		ua /= denom;
		ub /= denom;
		
		if ((0 < ua) && (ua < 1) && (0 < ub) && (ub < 1)) 
		{
			intersectionPoint.x = (x1 + ua * (x2 - x1));
			intersectionPoint.y = (y1 + ua * (y2 - y1));
			return true;
		}
		
		return false;
	}
}
