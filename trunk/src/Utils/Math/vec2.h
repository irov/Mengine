#	pragma once

/*	
	Vector2 class.
	-	basic operation with vectors(+,-,*,=,==).
	-	dot, reflect, norm, perp.
*/

#	include "config.h"

namespace mt
{
	struct vec2f
	{
		static const mt::vec2f zero_v2;

		float x;
		float y;

		MATH_INLINE vec2f();
		MATH_INLINE vec2f(float _x, float _y);
		MATH_INLINE vec2f(const vec2f&	_v);
		MATH_INLINE vec2f& operator=(const vec2f& _rhs);

		MATH_INLINE const float operator[](int i) const;
		MATH_INLINE float & operator[](int i);

		MATH_INLINE vec2f& operator+=(const vec2f&	_rhs);
		MATH_INLINE vec2f& operator-=(const vec2f&	_rhs);
		MATH_INLINE vec2f& operator/=(const float _rhs);
		MATH_INLINE vec2f& operator*=(const float _rhs);

		MATH_INLINE float sqrlength() const;
		MATH_INLINE float length() const;

		MATH_INLINE float * buff();
		MATH_INLINE const float * buff() const;
	};

	MATH_INLINE bool operator==(const vec2f& _a, const vec2f& _b);
	MATH_INLINE bool operator!=(const vec2f& _a, const vec2f& _b);
	MATH_INLINE vec2f operator+(const vec2f& _a, const vec2f& _b);
	MATH_INLINE vec2f operator-(const vec2f& _a, const vec2f& _b);
	MATH_INLINE vec2f operator*(const vec2f& _rhs, float _val);
	MATH_INLINE vec2f operator*(float _val, const vec2f& _rhs );
	MATH_INLINE vec2f operator/(const vec2f& _rhs, float _val);
	MATH_INLINE vec2f operator-(vec2f _rhs);

	MATH_INLINE void ident_v2(vec2f& _a);
	MATH_INLINE float length_v2(const vec2f& _a);
	MATH_INLINE float length_v2_v2(const vec2f& _a, const vec2f& _b);

	MATH_INLINE bool cmp_v2_v2(const vec2f& _a, const vec2f& _b, float eps = 0.00001f);

	MATH_INLINE void add_v2_v2(vec2f& _out,const vec2f& _a, const vec2f& _b);
	MATH_INLINE void sub_v2_v2(vec2f& _out,const vec2f& _a, const vec2f& _b);
	MATH_INLINE void scale_v2_v2(vec2f& _out, const vec2f& _a, float _val);
	MATH_INLINE void neg_v2(vec2f& _out);

	MATH_INLINE float dot_v2_v2(const vec2f& a, const vec2f& b);

	MATH_INLINE float norm_v2_f(vec2f& _out, const vec2f& _rhs);
	MATH_INLINE void norm_v2(vec2f& _out, const vec2f& _rhs);
	MATH_INLINE vec2f norm_v2(const vec2f& _rhs);
	MATH_INLINE void norm_safe_v2(vec2f& out, const vec2f& _rhs, float err = 1e-06);
	MATH_INLINE vec2f norm_safe_v2(const vec2f& _rhs);

	MATH_INLINE void perp(vec2f& out, const vec2f& in);
	MATH_INLINE vec2f perp(const vec2f&	in);

	MATH_INLINE float is_left_v2(const vec2f& P0, const vec2f& P1, const vec2f& P2);
	MATH_INLINE float pseudo_cross_v2(const vec2f& a, const vec2f& b);

	MATH_INLINE void project_v2_v2(const vec2f& a, const vec2f& b, vec2f& result);

	MATH_INLINE vec2f slerp_v2_v2(const vec2f& a, const vec2f& b, float t);

	MATH_INLINE void rotate_v2(vec2f& _out, float _angle);
};

#	if MATH_FORCE_INLINE == 1
#	include "vec2_inline.h"
#	endif
