#	pragma once

/*	
	Vector4 class.
	-	basic operation with vectors(+,-,*,=,==).
	-	dot, norm, homogenize.
*/

#	include "config.h"

#	include "vec3.h"

#	include <assert.h>

namespace mt
{
	struct vec4f
	{
		static const mt::vec4f zero_v4;

		float x;
		float y;
		float z;
		float w;

		MATH_INLINE vec4f();
		MATH_INLINE vec4f(float _x, float _y, float _z, float _w);
		MATH_INLINE vec4f(const vec2f& _v0, const vec2f& _v1);
		MATH_INLINE vec4f(const vec4f& _v);
		MATH_INLINE explicit vec4f(const vec3f& _v);
		MATH_INLINE vec4f(const vec3f& _v, float _w);

		MATH_INLINE vec4f & operator=(const vec4f& _rhs);

		MATH_INLINE float &	operator [](int i);
		MATH_INLINE const float operator [](int i) const;

		MATH_INLINE vec4f & operator+=(const vec4f&	_rhs);
		MATH_INLINE vec4f & operator-=(const vec4f&	_rhs);
		MATH_INLINE vec4f &	operator/=(const float _rhs);
		MATH_INLINE vec4f &	operator*=(const float _rhs);

		MATH_INLINE float sqrlength() const;
		MATH_INLINE float length() const;

		MATH_INLINE const vec3f & to_vec3f() const;

		MATH_INLINE float * buff();
		MATH_INLINE const float * buff() const;
	};

	MATH_INLINE bool operator==(const vec4f& _a, const vec4f& _b);
	MATH_INLINE bool operator!=(const vec4f& _a, const vec4f& _b);

	MATH_INLINE vec4f operator+(const vec4f& _a, const vec4f& _b);
	MATH_INLINE vec4f operator-(const vec4f& _a, const vec4f& _b);
	MATH_INLINE vec4f operator*(const vec4f& _rhs, const float _val);
	MATH_INLINE vec4f operator/(const vec4f& _rhs, const float _val);
	MATH_INLINE vec4f operator-(vec4f _rhs);
	
	MATH_INLINE void ident_v4( vec4f & _rhs );
	MATH_INLINE float length_v4( vec4f _rhs );

	MATH_INLINE void extract_v4_v3( vec3f & _out, const vec4f & _rhs );
	MATH_INLINE const vec3f & cast_v4_v3( const vec4f & _rhs );

	MATH_INLINE void set_v4_v3( vec4f & _out, const vec3f & _rhs );

	MATH_INLINE bool cmp_v4_v4(const vec4f& _a, const vec4f& _b, float eps = 0.00001f);

	MATH_INLINE void add_v4_v3(vec4f& _out, const vec4f& _a, const vec4f& _b);
	MATH_INLINE void add_v4_v4(vec4f& _out, const vec4f& _a, const vec3f& _b);

	MATH_INLINE void sub_v4_v4(vec4f& _out, const vec4f& _a, const vec4f& _b);
	MATH_INLINE void scale_v4_v4(vec4f& _out, const vec4f& _a, float _val);
	MATH_INLINE void neg_v4(vec4f& _out);
	
	MATH_INLINE float dot_v4_v3(const vec4f &a, const vec3f &b);
	MATH_INLINE float dot_v4_v4(const vec4f &a, const vec4f &b);
	
	MATH_INLINE void norm_v4(vec4f& _out, const vec4f& _rhs);
	MATH_INLINE vec4f norm_v4(const vec4f& _rhs);
	MATH_INLINE void norm_safe_v4(vec4f& out, const vec4f& _rhs, float err = 1e-06);
	MATH_INLINE vec4f norm_safe_v4(const vec4f& _rhs);
	
	MATH_INLINE void homogenize_v4(vec4f& _out, const vec4f& _rhs);
	MATH_INLINE vec4f homogenize_v4(const vec4f& _rhs);
};

#	if MATH_FORCE_INLINE == 1
#	include "vec4_inline.h"
#	endif