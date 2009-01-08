#	pragma once

/*	
	Matrix 3x3 class.
	-	basic operation(+,-,*,=,==).
	-	identity, transpose, minor, determinant, adjoint, inverse
*/

#	include "config.h"

#	include "vec3.h"

namespace mt
{ 
	struct mat3f
	{
		vec3f v0;
		vec3f v1;
		vec3f v2;

		MATH_INLINE mat3f();
		MATH_INLINE mat3f(const mat3f&	_rhs);

		MATH_INLINE vec3f& operator[](int i);
		MATH_INLINE const vec3f& operator[](int i)const;

		MATH_INLINE mat3f& operator=(const mat3f& _rhs);

		MATH_INLINE float * buff();
	};

	MATH_INLINE bool operator==(const mat3f& _a, const mat3f& _b);
	MATH_INLINE bool operator!=(const mat3f& _a, const mat3f& _b);
	MATH_INLINE vec3f operator*(const mat3f& _m, const vec3f& _v);
	MATH_INLINE vec3f operator*(const vec3f& _v, const mat3f& _m);
	MATH_INLINE vec2f operator*(const vec2f& _v, const mat3f& _m);
	MATH_INLINE mat3f operator*(const mat3f& _a, const mat3f& _b);
	MATH_INLINE mat3f operator+(const mat3f& _a, const mat3f& _b);
	MATH_INLINE mat3f operator-(const mat3f& _a, const mat3f& _b);
	MATH_INLINE mat3f operator*(const mat3f& _rhs, float _val);

	MATH_INLINE float * buff_m3( const mat3f& _a );

	MATH_INLINE bool cmp_m3_m3(const mat3f& _a, const mat3f& _b);
	
	MATH_INLINE void mul_m3_v3(vec3f& _out, const mat3f& _m, const vec3f& _v);
	MATH_INLINE void mul_v3_m3(vec3f& _out, const vec3f& _v, const mat3f& _m);
	MATH_INLINE void mul_v2_m3(vec2f& _out, const vec2f& _v, const mat3f& _m);
	MATH_INLINE void mul_v2_m3_r(vec2f& _out, const vec2f& _v, const mat3f& _m);

	MATH_INLINE void mul_m3_m3(mat3f& _out, const mat3f& _a, const mat3f& _b);
	MATH_INLINE void add_m3_m3(mat3f& _out, const mat3f& _a, const mat3f& _b);
	MATH_INLINE void sub_m3_m3(mat3f& _out, const mat3f& _a, const mat3f& _b);
	MATH_INLINE void scale_m3_s(mat3f& _out, const mat3f& _rhs, float _val);
	MATH_INLINE void scale_m3_s(mat3f& _out, float _val);

	MATH_INLINE void scale_m3_m3(mat3f& _out, const mat3f& _rhs, const mt::vec3f & _val);
	MATH_INLINE void scale_m3(mat3f& _out, const mt::vec3f &_val);

	MATH_INLINE void transpose_m3(mat3f& _out, const mat3f& _rhs);
	MATH_INLINE mat3f transpose_m3(const mat3f& _rhs);

	MATH_INLINE float det_m3(const mat3f& _rhs);

	MATH_INLINE void inv_m3(mat3f& _out, const mat3f& _rhs);

	MATH_INLINE void ident_m3(mat3f& out);

	MATH_INLINE void get_col_m3(vec3f& out, const mat3f& _rhs, int _index);
	MATH_INLINE vec3f get_col_m3(const mat3f& _rhs, int _index);
	MATH_INLINE void set_col_m3(mat3f& out, const vec3f& _rhs, int _index);

 	MATH_INLINE void set_m3_from_axes(mat3f& _out, const vec3f& _a, const vec3f& _b, const vec3f& _c);
	MATH_INLINE mat3f set_m3_from_axes(const vec3f& _a, const vec3f& _b, const vec3f& _c);
}

#	if MATH_FORCE_INLINE == 1
#	include "mat3_inline.h"
#	endif