/*	
	Vector2 class.
	-	basic operation with vectors(+,-,*,=,==).
	-	dot, reflect, norm, perp.
*/
#pragma once

#include "math.h"
#include <assert.h>

namespace mt
{
	struct vec2f
	{
		union
		{
			struct
			{
				float x,y;
			};
			struct
			{
				float u,v;
			};
			float m[2];
		};

		vec2f();
		vec2f(float _x, float _y);
		vec2f(const vec2f&	_v);
		vec2f& operator=(const vec2f& _rhs);

		const float operator[](int i) const;
		float&		operator[](int i);

		vec2f&		operator+=(const vec2f&	_rhs);
		vec2f&		operator-=(const vec2f&	_rhs);
		vec2f&		operator/=(const float _rhs);
		vec2f&		operator*=(const float _rhs);

		float sqrlength()	const;
		float length()	const;
	};

	bool	operator==(const vec2f& _a, const vec2f& _b);
	bool	operator!=(const vec2f& _a, const vec2f& _b);
	vec2f	operator+(const vec2f& _a, const vec2f& _b);
	vec2f	operator-(const vec2f& _a, const vec2f& _b);
	vec2f	operator*(const vec2f& _rhs, float _val);
	vec2f	operator*(float _val, const vec2f& _rhs );
	vec2f	operator/(const vec2f& _rhs, float _val);
	vec2f	operator-(vec2f _rhs);

	float	length_v2_v2(const vec2f& _a, const vec2f& _b);

	bool	cmp_v2_v2(const vec2f& _a, const vec2f& _b, float eps = 0.00001f);

	void	add_v2_v2(vec2f& _out,const vec2f& _a, const vec2f& _b);
	void	sub_v2_v2(vec2f& _out,const vec2f& _a, const vec2f& _b);
	void	scale_v2_v2(vec2f& _out, const vec2f& _a, float _val);
	void	neg_v2(vec2f& _out);

	float	dot_v2_v2(const vec2f& a, const vec2f& b);

	float	norm_v2_f(vec2f& _out, const vec2f& _rhs);
	void	norm_v2(vec2f& _out, const vec2f& _rhs);
	vec2f	norm_v2(const vec2f& _rhs);
	void	norm_safe_v2(vec2f& out, const vec2f& _rhs, float err = 1e-06);
	vec2f	norm_safe_v2(const vec2f& _rhs);

	void	perp(vec2f&	out, const vec2f& in);
	vec2f	perp(const vec2f&	in);

	float	is_left_v2(const vec2f& P0, const vec2f& P1, const vec2f& P2);
	float	pseudo_cross_v2(const vec2f& a, const vec2f& b);

	void	project_v2_v2(const vec2f& a, const vec2f& b, vec2f& result);

	struct	mat2f;

	int clip_segment_to_line_v2(mt::vec2f vOut[2], const vec2f vIn[2],
		const vec2f& normal, float offset, char clipEdge);

	void compute_incident_edge_v2(mt::vec2f c[2], const vec2f& h, const mt::vec2f& pos,
		float angle, const vec2f& normal);

};