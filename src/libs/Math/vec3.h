#	pragma once

/*	
	Vector3 class.
	-	basic operation with vectors(+,-,*,=,==).
	-	dot, reflect, norm, cross.
*/


#	include "config.h"

#	include "vec2.h"

namespace mt
{
	struct vec3f
	{
		static const mt::vec3f zero_v3;

		float x;
		float y;
		float z;

		MATH_INLINE vec3f();
		MATH_INLINE vec3f(float _x, float _y, float _z);
		MATH_INLINE vec3f(const vec3f& _v);

		MATH_INLINE explicit vec3f(const vec2f&	_v, float _z);

		MATH_INLINE vec3f& operator= (const vec3f& _rhs);
	
		MATH_INLINE const float operator[](int i) const;
		MATH_INLINE float& operator[](int i);

		MATH_INLINE vec3f& operator+=(const vec3f&	_rhs);
		MATH_INLINE vec3f& operator-=(const vec3f&	_rhs);
		MATH_INLINE vec3f& operator/=(const float _rhs);
		MATH_INLINE vec3f& operator*=(const float _rhs);
		MATH_INLINE float sqrlength() const;
		MATH_INLINE float length() const;

		MATH_INLINE vec2f & to_vec2f();
		MATH_INLINE const vec2f & to_vec2f() const;

		MATH_INLINE float * buff();
		MATH_INLINE const float * buff() const;
	};

	MATH_INLINE bool operator==(const vec3f& _a, const vec3f& _b);
	MATH_INLINE bool operator!=(const vec3f& _a, const vec3f& _b);

	MATH_INLINE vec3f operator+(const vec3f& _a, const vec3f& _b);
	MATH_INLINE vec3f operator-(const vec3f& _a, const vec3f& _b);
	MATH_INLINE vec3f operator*(const vec3f& _rhs, const float _val);
	MATH_INLINE vec3f operator*(const float _val, const vec3f& _rhs);
	MATH_INLINE vec3f operator/(const vec3f& _rhs, const float _val);
	MATH_INLINE vec3f operator-(vec3f _rhs);

	MATH_INLINE void ident_v3( vec3f& _a );

	MATH_INLINE float length_v3_v3(const vec3f& _a, const vec3f& _b);

	MATH_INLINE bool cmp_v3_v3(const vec3f& _a, const vec3f& _b, float eps = 0.00001f);

	MATH_INLINE void add_v3_v3(vec3f& _out,const vec3f& _a, const vec3f& _b);
	MATH_INLINE void sub_v3_v3(vec3f& _out,const vec3f& _a, const vec3f& _b);
	MATH_INLINE void scale_v3_v3(vec3f& _out, const vec3f& _a, float _val);
	MATH_INLINE void neg_v3(vec3f& _out);

	MATH_INLINE float dot_v3_v3(const vec3f &a, const vec3f &b);
	MATH_INLINE float dot_v3_v3_clamp(const vec3f &a, const vec3f &b);

	MATH_INLINE void reflect_v3_v3(vec3f& _out, const vec3f& _a, const vec3f& _b);
	MATH_INLINE vec3f reflect_v3_v3(const vec3f& _a, const vec3f& _b);

	MATH_INLINE void cross_v3_v3(vec3f& _out, const vec3f& _a, const vec3f& _b);
	MATH_INLINE vec3f cross_v3_v3(const vec3f& _a, const vec3f& _b);

	MATH_INLINE void cross_v3_v3_norm(vec3f& _out, const vec3f& _a, const vec3f& _b);

	MATH_INLINE void norm_v3(vec3f& _out, const vec3f& _rhs);
	MATH_INLINE vec3f norm_v3(const vec3f& _rhs);
	MATH_INLINE void norm_safe_v3(vec3f& out, const vec3f& _rhs, float err = 1e-06);
	MATH_INLINE vec3f norm_safe_v3(const vec3f& _rhs);

	MATH_INLINE float get_axis_angle(const vec3f& dir, int axis);
};

#	if MATH_FORCE_INLINE == 1
#	include "vec3_inline.h"
#	endif