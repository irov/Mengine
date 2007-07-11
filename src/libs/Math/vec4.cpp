#include "vec4.h"

namespace	mt
{
	vec4f::vec4f()
	{}

	vec4f::vec4f(float _x, float _y, float _z, float _w)
		:x(_x)
		,y(_y)
		,z(_z)
		,w(_w)
	{}

	vec4f::vec4f(const vec4f& _v)
		:x(_v.x)
		,y(_v.y)
		,z(_v.z)
		,w(_v.w)
	{}

	vec4f::vec4f(const vec3f& _v)
		:x(_v.x)
		,y(_v.y)
		,z(_v.z)
		,w(0.f)
	{}

	float& vec4f::operator[](int i)
	{
		assert(i < 4);
		return m[i];
	}

	const float vec4f::operator[](int i)const
	{
		assert(i < 4);
		return m[i];
	}

	vec4f&	vec4f::operator+=(const vec4f&	_rhs)
	{
		x += _rhs.x;
		y += _rhs.y;
		z += _rhs.z;
		w += _rhs.w;
		return *this;
	}

	vec4f&	vec4f::operator-=(const vec4f&	_rhs)
	{
		x -= _rhs.x;
		y -= _rhs.y;
		z -= _rhs.z;
		w -= _rhs.w;
		return *this;
	}

	vec4f&	vec4f::operator/=(const float _rhs)
	{
		assert(_rhs != 0.0f);
		operator *= (1.f/_rhs); 
		return *this;
	}

	vec4f&	vec4f::operator*=(const float _rhs)
	{
		x *= _rhs;
		y *= _rhs;
		z *= _rhs;
		w *= _rhs;
		return *this;
	}

	float vec4f::sqrlength() const
	{
		return	x*x+y*y+z*z+w*w;
	}

	float vec4f::length() const
	{
		return sqrtf(sqrlength());
	}

	/*	cmp	 */
	bool	cmp_v4_v4(const vec4f& _a, const vec4f& _b, float eps)
	{
		return	(fabs(_a.x - _b.x) < eps) && (fabs(_a.y - _b.y) < eps) && (fabs(_a.z - _b.z) < eps) && (fabs(_a.w - _b.w) < eps);
	}

	bool operator==(const vec4f& _a, const vec4f& _b) 
	{
		return cmp_v4_v4(_a, _b);
	}

	bool operator!=(const vec4f& _a, const vec4f& _b) 
	{
		return !operator==(_a, _b);
	}

	/*	Addition of vecs  */
	void	add_v4_v4(vec4f& _out,const vec4f& _a, const vec4f& _b)
	{
		_out.x = _a.x + _b.x;
		_out.y = _a.y + _b.y;
		_out.z = _a.z + _b.z;
		_out.w = _a.w + _b.w;
	}

	vec4f operator+(const vec4f& _a, const vec4f& _b) 
	{
		vec4f	out;
		add_v4_v4(out, _a, _b);
		return	out;
	}

	/*	Addition of vecs  */
	void	sub_v4_v4(vec4f& _out,const vec4f& _a, const vec4f& _b)
	{
		_out.x = _a.x - _b.x;
		_out.y = _a.y - _b.y;
		_out.z = _a.z - _b.z;
		_out.w = _a.w - _b.w;
	}

	vec4f operator-(const vec4f& _a, const vec4f& _b) 
	{
		vec4f	out;
		sub_v4_v4(out, _a, _b);
		return	out;
	}

	/*	Scale of vecs  */
	void	scale_v4_v4(vec4f& _out, const vec4f& _a, float _val)
	{
		_out.x = _a.x * _val;
		_out.y = _a.y * _val;
		_out.z = _a.z * _val;
		_out.w = _a.w * _val;
	}

	vec4f	operator*(const vec4f& _rhs, const float _val) 
	{
		vec4f	out;
		scale_v4_v4(out, _rhs, _val);
		return out;
	}

	vec4f	operator/(const vec4f& _rhs, const float _val) 
	{
		return operator*(_rhs, 1/_val);
	}

	/*	Negation */
	void	neg_v4(vec4f& _out)
	{
		_out.x = -_out.x;
		_out.y = -_out.y;
		_out.z = -_out.z;
		_out.w = -_out.w;
	}

	vec4f operator-(vec4f _rhs)
	{
		neg_v4(_rhs);
		return	_rhs;
	}

	/*	Dot	Product			*/
	float	dot_v4_v4(const vec4f& a, const vec4f& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	/*	Normalize Vector*/
	void	norm_v4(vec4f& _out, const vec4f& _rhs)
	{
		_out = _rhs / _rhs.length();
	}

	vec4f	norm_v4(const vec4f& _rhs)
	{
		vec4f	out;
		norm_v4(out, _rhs);
		return	out;
	}

	void	norm_safe_v4(vec4f& out, const vec4f& _rhs, float err)
	{
		float	len = _rhs.length();
		if (len > err)
        {
			out = _rhs / len;
			return;
		}
		out = _rhs;
	}

	vec4f	norm_safe_v4(const vec4f& _rhs)
	{
		vec4f	out;
		norm_safe_v4(out, _rhs);
		return	out;
	}

	/*	SetHomogenized */
	void	homogenize_v4(vec4f& _out, const vec4f& _rhs)
	{
		_out.x /= _out.w;
		_out.y /= _out.w;
		_out.z /= _out.w;
		_out.w = 1.0f;
	}

	vec4f	homogenize_v4(const vec4f& _rhs)
	{
		vec4f	out;
		homogenize_v4(out, _rhs);
		return	out;
	}
}