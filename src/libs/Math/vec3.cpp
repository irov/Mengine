#	define _USE_MATH_DEFINES
#	include "vec3.h"
#	include <math.h>
#	include <assert.h>

namespace	mt
{
	const vec3f vec3f::zero_v3(0, 0, 0);

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

	void vec3f::cross(const vec3f &left, const vec3f & right)	//prefered version, w/o temp object.
	{
		// temps needed in case left or right is this.
		float a = (left.y * right.z) - (left.z * right.y);
		float b = (left.z * right.x) - (left.x * right.z);
		float c = (left.x * right.y) - (left.y * right.x);

		x = a;
		y = b;
		z = c;
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

	float	dot_v3_v3_clamp(const vec3f &a, const vec3f &b)
	{
		float dot = a.x * b.x + a.y * b.y + a.z * b.z;
		if(dot > 1.0f) dot = 1.0f;
		if(dot < -1.0f) dot = -1.0f;
		return dot;
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

	float get_axis_angle(const vec3f& dir, int axis)
	{
		assert(axis >= 0 && axis < 3);

		int ind1, ind2;

		if(axis == 0)	
		{
			ind1 = 1; 
			ind2 = 2;
		}
		else if(axis == 1)	
		{
			ind1 = 0; 
			ind2 = 2;
		}
		else if(axis == 2)
		{
			ind1 = 0; 
			ind2 = 1;
		}

		float result = 0.f;

		vec2f n = mt::norm_v2(vec2f(dir.m[ind1], dir.m[ind2]));

		if ( (n.x >= 0) && (n.y >= 0) )
		{
			result = asinf(n.y);
		}

		if ( (n.x <= 0) && (n.y <= 0) )
		{
			result = float(M_PI) + asinf(-n.y);
		}

		if ( (n.x >= 0) && (n.y <= 0) )
		{
			result = float(M_2_PI) - asinf(-n.y);
		}

		if ( (n.x <= 0) && (n.y >= 0) )
		{
			result = float(M_PI) - asinf(n.y);
		}

		return float(M_PI_2) - result;
	}
}