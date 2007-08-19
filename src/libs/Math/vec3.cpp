#include "vec3.h"

namespace	mt
{
	vec3f::vec3f() 
	{
	}

	vec3f::vec3f(float _x, float _y, float _z)
		:x(_x)
		,y(_y)
		,z(_z)
	{
	}

	vec3f::vec3f(const vec3f&	_v)
		:x(_v.x)
		,y(_v.y)
		,z(_v.z)
	{}

	vec3f::vec3f(const vec2f&	_v)
		:x(_v.x)
		,y(_v.y)
		,z(0.0f)
	{}

	vec3f::vec3f(const vec2f&	_v, float _z)
		:x(_v.x)
		,y(_v.y)
		,z(_z)
	{}

	vec3f& vec3f::operator=(const vec3f& _rhs)
    {
        x = _rhs.x;
        y = _rhs.y;
        z = _rhs.z;            
        return *this;
    }

	const float vec3f::operator[](int i) const
	{
		assert(i < 3);
		return m[i];
	}

	float& vec3f::operator[](int i)
	{
		assert(i < 3);
		return m[i];
	}

	vec3f&		vec3f::operator+=(const vec3f&	_rhs)
	{
		x += _rhs.x;
		y += _rhs.y;
		z += _rhs.z;
		return *this;
	}

	vec3f&		vec3f::operator-=(const vec3f&	_rhs)
	{
		x -= _rhs.x;
		y -= _rhs.y;
		z -= _rhs.z;
		return *this;
	}

	vec3f&		vec3f::operator/=(const float _rhs)
	{
		assert(_rhs != 0.0f);
		operator *= (1.f/_rhs); 
		return *this;
	}

	vec3f&		vec3f::operator*=(const float _rhs)
	{
		x *= _rhs;
		y *= _rhs;
		z *= _rhs;
		return *this;
	}

	float	vec3f::sqrlength()	const
	{
		return	x*x+y*y+z*z;
	}

	float	vec3f::length()	const
	{
		float len = sqrlength();
		return sqrtf(len);
	}

	float	length_v3_v3(const vec3f& _a, const vec3f& _b)
	{
		vec3f c = _a - _b;
		return c.length();
	}

	bool	cmp_v3_v3(const vec3f& _a, const vec3f& _b, float eps)
	{
		return	(fabs(_a.x - _b.x) < eps) && (fabs(_a.y - _b.y) < eps) && (fabs(_a.z - _b.z) < eps);
	}

	bool operator==(const vec3f& _a, const vec3f& _b) 
	{
		return cmp_v3_v3(_a, _b);
	}

	bool operator!=(const vec3f& _a, const vec3f& _b) 
	{
		return !operator==(_a, _b);
	}

	/*	Addition of vecs  */
	void	add_v3_v3(vec3f& _out,const vec3f& _a, const vec3f& _b)
	{
		_out.x = _a.x + _b.x;
		_out.y = _a.y + _b.y;
		_out.z = _a.z + _b.z;
	}

	vec3f operator+(const vec3f& _a, const vec3f& _b) 
	{
		vec3f	out;
		add_v3_v3(out, _a, _b);
		return	out;
	}

	/*	Addition of vecs  */
	void	sub_v3_v3(vec3f& _out,const vec3f& _a, const vec3f& _b)
	{
		_out.x = _a.x - _b.x;
		_out.y = _a.y - _b.y;
		_out.z = _a.z - _b.z;
	}

	vec3f operator-(const vec3f& _a, const vec3f& _b) 
	{
		vec3f	out;
		sub_v3_v3(out, _a, _b);
		return	out;
	}

	/*	Scale of vecs  */
	void	scale_v3_v3(vec3f& _out, const vec3f& _a, float _val)
	{
		_out.x = _a.x * _val;
		_out.y = _a.y * _val;
		_out.z = _a.z * _val;
	}

	vec3f	operator*(const vec3f& _rhs, const float _val) 
	{
		vec3f	out;
		scale_v3_v3(out, _rhs, _val);
		return out;
	}

	vec3f	operator*(const float _val, const vec3f& _rhs)
	{
		vec3f	out;
		scale_v3_v3(out, _rhs, _val);
		return out;
	}

	vec3f	operator/(const vec3f& _rhs, const float _val) 
	{
		return operator*(_rhs, 1/_val);
	}

	/*	Negation */
	void	neg_v3(vec3f& _out)
	{
		_out.x = -_out.x;
		_out.y = -_out.y;
		_out.z = -_out.z;
	}

	vec3f operator-(vec3f _rhs)
	{
		neg_v3(_rhs);
		return	_rhs;
	}

	/*	Dot	Product			*/
	float	dot_v3_v3(const vec3f &a, const vec3f &b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	/*	Reflection Vector	*/
	void	reflect_v3_v3(vec3f& _out, const vec3f& _a, const vec3f& _b)
	{
		_out = _a-_b*dot_v3_v3(_a,_b)*2.0f;
	}	

	vec3f	reflect_v3_v3(const vec3f& _a, const vec3f& _b)
	{
		vec3f	out;
		reflect_v3_v3(out, _a, _b);
		return	out;
	}	
	
	/*	Cross Product	*/
	void	cross_v3_v3(vec3f& _out, const vec3f& _a, const vec3f& _b)
	{
		_out.x = _a.y*_b.z - _a.z*_b.y;
		_out.y = _a.z*_b.x - _a.x*_b.z;
		_out.z = _a.x*_b.y - _a.y*_b.x;
	}

	vec3f	cross_v3_v3(const vec3f& _a, const vec3f& _b)
	{
		vec3f	out;
		cross_v3_v3(out, _a, _b);
		return	out;
	}

	void	cross_v3_v3_norm(vec3f& _out, const vec3f& _a, const vec3f& _b)
	{
		_out.x = _a.y*_b.z - _a.z*_b.y;
		_out.y = _a.z*_b.x - _a.x*_b.z;
		_out.z = _a.x*_b.y - _a.y*_b.x;

		norm_v3( _out , _out );
	}
	
	/*	Normalize Vector*/
	void	norm_v3(vec3f& _out, const vec3f& _rhs)
	{
		_out = _rhs / _rhs.length();
	}

	vec3f	norm_v3(const vec3f& _rhs)
	{
		vec3f	out;
		norm_v3(out, _rhs);
		return	out;
	}

	void	norm_safe_v3(vec3f& out, const vec3f& _rhs, float err)
	{
		float	len = _rhs.length();
		if (len > err)
        {
			out = _rhs / len;
			return;
		}
		out = _rhs;
	}

	vec3f	norm_safe_v3(const vec3f& _rhs)
	{
		vec3f	out;
		norm_safe_v3(out, _rhs);
		return	out;
	}
}