/*	
	Vector4 class.
	-	basic operation with vectors(+,-,*,=,==).
	-	dot, norm, homogenize.
*/
#pragma once

#include "vec3.h"

#include "math.h"
#include <assert.h>

namespace mt
{
	struct vec4f
	{
		union
		{
			struct
			{ 
				float x,y,z,w; 
			};
			struct
			{
				float r,g,b,a;
			};	
			struct
			{
				vec3f v3;
				float w; 
			};
			float m[4];
		};

		vec4f();
		vec4f(float _x, float _y, float _z, float _w);
		vec4f(const vec4f& _v);
		explicit vec4f(const vec3f& _v);

		float&		operator [](int i);
		const float operator [](int i)const;

		vec4f&		operator+=(const vec4f&	_rhs);
		vec4f&		operator-=(const vec4f&	_rhs);
		vec4f&		operator/=(const float _rhs);
		vec4f&		operator*=(const float _rhs);

		float		sqrlength()	const;
		float		length() const;
	};

	bool	operator==(const vec4f& _a, const vec4f& _b);
	bool	operator!=(const vec4f& _a, const vec4f& _b);

	vec4f	operator+(const vec4f& _a, const vec4f& _b);
	vec4f	operator-(const vec4f& _a, const vec4f& _b);
	vec4f	operator*(const vec4f& _rhs, const float _val);
	vec4f	operator/(const vec4f& _rhs, const float _val);
	vec4f	operator-(vec4f _rhs);
	
	bool	cmp_v4_v4(const vec4f& _a, const vec4f& _b, float eps = 0.00001f);

	void	add_v4_v4(vec4f& _out, const vec4f& _a, const vec4f& _b);
	void	sub_v4_v4(vec4f& _out, const vec4f& _a, const vec4f& _b);
	void	scale_v4_v4(vec4f& _out, const vec4f& _a, float _val);
	void	neg_v4(vec4f& _out);
	
	float	dot_v4_v4(const vec4f &a, const vec4f &b);
	
	void	norm_v4(vec4f& _out, const vec4f& _rhs);
	vec4f	norm_v4(const vec4f& _rhs);
	void	norm_safe_v4(vec4f& out, const vec4f& _rhs, float err = 1e-06);
	vec4f	norm_safe_v4(const vec4f& _rhs);
	
	void	homogenize_v4(vec4f& _out, const vec4f& _rhs);
	vec4f	homogenize_v4(const vec4f& _rhs);
};