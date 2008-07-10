#	include "line.h"

namespace mt
{
	bool line_intersect_v2(
		const mt::vec2f & begin_,
		const mt::vec2f & end_,
		const mt::vec2f & other_line_begin_,
		const mt::vec2f & other_line_end_,
		mt::vec2f & intersection)
	{
		float denom = ((other_line_end_.y - other_line_begin_.y)*(end_.x - begin_.x)) -
					  ((other_line_end_.x - other_line_begin_.x)*(end_.y - begin_.y));

		float nume_a = ((other_line_end_.x - other_line_begin_.x)*(begin_.y - other_line_begin_.y)) -
					   ((other_line_end_.y - other_line_begin_.y)*(begin_.x - other_line_begin_.x));

		float nume_b = ((end_.x - begin_.x)*(begin_.y - other_line_begin_.y)) -
					   ((end_.y - begin_.y)*(begin_.x - other_line_begin_.x));

		if(denom == 0.0f)
		{
			if(nume_a == 0.0f && nume_b == 0.0f)
			{
				//coincident
	//			assert(0);
				return false;
			}

			//parallel
			return false;
		}

		float ua = nume_a / denom;
		float ub = nume_b / denom;

		if(ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f)
		{
			intersection.x = begin_.x + ua*(end_.x - begin_.x);
			intersection.y = begin_.y + ua*(end_.y - begin_.y);

			return true;
		}

		return false;
	}
}