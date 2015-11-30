#	pragma once

#	include "config.h"

#	include "vec3.h"

namespace mt
{ 
	struct mat3f
	{
		typedef vec3f value_type;
		typedef float element_type;

		vec3f v0;
		vec3f v1;
		vec3f v2;

		MENGINE_MATH_METHOD_INLINE mat3f();
		MENGINE_MATH_METHOD_INLINE mat3f(const mat3f&	_rhs);

		MENGINE_MATH_METHOD_INLINE vec3f& operator[](size_t i);
		MENGINE_MATH_METHOD_INLINE const vec3f& operator[](size_t i)const;

		MENGINE_MATH_METHOD_INLINE mat3f& operator=(const mat3f& _rhs);

		MENGINE_MATH_METHOD_INLINE float * buff();
	};

	MENGINE_MATH_FUNCTION_INLINE bool operator==(const mat3f& _a, const mat3f& _b);
	MENGINE_MATH_FUNCTION_INLINE bool operator!=(const mat3f& _a, const mat3f& _b);
	MENGINE_MATH_FUNCTION_INLINE vec3f operator*(const mat3f& _m, const vec3f& _v);
	MENGINE_MATH_FUNCTION_INLINE vec3f operator*(const vec3f& _v, const mat3f& _m);
	MENGINE_MATH_FUNCTION_INLINE vec2f operator*(const vec2f& _v, const mat3f& _m);
	MENGINE_MATH_FUNCTION_INLINE mat3f operator*(const mat3f& _a, const mat3f& _b);
	MENGINE_MATH_FUNCTION_INLINE mat3f operator+(const mat3f& _a, const mat3f& _b);
	MENGINE_MATH_FUNCTION_INLINE mat3f operator-(const mat3f& _a, const mat3f& _b);
	MENGINE_MATH_FUNCTION_INLINE mat3f operator*(const mat3f& _rhs, float _val);

	MENGINE_MATH_FUNCTION_INLINE float * buff_m3( const mat3f& _a );

	MENGINE_MATH_FUNCTION_INLINE bool cmp_m3_m3(const mat3f& _a, const mat3f& _b);
	
	MENGINE_MATH_FUNCTION_INLINE void mul_m3_v3(vec3f& _out, const mat3f& _m, const vec3f& _v);
    MENGINE_MATH_FUNCTION_INLINE void mul_m3_v2(vec2f& _out, const mat3f& _m, const vec2f& _v);
	MENGINE_MATH_FUNCTION_INLINE void mul_v3_m3(vec3f& _out, const vec3f& _v, const mat3f& _m);
	MENGINE_MATH_FUNCTION_INLINE void mul_v2_m3(vec2f& _out, const vec2f& _v, const mat3f& _m);
	MENGINE_MATH_FUNCTION_INLINE void mul_v2_m3_r(vec2f& _out, const vec2f& _v, const mat3f& _m);

	MENGINE_MATH_FUNCTION_INLINE void mul_m3_m3(mat3f& _out, const mat3f& _a, const mat3f& _b);
	MENGINE_MATH_FUNCTION_INLINE void add_m3_m3(mat3f& _out, const mat3f& _a, const mat3f& _b);
	MENGINE_MATH_FUNCTION_INLINE void sub_m3_m3(mat3f& _out, const mat3f& _a, const mat3f& _b);
	MENGINE_MATH_FUNCTION_INLINE void scale_m3_s(mat3f& _out, const mat3f& _rhs, float _val);
	MENGINE_MATH_FUNCTION_INLINE void scale_m3_s(mat3f& _out, float _val);

	MENGINE_MATH_FUNCTION_INLINE void scale_m3_m3(mat3f& _out, const mat3f& _rhs, const mt::vec3f & _val);
	MENGINE_MATH_FUNCTION_INLINE void scale_m3(mat3f& _out, const mt::vec3f &_val);

	MENGINE_MATH_FUNCTION_INLINE void transpose_m3(mat3f& _out, const mat3f& _rhs);
	MENGINE_MATH_FUNCTION_INLINE mat3f transpose_m3(const mat3f& _rhs);

	MENGINE_MATH_FUNCTION_INLINE float det_m3(const mat3f& _rhs);

	MENGINE_MATH_FUNCTION_INLINE void inv_m3(mat3f& _out, const mat3f& _rhs);

	MENGINE_MATH_FUNCTION_INLINE void ident_m3(mat3f& out);

	MENGINE_MATH_FUNCTION_INLINE void get_col_m3(vec3f& out, const mat3f& _rhs, int _index);
	MENGINE_MATH_FUNCTION_INLINE vec3f get_col_m3(const mat3f& _rhs, int _index);
	MENGINE_MATH_FUNCTION_INLINE void set_col_m3(mat3f& out, const vec3f& _rhs, int _index);

 	MENGINE_MATH_FUNCTION_INLINE void set_m3_from_axes(mat3f& _out, const vec3f& _a, const vec3f& _b, const vec3f& _c);
	MENGINE_MATH_FUNCTION_INLINE mat3f set_m3_from_axes(const vec3f& _a, const vec3f& _b, const vec3f& _c);

	MENGINE_MATH_FUNCTION_INLINE void make_rotate_x_axis_m3(mat3f & _out, float _angle);
	MENGINE_MATH_FUNCTION_INLINE void make_rotate_y_axis_m3(mat3f & _out, float _angle);
	MENGINE_MATH_FUNCTION_INLINE void make_rotate_z_axis_m3(mat3f & _out, float _angle);

	MENGINE_MATH_FUNCTION_INLINE void translate_m3(mat3f & _out, const mat3f & _in, const vec2f & _offset);
}

#	if MENGINE_MATH_FORCE_INLINE == 1
#	include "mat3_inline.h"
#	endif
