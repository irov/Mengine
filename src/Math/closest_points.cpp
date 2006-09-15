#include "closest_points.h"

namespace mt
{
	void closest_pt_point_seg(mt::vec2f c, mt::vec2f a, mt::vec2f b, mt::vec2f &d)
	{
		mt::vec2f ab(b-a);

		float t = dot_v2_v2(c-a, ab);
		if (t <= 0.0f)
		{
			t = 0.0f;
			d = a;
		} else 
		{
			float denom = dot_v2_v2(ab, ab); 
			if (t >= denom) 
			{
				t = 1.0f;
				d = b;
			} 
			else 
			{
				t = t / denom;
				d = a + ab * t;
			}
		}
	}
}