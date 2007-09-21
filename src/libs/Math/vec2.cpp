#include "vec2.h"
#include "mat2.h"

namespace	mt
{
	vec2f::vec2f() 
	{
	}

	vec2f::vec2f(float _x, float _y)
		:x(_x)
		,y(_y)
	{
	}

	vec2f::vec2f(const vec2f&	_v)
		:x(_v.x)
		,y(_v.y)
	{}
	
	vec2f& vec2f::operator=(const vec2f& _rhs)
    {
        x = _rhs.x;	
        y = _rhs.y;	
        return *this;
    }

	const float vec2f::operator[](int i) const
	{
		assert(i < 2);
		return m[i];
	}

	float&	vec2f::operator[](int i)
	{
		assert(i < 2);
		return m[i];
	}

	vec2f&		vec2f::operator+=(const vec2f&	_rhs)
	{
		x += _rhs.x;
		y += _rhs.y;
		return *this;
	}

	vec2f&		vec2f::operator-=(const vec2f&	_rhs)
	{
		x -= _rhs.x;
		y -= _rhs.y;
		return *this;
	}

	vec2f&		vec2f::operator/=(const float _rhs)
	{
		assert(_rhs != 0.0f);
		operator *= (1.f/_rhs); 
		return *this;
	}

	vec2f&		vec2f::operator*=(const float _rhs)
	{
		x *= _rhs;
		y *= _rhs;
		return *this;
	}

	float vec2f::sqrlength()	const
	{
		return	x * x + y * y;
	}

	float vec2f::length()	const
	{
		return sqrtf(sqrlength());
	}

	float	length_v2_v2(const vec2f& _a, const vec2f& _b)
	{
		vec2f c = _a - _b;
		return c.length();
	}

	bool	cmp_v2_v2(const vec2f& _a, const vec2f& _b, float eps)
	{
		return	(fabs(_a.x - _b.x) < eps) && (fabs(_a.y - _b.y) < eps);
	}

	bool operator==(const vec2f& _a, const vec2f& _b)
	{
		return cmp_v2_v2(_a, _b);
	}

	bool operator!=(const vec2f& _a, const vec2f& _b)
	{
		return !operator==(_a, _b);
	}

	void	add_v2_v2(vec2f& _out,const vec2f& _a, const vec2f& _b)
	{
		_out.x = _a.x + _b.x;
		_out.y = _a.y + _b.y;
	}

	vec2f operator+(const vec2f& _a, const vec2f& _b) 
	{
		vec2f	out;
		add_v2_v2(out, _a, _b);
		return	out;
	}

	void	sub_v2_v2(vec2f& _out,const vec2f& _a, const vec2f& _b)
	{
		_out.x = _a.x - _b.x;
		_out.y = _a.y - _b.y;
	}

	vec2f operator-(const vec2f& _a, const vec2f& _b) 
	{
		vec2f	out;
		sub_v2_v2(out, _a, _b);
		return	out;
	}

	void	scale_v2_v2(vec2f& _out, const vec2f& _a, float _val)
	{
		_out.x = _a.x * _val;
		_out.y = _a.y * _val;
	}

	vec2f	operator*(const vec2f& _rhs, float _val) 
	{
		vec2f	out;
		scale_v2_v2(out, _rhs, _val);
		return out;
	}

	vec2f	operator*(float _val, const vec2f& _rhs )
	{
		vec2f	out;
		scale_v2_v2(out, _rhs, _val);
		return out;
	}

	vec2f	operator/(const vec2f& _rhs, float _val) 
	{
		return operator*(_rhs, 1/_val);
	}

	void	neg_v2(vec2f& _out)
	{
		_out.x = -_out.x;
		_out.y = -_out.y;
	}

	vec2f operator-(vec2f _rhs)
	{
		neg_v2(_rhs);
		return	_rhs;
	}

	float	dot_v2_v2(const vec2f &a, const vec2f &b)
	{
		return a.x * b.x + a.y * b.y ;
	}

	void	norm_v2(vec2f& _out, const vec2f& _rhs)
	{
		_out = _rhs / _rhs.length();
	}

	float	norm_v2_f(vec2f& _out, const vec2f& _rhs)
	{
		float l = _rhs.length();
		_out = _rhs / l;
		return l;
	}

	vec2f	norm_v2(const vec2f& _rhs)
	{
		vec2f	out;
		norm_v2(out, _rhs);
		return	out;
	}

	void	norm_safe_v2(vec2f& _out, const vec2f& _rhs, float _err)
	{
		float	len = _rhs.length();
		if (len > _err)
        {
			_out = _rhs / len;
			return;
		}
		_out = _rhs;
	}

	vec2f	norm_safe_v2(const vec2f& _rhs)
	{
		vec2f	out;
		norm_safe_v2(out, _rhs);
		return	out;
	}
	
	void	perp(vec2f&	_out, const vec2f& _rhs)
	{
		_out = vec2f(-_rhs.y,_rhs.x);
	}

	vec2f	perp(const vec2f&	_rhs)
	{
		return	vec2f(-_rhs.y,_rhs.x);
	}

	float	is_left_v2(const vec2f& p0, const vec2f& p1, const vec2f& p2)
	{
		return ((p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y));
	}

	float	pseudo_cross_v2(const mt::vec2f& a, const mt::vec2f& b)
	{
		return a.x * b.y - a.y * b.x;
	}

	void	project_v2_v2(const vec2f& a, const vec2f& b, mt::vec2f& result) 
	{
		float dp = dot_v2_v2(a, b);

		result.x = dp * b.x;
		result.y = dp * b.y;
	}

	bool	slerp_v2_v2(const vec2f& a, const vec2f& b, float t, vec2f& result)
	{
	//	float m_to = pseudo_cross_v2(b,a) < 0.0f ? 1.0f : -1.0f;
		float m_to = (dot_v2_v2(b,perp(a)) < 0.0f) ? 1.0f : -1.0f;


		float _sin = sin(t * m_to);
		float _cos = cos(t * m_to);

		result.x = _sin * a.y + _cos * a.x;
		result.y = _cos * a.y - _sin * a.x;

	//	float mn_to = pseudo_cross_v2(b, result) < 0.0f ? 1.0f : -1.0f;
		float mn_to = (dot_v2_v2(b,perp(result)) < 0.0f ) ? 1.0f : -1.0f;


		if ( m_to * mn_to < 0.0f )
		{
			result = b;
			return true;
		}

		return false;
	}
}