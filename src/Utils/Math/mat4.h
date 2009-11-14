/*	
	Matrix 4x4 class.
	-	basic operation(+,-,*,=,==).
	-	identity, transpose, minor, determinant, adjoint, inverse
*/
#pragma once

#include "vec4.h"
#include "mat3.h"
#include "math.h"

namespace mt
{
	struct mat4f
	{	
		vec4f v0;
		vec4f v1;
		vec4f v2;
		vec4f v3;

		MATH_INLINE mat4f();
		MATH_INLINE mat4f(const mat4f& _rhs);
	
		MATH_INLINE vec4f& operator [] (int i);
		MATH_INLINE const vec4f& operator [] (int i)const;

		MATH_INLINE mat4f& operator = (const mat4f& _rhs);

		MATH_INLINE float * buff();
		MATH_INLINE const float * buff() const;
	};

	MATH_INLINE bool operator==(const mat4f& _a, const mat4f& _b);
	MATH_INLINE bool operator!=(const mat4f& _a, const mat4f& _b);

	MATH_INLINE vec3f operator*(const mat4f& m, const vec3f& v);
	MATH_INLINE vec3f operator*(const vec3f& v, const mat4f& m);

	MATH_INLINE vec4f operator*(const mat4f& m, const vec4f& v);
	MATH_INLINE vec4f operator*(const vec4f& v, const mat4f& m);
	
	MATH_INLINE mat4f operator*(const mat4f& _a, const mat4f& _b);
	MATH_INLINE mat4f operator*(const mat4f& _a, const mat3f& _b);

	MATH_INLINE mat4f operator+(const mat4f& _a, const mat4f& _b);
	MATH_INLINE mat4f operator-(const mat4f& _a, const mat4f& _b);

	MATH_INLINE bool cmp_m4_m4(const mat4f& _a, const mat4f& _b);

	MATH_INLINE void mul_m4_v3(vec3f& _out, const mat4f& _m,const vec3f& _v);
	MATH_INLINE void mul_v3_m4(vec3f& _out, const vec3f& _v,const mat4f& _m);
	MATH_INLINE void mul_v2_m4(vec2f& _out, const vec2f& _v,const mat4f& _m);
	
	MATH_INLINE void mul_m4_v4(vec4f& _out, const mat4f& _m,const vec4f& _v);
	MATH_INLINE void mul_v4_m4(vec4f& _out, const vec4f& _v, const mat4f& _m);
	MATH_INLINE void mul_m4_m4(mat4f& _out, const mat4f& _a, const mat4f& _b);
	MATH_INLINE void mul_m4_m3(mat4f& _out, const mat4f& _a, const mat3f& _b);
	MATH_INLINE void add_m4_m4(mat4f& _out, const mat4f& _a, const mat4f& _b);
	MATH_INLINE void sub_m4_m4(mat4f& _out, const mat4f& _a, const mat4f& _b);

	MATH_INLINE void scale_m4_m4(mat4f& _out, const mat4f& _rhs, const mt::vec4f & _val);
	MATH_INLINE void scale_m4(mat4f& _out, const mt::vec4f &_val);

	MATH_INLINE void scale_rotate_m4_m4(mat4f& _out, const mat4f& _rhs, const mt::vec3f & _val);
	MATH_INLINE void scale_rotate_m4(mat4f& _out, const mt::vec3f &_val);

	MATH_INLINE void ident_m4(mat4f& _out);

	MATH_INLINE void get_col_m4(vec4f& out, const mat4f& _rhs, int _index);
	MATH_INLINE vec4f get_col_m4(const mat4f& _rhs, int _index);
	MATH_INLINE void set_col_m4(mat4f& out, const vec4f& _rhs, int _index);

	MATH_INLINE void transpose_m4(mat4f& _out, const mat4f& _rhs);
	MATH_INLINE mat4f transpose_m4(const mat4f& _rhs);

	MATH_INLINE void extract_m4_m3(mat3f& _out, const mat4f& _rhs);

	MATH_INLINE float minor_m4(const mat4f& _rhs, const int r0, const int r1, const int r2, const int c0, const int c1, const int c2);

	MATH_INLINE float det_m4(const mat4f& _rhs);

	MATH_INLINE void adj_m4(mat4f& _out, const mat4f& _rhs);
	MATH_INLINE mat4f adj_m4(const mat4f& _rhs);

	MATH_INLINE void inv_m4(mat4f& _out, const mat4f& _rhs);
	MATH_INLINE mat4f inv_m4(const mat4f& _rhs);

	MATH_INLINE void rotate_axis_m4(mat4f &out, const vec3f &u, float degrees);
	MATH_INLINE void make_projection_m4( mat4f &_out , float _fov, float _aspect, float _zn, float _zf );
}

#	if MATH_FORCE_INLINE == 1
#	include "mat4_inline.h"
#	endif
