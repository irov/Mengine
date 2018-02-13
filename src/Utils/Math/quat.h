#	pragma once

#	include "config.h"

#	include "utils.h"

#	include "mat4.h"

namespace mt
{
	struct quatf
	{	
		float x;
		float y;
		float z;
		float w;

		MENGINE_MATH_METHOD_INLINE quatf();
		MENGINE_MATH_METHOD_INLINE quatf(float _x, float _y, float _z, float _w);
		MENGINE_MATH_METHOD_INLINE quatf(float _angle, const mt::vec3f & _v);
		MENGINE_MATH_METHOD_INLINE quatf(const quatf& _q);

		MENGINE_MATH_METHOD_INLINE void rotate(vec3f & v) const;
		MENGINE_MATH_METHOD_INLINE void multiply(const quatf& left, const vec3f& right);
		MENGINE_MATH_METHOD_INLINE float& operator[](size_t i);
		MENGINE_MATH_METHOD_INLINE const float operator[](size_t i)const;

		MENGINE_MATH_METHOD_INLINE quatf& operator=(const quatf &q);

		MENGINE_MATH_METHOD_INLINE float length() const;
		MENGINE_MATH_METHOD_INLINE void normalize();

		MENGINE_MATH_METHOD_INLINE float getYaw(bool _reprojectAxis = true) const;
		MENGINE_MATH_METHOD_INLINE float getPitch(bool _reprojectAxis = true) const;
	};

	MENGINE_MATH_FUNCTION_INLINE bool operator==(const quatf& _a, const quatf& _b);
	MENGINE_MATH_FUNCTION_INLINE bool operator!=(const quatf& _a, const quatf& _b);
	MENGINE_MATH_FUNCTION_INLINE quatf operator+(const quatf& _a, const quatf& _b);
	MENGINE_MATH_FUNCTION_INLINE quatf operator-(const quatf& _a, const quatf& _b);
	MENGINE_MATH_FUNCTION_INLINE quatf operator*(const quatf& _a, const quatf& _b);
	MENGINE_MATH_FUNCTION_INLINE vec3f operator*(const quatf& _rhs, const vec3f& _v);
	MENGINE_MATH_FUNCTION_INLINE quatf operator*(const quatf& _rhs, float _val);
	MENGINE_MATH_FUNCTION_INLINE quatf operator/(const quatf& _rhs, float _val);
	
	MENGINE_MATH_FUNCTION_INLINE bool cmp_q_q( const quatf& _a, const quatf& _b );

	MENGINE_MATH_FUNCTION_INLINE void add_q_q(quatf&	_out, const quatf& _a,const quatf& _b);
	MENGINE_MATH_FUNCTION_INLINE void sub_q_q(quatf&	_out, const quatf& _a,const quatf& _b);
	MENGINE_MATH_FUNCTION_INLINE void mul_q_q(quatf& out, const quatf& _rhs, const quatf& in2);
	MENGINE_MATH_FUNCTION_INLINE void mul_q_v3(vec3f& out,const quatf& _rhs,const vec3f& in2);
	MENGINE_MATH_FUNCTION_INLINE void scale_q_s(quatf& out, const quatf& _rhs, float _val);

	MENGINE_MATH_FUNCTION_INLINE float dot_q_q(const quatf& _a, const quatf& _b);

	MENGINE_MATH_FUNCTION_INLINE void norm_q_q(quatf& _out, const quatf& _rhs);
	MENGINE_MATH_FUNCTION_INLINE quatf norm_q(const quatf& _rhs);
	MENGINE_MATH_FUNCTION_INLINE void norm_safe_q(quatf& out, const quatf& _rhs);
	MENGINE_MATH_FUNCTION_INLINE quatf norm_safe_q(const quatf& _rhs);

	MENGINE_MATH_FUNCTION_INLINE void lerp_q_q( quatf & _q, const quatf & _q1, const quatf& _q2, float _t );
	
	MENGINE_MATH_FUNCTION_INLINE void q_from_angle_axis(quatf& out, const vec3f& _rhs, float _val);
	MENGINE_MATH_FUNCTION_INLINE quatf q_from_angle_axis(const vec3f& _rhs, float _val);

	MENGINE_MATH_FUNCTION_INLINE void inverse_q(quatf& _out, const quatf& _rhs);
	MENGINE_MATH_FUNCTION_INLINE quatf inverse_q(const quatf& _rhs);

	MENGINE_MATH_FUNCTION_INLINE void exp_q(quatf& _out, const quatf& _rhs);
	MENGINE_MATH_FUNCTION_INLINE quatf exp_q(const quatf& _rhs);

	MENGINE_MATH_FUNCTION_INLINE void log_q(quatf& _out, const quatf& _rhs);
	MENGINE_MATH_FUNCTION_INLINE quatf log_q(const quatf& _rhs);

	MENGINE_MATH_FUNCTION_INLINE void q_from_rot_m3(quatf& out, const mat3f& _rhs);
	MENGINE_MATH_FUNCTION_INLINE quatf q_from_rot_m3(const mat3f& _rhs);

	MENGINE_MATH_FUNCTION_INLINE void q_to_rot_m3(mat3f& out, const quatf& _rhs);
	MENGINE_MATH_FUNCTION_INLINE mat3f q_to_rot_m3(const quatf& _rhs);

	MENGINE_MATH_FUNCTION_INLINE void q_from_rot_m4(quatf& out, const mat4f& _rhs);
	MENGINE_MATH_FUNCTION_INLINE quatf q_from_rot_m4(const mat4f& _rhs);

	MENGINE_MATH_FUNCTION_INLINE void q_from_axes(quatf& out, const vec3f& _x, const vec3f& _y, const vec3f& _z);
	MENGINE_MATH_FUNCTION_INLINE quatf q_from_axes(const vec3f& _x, const vec3f& _y, const vec3f& _z);

	MENGINE_MATH_FUNCTION_INLINE void q_to_angle_axis(vec3f& _out, float& _out1, const quatf& _rhs);
	MENGINE_MATH_FUNCTION_INLINE vec3f q_to_angle_axis(float& _out, const quatf& _rhs);

	MENGINE_MATH_FUNCTION_INLINE void rotate_q(quatf& _out, const vec3f& axis, float angle);

	MENGINE_MATH_FUNCTION_INLINE void qpos_to_rot_m4(mat4f& out, const quatf& _rhs, const vec3f& _pos);
	MENGINE_MATH_FUNCTION_INLINE mat4f qpos_to_rot_m4(const quatf& _rhs, const vec3f& _pos);

	MENGINE_MATH_FUNCTION_INLINE void make_quat_from_euler( quatf & _out, const mt::vec3f & _euler );
    MENGINE_MATH_FUNCTION_INLINE void quat_to_euler( const quatf & _q, mt::vec3f & _euler );

	MENGINE_MATH_FUNCTION_INLINE void make_quat_from_angle( quatf & _out, float _angle );

    MENGINE_MATH_FUNCTION_INLINE float quatzw_to_angle( const quatf & _q );
};

#	if MENGINE_MATH_FORCE_INLINE == 1
#	include "quat_inline.h"
#	endif
