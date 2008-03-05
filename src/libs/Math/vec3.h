/*	
	Vector3 class.
	-	basic operation with vectors(+,-,*,=,==).
	-	dot, reflect, norm, cross.
*/
#pragma once

#include "vec2.h"

#include "math.h"

namespace mt
{
	struct vec3f
	{
		static const mt::vec3f zero_v3;

		union
		{
			struct
			{
				float x,y,z;
			};
			struct
			{
				float u,v,t;
			};
			struct
			{
				vec2f	v2;
				float	z;
			};
			float m[3];
		};

		vec3f();
		vec3f(float _x, float _y, float _z);
		vec3f(const vec3f& _v);


		void cross(const vec3f &left, const vec3f & right);

		explicit vec3f(const vec2f&	_v);
		explicit vec3f(const vec2f&	_v, float _z);

		vec3f& operator= (const vec3f& _rhs);
	
		const float operator[](int i) const;
		float&		operator[](int i);

		vec3f&		operator+=(const vec3f&	_rhs);
		vec3f&		operator-=(const vec3f&	_rhs);
		vec3f&		operator/=(const float _rhs);
		vec3f&		operator*=(const float _rhs);
		float sqrlength() const;
		float length() const;
	};

	bool	operator==(const vec3f& _a, const vec3f& _b);
	bool	operator!=(const vec3f& _a, const vec3f& _b);

	vec3f	operator+(const vec3f& _a, const vec3f& _b);
	vec3f	operator-(const vec3f& _a, const vec3f& _b);
	vec3f	operator*(const vec3f& _rhs, const float _val);
	vec3f	operator*(const float _val, const vec3f& _rhs);
	vec3f	operator/(const vec3f& _rhs, const float _val);
	vec3f	operator-(vec3f _rhs);

	float	length_v3_v3(const vec3f& _a, const vec3f& _b);

	bool	cmp_v3_v3(const vec3f& _a, const vec3f& _b, float eps = 0.00001f);

	void	add_v3_v3(vec3f& _out,const vec3f& _a, const vec3f& _b);
	void	sub_v3_v3(vec3f& _out,const vec3f& _a, const vec3f& _b);
	void	scale_v3_v3(vec3f& _out, const vec3f& _a, float _val);
	void	neg_v3(vec3f& _out);

	float	dot_v3_v3(const vec3f &a, const vec3f &b);
	float	dot_v3_v3_clamp(const vec3f &a, const vec3f &b);

	void	reflect_v3_v3(vec3f& _out, const vec3f& _a, const vec3f& _b);
	vec3f	reflect_v3_v3(const vec3f& _a, const vec3f& _b);

	void	cross_v3_v3(vec3f& _out, const vec3f& _a, const vec3f& _b);
	vec3f	cross_v3_v3(const vec3f& _a, const vec3f& _b);

	void	cross_v3_v3_norm(vec3f& _out, const vec3f& _a, const vec3f& _b);

	void	norm_v3(vec3f& _out, const vec3f& _rhs);
	vec3f	norm_v3(const vec3f& _rhs);
	void	norm_safe_v3(vec3f& out, const vec3f& _rhs, float err = 1e-06);
	vec3f	norm_safe_v3(const vec3f& _rhs);

	float	get_axis_angle(const vec3f& dir, int axis);
};