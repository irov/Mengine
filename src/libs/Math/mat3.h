/*	
	Matrix 3x3 class.
	-	basic operation(+,-,*,=,==).
	-	identity, transpose, minor, determinant, adjoint, inverse
*/
#pragma once

#include "vec3.h"

namespace mt
{ 
	struct mat3f
	{
		union 
		{
			struct
			{
				vec3f v[3]; 
			};
			struct
			{
				vec3f v0, v1, v2; 
			};
			float m[9];
		};

		mat3f();
		mat3f(const mat3f&	_rhs);
		mat3f(const vec3f&	_v0, const vec3f&	_v1, const vec3f&	_v2);

		vec3f& operator[](int i);
		const vec3f& operator[](int i)const;

		mat3f& operator=(const mat3f& _rhs);
	};

	bool	operator==(const mat3f& _a, const mat3f& _b);
	bool	operator!=(const mat3f& _a, const mat3f& _b);
	vec3f	operator*(const mat3f& _m, const vec3f& _v);
	vec3f	operator*(const vec3f& _v, const mat3f& _m);
	mat3f	operator*(const mat3f& _a, const mat3f& _b);
	mat3f	operator+(const mat3f& _a, const mat3f& _b);
	mat3f	operator-(const mat3f& _a, const mat3f& _b);
	mat3f	operator*(const mat3f& _rhs, float _val);

	bool	cmp_m3_m3(const mat3f& _a, const mat3f& _b);
	
	void	mul_m3_v3(vec3f& _out, const mat3f& _m, const vec3f& _v);
	void	mul_v3_m3(vec3f& _out, const vec3f& _v, const mat3f& _m);
	void	mul_v2_m3(vec2f& _out, const vec2f& _v, const mat3f& _m);
	void	mul_v2_m3_r(vec2f& _out, const vec2f& _v, const mat3f& _m);

	void	mul_m3_m3(mat3f& _out, const mat3f& _a, const mat3f& _b);
	void	add_m3_m3(mat3f& _out, const mat3f& _a, const mat3f& _b);
	void	sub_m3_m3(mat3f& _out, const mat3f& _a, const mat3f& _b);
	void	scale_m3_s(mat3f& _out, const mat3f& _rhs, float _val);
	void	scale_m3_s(mat3f& _out, float _val);

	void	scale_m3_m3(mat3f& _out, const mat3f& _rhs, const mt::vec3f & _val);
	void	scale_m3(mat3f& _out, const mt::vec3f &_val);

	void	transpose_m3(mat3f& _out, const mat3f& _rhs);
	mat3f	transpose_m3(const mat3f& _rhs);

	float	det_m3(const mat3f& _rhs);

	void	inv_m3(mat3f& _out, const mat3f& _rhs);

	void	ident_m3(mat3f& out);

	void	get_col_m3(vec3f& out, const mat3f& _rhs, int _index);
	vec3f	get_col_m3(const mat3f& _rhs, int _index);
	void	set_col_m3(mat3f& out, const vec3f& _rhs, int _index);

 	void	set_m3_from_axes(mat3f& _out, const vec3f& _a, const vec3f& _b, const vec3f& _c);
	mat3f	set_m3_from_axes(const vec3f& _a, const vec3f& _b, const vec3f& _c);
}