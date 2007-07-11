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
		union 
		{
			struct
			{
				vec4f v[4];
			};

			struct
			{
				vec4f v0, v1, v2, v3;
			};
			struct
			{
				vec3f v3_0;	float f_0;
				vec3f v3_1;	float f_1;
				vec3f v3_2;	float f_2;
				vec3f v3_3;	float f_3;
			};
			float m[16];
		};

		mat4f();
		mat4f(const mat4f& _rhs);
		mat4f(const vec4f& _a0, const vec4f& _a1, const vec4f& _a2, const vec4f& _a3);
	
		vec4f& operator[](int i);
		const vec4f& operator[](int i)const;

		mat4f& operator=(const mat4f& _rhs);
		mat4f& operator=(const mat3f& _rhs);
	};

	bool	operator==(const mat4f& _a, const mat4f& _b);
	bool	operator!=(const mat4f& _a, const mat4f& _b);

	vec3f	operator*(const mat4f& m, const vec3f& v);
	vec3f	operator*(const vec3f& v, const mat4f& m);

	vec4f	operator*(const mat4f& m, const vec4f& v);
	vec4f	operator*(const vec4f& v, const mat4f& m);
	
	mat4f	operator*(const mat4f& _a, const mat4f& _b);
	mat4f	operator*(const mat4f& _a, const mat3f& _b);

	mat4f	operator+(const mat4f& _a, const mat4f& _b);
	mat4f	operator-(const mat4f& _a, const mat4f& _b);

	bool	cmp_m4_m4(const mat4f& _a, const mat4f& _b);

	void	mul_m4_v3(vec3f& _out, const mat4f& _m,const vec3f& _v);
	void	mul_v3_m4(vec3f& _out, const vec3f& _v,const mat4f& _m);
	
	void	mul_m4_v4(vec4f& _out, const mat4f& _m,const vec4f& _v);
	void	mul_v4_m4(vec4f& _out, const vec4f& _v, const mat4f& _m);
	void	mul_m4_m4(mat4f& _out, const mat4f& _a, const mat4f& _b);
	void	mul_m4_m3(mat4f& _out, const mat4f& _a, const mat3f& _b);
	void	add_m4_m4(mat4f& _out, const mat4f& _a, const mat4f& _b);
	void	sub_m4_m4(mat4f& _out, const mat4f& _a, const mat4f& _b);

	void	scale_m4_m4(mat4f& _out, const mat4f& _rhs, const mt::vec4f & _val);
	void	scale_m4(mat4f& _out, const mt::vec4f &_val);

	void	scale_rotate_m4_m4(mat4f& _out, const mat4f& _rhs, const mt::vec3f & _val);
	void	scale_rotate_m4(mat4f& _out, const mt::vec3f &_val);

	void	ident_m4(mat4f& _out);

	void	get_col_m4(vec4f& out, const mat4f& _rhs, int _index);
	vec4f	get_col_m4(const mat4f& _rhs, int _index);
	void	set_col_m4(mat4f& out, const vec4f& _rhs, int _index);

	void	transpose_m4(mat4f& _out, const mat4f& _rhs);
	mat4f	transpose_m4(const mat4f& _rhs);

	void	extract_m3(mat3f& _out, const mat4f& _rhs);
	mat3f	extract_m3(const mat4f& _rhs);

	float	minor_m4(const mat4f& _rhs, const int r0, const int r1, const int r2, const int c0, const int c1, const int c2);

	float	det_m4(const mat4f& _rhs);

	void	adj_m4(mat4f& _out, const mat4f& _rhs);
	mat4f	adj_m4(const mat4f& _rhs);

	void	inv_m4(mat4f& _out, const mat4f& _rhs);
	mat4f	inv_m4(const mat4f& _rhs);

	void	rotate_axis_m4(mat4f &out, const vec3f &u, float degrees);
	void	make_projection_m4( mat4f &_out , float _fov, float _aspect, float _zn, float _zf );
}