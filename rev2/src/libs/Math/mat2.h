/*	
Matrix 3x3 class.
-	basic operation(+,-,*,=,==).
-	identity, transpose, minor, determinant, adjoint, inverse
*/
#pragma once

#include "vec2.h"

namespace mt
{ 
	struct mat2f
	{
		union 
		{
			struct
			{
				vec2f v[2]; 
			};
			struct
			{
				vec2f v0, v1; 
			};
			float m[4];
		};

		mat2f();
		mat2f(float angle);
		mat2f(const mat2f&	_rhs);
		mat2f(const vec2f&	_v0, const vec2f&	_v1);

		vec2f& operator[](int i);
		const vec2f& operator[](int i)const;

		mat2f& operator=(const mat2f& _rhs);
	};

	bool	operator==(const mat2f& _a, const mat2f& _b);
	bool	operator!=(const mat2f& _a, const mat2f& _b);
	vec2f	operator*(const mat2f& _m, const vec2f& _v);
	vec2f	operator*(const vec2f& _v, const mat2f& _m);
	mat2f	operator*(const mat2f& _a, const mat2f& _b);
	mat2f	operator+(const mat2f& _a, const mat2f& _b);
	mat2f	operator-(const mat2f& _a, const mat2f& _b);
	mat2f	operator*(const mat2f& _rhs, float _val);

	bool	cmp_m2_m2(const mat2f& _a, const mat2f& _b);
	void	mul_m2_v2(vec2f& _out, const mat2f& _m, const vec2f& _v);
	void	mul_v2_m2(vec2f& _out, const vec2f& _v, const mat2f& _m);
	void	mul_m2_m2(mat2f& _out, const mat2f& _a, const mat2f& _b);
	void	add_m2_m2(mat2f& _out, const mat2f& _a, const mat2f& _b);
	void	sub_m2_m2(mat2f& _out, const mat2f& _a, const mat2f& _b);
	void	scale_m2_s(mat2f& _out, const mat2f& _rhs, float _val);
	void	scale_m2_s(mat2f& _out, float _val);

	void	transpose_m2(mat2f& _out, const mat2f& _rhs);
	mat2f	transpose_m2(const mat2f& _rhs);
}