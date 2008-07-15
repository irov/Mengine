#	pragma once

#	include <math.h>

namespace mt
{
	MATH_INLINE vec4f::vec4f()
	{}

	MATH_INLINE vec4f::vec4f(float _x, float _y, float _z, float _w)
		:x(_x)
		,y(_y)
		,z(_z)
		,w(_w)
	{}

	MATH_INLINE vec4f::vec4f(const vec4f& _v)
		:x(_v.x)
		,y(_v.y)
		,z(_v.z)
		,w(_v.w)
	{}

	MATH_INLINE vec4f::vec4f(const vec3f& _v)
		:x(_v.x)
		,y(_v.y)
		,z(_v.z)
		,w(0.f)
	{}

	MATH_INLINE vec4f::vec4f(const vec3f& _v, float _w)
		:x(_v.x)
		,y(_v.y)
		,z(_v.z)
		,w(_w)
	{}

	MATH_INLINE vec4f& vec4f::operator=(const vec4f& _rhs)
	{
		x = _rhs.x;
		y = _rhs.y;
		z = _rhs.z;
		w = _rhs.w;
		return *this;
	}

	MATH_INLINE float& vec4f::operator[](int i)
	{
		assert(i < 4);
		return m[i];
	}

	MATH_INLINE const float vec4f::operator[](int i)const
	{
		assert(i < 4);
		return m[i];
	}

	MATH_INLINE vec4f&	vec4f::operator+=(const vec4f&	_rhs)
	{
		x += _rhs.x;
		y += _rhs.y;
		z += _rhs.z;
		w += _rhs.w;
		return *this;
	}

	MATH_INLINE vec4f&	vec4f::operator-=(const vec4f&	_rhs)
	{
		x -= _rhs.x;
		y -= _rhs.y;
		z -= _rhs.z;
		w -= _rhs.w;
		return *this;
	}

	MATH_INLINE vec4f&	vec4f::operator/=(const float _rhs)
	{
		assert(_rhs != 0.0f);
		operator *= (1.f/_rhs); 
		return *this;
	}

	MATH_INLINE vec4f&	vec4f::operator*=(const float _rhs)
	{
		x *= _rhs;
		y *= _rhs;
		z *= _rhs;
		w *= _rhs;
		return *this;
	}

	MATH_INLINE float vec4f::sqrlength() const
	{
		return	x*x+y*y+z*z+w*w;
	}

	MATH_INLINE float vec4f::length() const
	{
		return sqrtf(sqrlength());
	}

	/*	cmp	 */
	MATH_INLINE bool cmp_v4_v4(const vec4f& _a, const vec4f& _b, float eps)
	{
		return	(fabsf(_a.x - _b.x) < eps) && (fabsf(_a.y - _b.y) < eps) && (fabsf(_a.z - _b.z) < eps) && (fabsf(_a.w - _b.w) < eps);
	}

	MATH_INLINE bool operator==(const vec4f& _a, const vec4f& _b) 
	{
		return cmp_v4_v4(_a, _b);
	}

	MATH_INLINE bool operator!=(const vec4f& _a, const vec4f& _b) 
	{
		return !operator==(_a, _b);
	}

	/*	Addition of vecs  */
	MATH_INLINE void	add_v4_v4(vec4f& _out,const vec4f& _a, const vec4f& _b)
	{
		_out.x = _a.x + _b.x;
		_out.y = _a.y + _b.y;
		_out.z = _a.z + _b.z;
		_out.w = _a.w + _b.w;
	}

	MATH_INLINE vec4f operator+(const vec4f& _a, const vec4f& _b) 
	{
		vec4f	out;
		add_v4_v4(out, _a, _b);
		return	out;
	}

	/*	Addition of vecs  */
	MATH_INLINE void	sub_v4_v4(vec4f& _out,const vec4f& _a, const vec4f& _b)
	{
		_out.x = _a.x - _b.x;
		_out.y = _a.y - _b.y;
		_out.z = _a.z - _b.z;
		_out.w = _a.w - _b.w;
	}

	MATH_INLINE vec4f operator-(const vec4f& _a, const vec4f& _b) 
	{
		vec4f	out;
		sub_v4_v4(out, _a, _b);
		return	out;
	}

	/*	Scale of vecs  */
	MATH_INLINE void	scale_v4_v4(vec4f& _out, const vec4f& _a, float _val)
	{
		_out.x = _a.x * _val;
		_out.y = _a.y * _val;
		_out.z = _a.z * _val;
		_out.w = _a.w * _val;
	}

	MATH_INLINE vec4f	operator*(const vec4f& _rhs, const float _val) 
	{
		vec4f	out;
		scale_v4_v4(out, _rhs, _val);
		return out;
	}

	MATH_INLINE vec4f	operator/(const vec4f& _rhs, const float _val) 
	{
		return operator*(_rhs, 1/_val);
	}

	/*	Negation */
	MATH_INLINE void	neg_v4(vec4f& _out)
	{
		_out.x = -_out.x;
		_out.y = -_out.y;
		_out.z = -_out.z;
		_out.w = -_out.w;
	}

	MATH_INLINE vec4f operator-(vec4f _rhs)
	{
		neg_v4(_rhs);
		return	_rhs;
	}

	/*	Dot	Product			*/
	MATH_INLINE float	dot_v4_v4(const vec4f& a, const vec4f& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	/*	Normalize Vector*/
	MATH_INLINE void	norm_v4(vec4f& _out, const vec4f& _rhs)
	{
		_out = _rhs / _rhs.length();
	}

	MATH_INLINE vec4f	norm_v4(const vec4f& _rhs)
	{
		vec4f	out;
		norm_v4(out, _rhs);
		return	out;
	}

	MATH_INLINE void	norm_safe_v4(vec4f& out, const vec4f& _rhs, float err)
	{
		float	len = _rhs.length();
		if (len > err)
        {
			out = _rhs / len;
			return;
		}
		out = _rhs;
	}

	MATH_INLINE vec4f	norm_safe_v4(const vec4f& _rhs)
	{
		vec4f	out;
		norm_safe_v4(out, _rhs);
		return	out;
	}

	/*	SetHomogenized */
	MATH_INLINE void	homogenize_v4(vec4f& _out, const vec4f& _rhs)
	{
		_out.x /= _out.w;
		_out.y /= _out.w;
		_out.z /= _out.w;
		_out.w = 1.0f;
	}

	MATH_INLINE vec4f	homogenize_v4(const vec4f& _rhs)
	{
		vec4f	out;
		homogenize_v4(out, _rhs);
		return	out;
	}

	MATH_INLINE float length_v4( vec4f _rhs )
	{
		return _rhs.length();
	}

}