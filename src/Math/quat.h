/*	
	Quaternion class.
	-	basic operation with quats(+,-,*,=,==).
	-	dot, reflect, norm, quat from angle axis,
	-	inverse quat, exp, log, quat from rot matrix 3x3,
	-	quat to rot matrix 3x3, quat from axes, quat to angle axis.
*/
#pragma once

#include "math.h"
#include <assert.h>
#include "vec3.h"
#include "mat3.h"

namespace mt
{
	struct quatf
	{
		union
		{
			struct
			{
				float w, x, y, z;
			};

			struct
			{
				float w;
				vec3f v;
			};
			float m[4];
		};

		quatf();
		quatf(float _w, float _x, float _y, float _z);
		quatf(const quatf& _q);

		float& operator[](int i);
		const float operator[](int i)const;

		quatf& operator=(const quatf &q);

		float norm()	const;
	};

	bool	operator==(const quatf& _a, const quatf& _b);
	bool	operator!=(const quatf& _a, const quatf& _b);
	quatf	operator+(const quatf& _a, const quatf& _b);
	quatf	operator-(const quatf& _a, const quatf& _b);
	quatf	operator*(const quatf& _a, const quatf& _b);
	vec3f	operator*(const quatf& _rhs, const vec3f& _v);
	quatf	operator*(const quatf& _rhs, float _val);
	quatf	operator/(const quatf& _rhs, float _val);
	
	bool	cmp_q_q(const quatf& _a, const quatf& _b, float eps = 0.00001f);

	void	add_q_q(quatf&	_out, const quatf& _a,const quatf& _b);
	void	sub_q_q(quatf&	_out, const quatf& _a,const quatf& _b);
	void	mul_q_q(quatf& out, const quatf& _rhs, const quatf& in2);
	void	mul_q_v3(vec3f& out,const quatf& _rhs,const vec3f& in2);
	void	scale_q_s(quatf& out, const quatf& _rhs, float _val);

	float	dot_q_q(const quatf& _a, const quatf& _b);

	void	norm_q(quatf& _out, const quatf& _rhs);
	quatf	norm_q(const quatf& _rhs);
	void	norm_safe_q(quatf& out, const quatf& _rhs, float err = 1e-06);
	quatf	norm_safe_q(const quatf& _rhs);

	void	q_from_angle_axis(quatf& out, const vec3f& _rhs, float _val);
	quatf	q_from_angle_axis(const vec3f& _rhs, float _val);

	void	inverse_q(quatf& _out, const quatf& _rhs);
	quatf	inverse_q(const quatf& _rhs);

	void	exp_q(quatf& _out, const quatf& _rhs);
	quatf	exp_q(const quatf& _rhs);

	void	log_q(quatf& _out, const quatf& _rhs);
	quatf	log_q(const quatf& _rhs);

	void	q_from_rot_m3(quatf& out, const mat3f& _rhs);
	quatf	q_from_rot_m3(const mat3f& _rhs);

	void	q_to_rot_m3(mat3f& out, const quatf& _rhs);
	mat3f	q_to_rot_m3(const quatf& _rhs);

	void	q_from_axes(quatf& out, const vec3f& _x, const vec3f& _y, const vec3f& _z);
	quatf	q_from_axes(const vec3f& _x, const vec3f& _y, const vec3f& _z);

	void	q_to_angle_axis(vec3f& _out, float& _out1, const quatf& _rhs);
	vec3f	q_to_angle_axis(float& _out, const quatf& _rhs);
};