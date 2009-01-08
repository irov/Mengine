#	pragma once

/*	
	Quaternion class.
	-	basic operation with quats(+,-,*,=,==).
	-	dot, reflect, norm, quat from angle axis,
	-	inverse quat, exp, log, quat from rot matrix 3x3,
	-	quat to rot matrix 3x3, quat from axes, quat to angle axis.
*/

#	include "config.h"

#	include "mat4.h"

namespace mt
{
	struct quatf
	{
	
		float x;
		float y;
		float z;
		float w;

		MATH_INLINE quatf();
		MATH_INLINE quatf(float _x, float _y, float _z, float _w);
		MATH_INLINE quatf(float _angle, const mt::vec3f & _v);
		MATH_INLINE quatf(const quatf& _q);

		MATH_INLINE void rotate(vec3f & v) const;
		MATH_INLINE void multiply(const quatf& left, const vec3f& right);
		MATH_INLINE float& operator[](int i);
		MATH_INLINE const float operator[](int i)const;

		MATH_INLINE quatf& operator=(const quatf &q);

		MATH_INLINE float norm() const;

		MATH_INLINE float getYaw(bool _reprojectAxis = true) const;
		MATH_INLINE float getPitch(bool _reprojectAxis = true) const;
	};

	MATH_INLINE bool operator==(const quatf& _a, const quatf& _b);
	MATH_INLINE bool operator!=(const quatf& _a, const quatf& _b);
	MATH_INLINE quatf operator+(const quatf& _a, const quatf& _b);
	MATH_INLINE quatf operator-(const quatf& _a, const quatf& _b);
	MATH_INLINE quatf operator*(const quatf& _a, const quatf& _b);
	MATH_INLINE vec3f operator*(const quatf& _rhs, const vec3f& _v);
	MATH_INLINE quatf operator*(const quatf& _rhs, float _val);
	MATH_INLINE quatf operator/(const quatf& _rhs, float _val);
	
	MATH_INLINE bool cmp_q_q(const quatf& _a, const quatf& _b, float eps = 0.00001f);

	MATH_INLINE void add_q_q(quatf&	_out, const quatf& _a,const quatf& _b);
	MATH_INLINE void sub_q_q(quatf&	_out, const quatf& _a,const quatf& _b);
	MATH_INLINE void mul_q_q(quatf& out, const quatf& _rhs, const quatf& in2);
	MATH_INLINE void mul_q_v3(vec3f& out,const quatf& _rhs,const vec3f& in2);
	MATH_INLINE void scale_q_s(quatf& out, const quatf& _rhs, float _val);

	MATH_INLINE float dot_q_q(const quatf& _a, const quatf& _b);

	MATH_INLINE void norm_q(quatf& _out, const quatf& _rhs);
	MATH_INLINE quatf norm_q(const quatf& _rhs);
	MATH_INLINE void norm_safe_q(quatf& out, const quatf& _rhs, float err = 1e-06);
	MATH_INLINE quatf norm_safe_q(const quatf& _rhs);

	MATH_INLINE void q_from_angle_axis(quatf& out, const vec3f& _rhs, float _val);
	MATH_INLINE quatf q_from_angle_axis(const vec3f& _rhs, float _val);

	MATH_INLINE void inverse_q(quatf& _out, const quatf& _rhs);
	MATH_INLINE quatf inverse_q(const quatf& _rhs);

	MATH_INLINE void exp_q(quatf& _out, const quatf& _rhs);
	MATH_INLINE quatf exp_q(const quatf& _rhs);

	MATH_INLINE void log_q(quatf& _out, const quatf& _rhs);
	MATH_INLINE quatf log_q(const quatf& _rhs);

	MATH_INLINE void q_from_rot_m3(quatf& out, const mat3f& _rhs);
	MATH_INLINE quatf q_from_rot_m3(const mat3f& _rhs);

	MATH_INLINE void q_to_rot_m3(mat3f& out, const quatf& _rhs);
	MATH_INLINE mat3f q_to_rot_m3(const quatf& _rhs);

	MATH_INLINE void q_from_rot_m4(quatf& out, const mat4f& _rhs);
	MATH_INLINE quatf q_from_rot_m4(const mat4f& _rhs);

	MATH_INLINE void q_from_axes(quatf& out, const vec3f& _x, const vec3f& _y, const vec3f& _z);
	MATH_INLINE quatf q_from_axes(const vec3f& _x, const vec3f& _y, const vec3f& _z);

	MATH_INLINE void q_to_angle_axis(vec3f& _out, float& _out1, const quatf& _rhs);
	MATH_INLINE vec3f q_to_angle_axis(float& _out, const quatf& _rhs);

	MATH_INLINE void rotate_q(quatf& _out, const vec3f& axis, float angle);

	MATH_INLINE void qpos_to_rot_m4(mat4f& out, const quatf& _rhs, const vec3f& _pos);
	MATH_INLINE mat4f qpos_to_rot_m4(const quatf& _rhs, const vec3f& _pos);
};

#	if MATH_FORCE_INLINE == 1
#	include "quat_inline.h"
#	endif