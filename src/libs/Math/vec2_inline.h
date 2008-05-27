#	include <assert.h>

#	include <math.h>

#	include <float.h>
#	include <algorithm>

namespace mt
{
	MATH_INLINE vec2f::vec2f() 
	{
	}

	MATH_INLINE vec2f::vec2f(float _x, float _y)
		:x(_x)
		,y(_y)
	{
	}

	MATH_INLINE vec2f::vec2f(const vec2f&	_v)
		:x(_v.x)
		,y(_v.y)
	{}
	
	MATH_INLINE vec2f& vec2f::operator=(const vec2f& _rhs)
    {
        x = _rhs.x;	
        y = _rhs.y;	
        return *this;
    }

	MATH_INLINE const float vec2f::operator[](int i) const
	{
		assert(i < 2);
		return m[i];
	}

	MATH_INLINE float& vec2f::operator[](int i)
	{
		assert(i < 2);
		return m[i];
	}

	MATH_INLINE vec2f& vec2f::operator+=(const vec2f& _rhs)
	{
		x += _rhs.x;
		y += _rhs.y;
		return *this;
	}

	MATH_INLINE vec2f& vec2f::operator-=(const vec2f& _rhs)
	{
		x -= _rhs.x;
		y -= _rhs.y;
		return *this;
	}

	MATH_INLINE vec2f& vec2f::operator/=(const float _rhs)
	{
		assert(_rhs != 0.0f);
		operator *= (1.f/_rhs); 
		return *this;
	}

	MATH_INLINE vec2f& vec2f::operator*=(const float _rhs)
	{
		x *= _rhs;
		y *= _rhs;
		return *this;
	}

	MATH_INLINE float vec2f::sqrlength() const
	{
		return	x * x + y * y;
	}

	MATH_INLINE float vec2f::length() const
	{
		return sqrtf(sqrlength());
	}

	MATH_INLINE float vec2f::normalize()
	{
		float l = length();
		if (l < FLT_EPSILON)
		{
			return 0.0f;
		}
		float invLength = 1.0f / l;
		x *= invLength;
		y *= invLength;

		return l;
	}

	MATH_INLINE float length_v2_v2(const vec2f& _a, const vec2f& _b)
	{
		vec2f c = _a - _b;
		return c.length();
	}

	MATH_INLINE bool cmp_v2_v2(const vec2f& _a, const vec2f& _b, float eps)
	{
		return (fabsf(_a.x - _b.x) < eps) && (fabsf(_a.y - _b.y) < eps);
	}

	MATH_INLINE bool operator==(const vec2f& _a, const vec2f& _b)
	{
		return cmp_v2_v2(_a, _b);
	}

	MATH_INLINE bool operator!=(const vec2f& _a, const vec2f& _b)
	{
		return !operator==(_a, _b);
	}

	MATH_INLINE void add_v2_v2(vec2f& _out,const vec2f& _a, const vec2f& _b)
	{
		_out.x = _a.x + _b.x;
		_out.y = _a.y + _b.y;
	}

	MATH_INLINE vec2f operator+(const vec2f& _a, const vec2f& _b) 
	{
		vec2f out;
		add_v2_v2(out, _a, _b);
		return	out;
	}

	MATH_INLINE void sub_v2_v2(vec2f& _out,const vec2f& _a, const vec2f& _b)
	{
		_out.x = _a.x - _b.x;
		_out.y = _a.y - _b.y;
	}

	MATH_INLINE vec2f operator-(const vec2f& _a, const vec2f& _b) 
	{
		vec2f out;
		sub_v2_v2(out, _a, _b);
		return	out;
	}

	MATH_INLINE void scale_v2_v2(vec2f& _out, const vec2f& _a, float _val)
	{
		_out.x = _a.x * _val;
		_out.y = _a.y * _val;
	}

	MATH_INLINE vec2f operator*(const vec2f& _rhs, float _val) 
	{
		vec2f out;
		scale_v2_v2(out, _rhs, _val);
		return out;
	}

	MATH_INLINE vec2f operator*(float _val, const vec2f& _rhs )
	{
		vec2f out;
		scale_v2_v2(out, _rhs, _val);
		return out;
	}

	MATH_INLINE vec2f operator/(const vec2f& _rhs, float _val) 
	{
		return operator*(_rhs, 1/_val);
	}

	MATH_INLINE void neg_v2(vec2f& _out)
	{
		_out.x = -_out.x;
		_out.y = -_out.y;
	}

	MATH_INLINE vec2f operator-(vec2f _rhs)
	{
		neg_v2(_rhs);
		return _rhs;
	}

	MATH_INLINE float dot_v2_v2(const vec2f &a, const vec2f &b)
	{
		return a.x * b.x + a.y * b.y ;
	}

	MATH_INLINE void norm_v2(vec2f& _out, const vec2f& _rhs)
	{
		_out = _rhs / _rhs.length();
	}

	MATH_INLINE float norm_v2_f(vec2f& _out, const vec2f& _rhs)
	{
		float l = _rhs.length();
		_out = _rhs / l;
		return l;
	}

	MATH_INLINE vec2f norm_v2(const vec2f& _rhs)
	{
		vec2f out;
		norm_v2(out, _rhs);
		return out;
	}

	MATH_INLINE void norm_safe_v2(vec2f& _out, const vec2f& _rhs, float _err)
	{
		float len = _rhs.length();
		if (len > _err)
        {
			_out = _rhs / len;
		}
		else
		{
			_out = _rhs;
		}
	}

	MATH_INLINE vec2f norm_safe_v2(const vec2f& _rhs)
	{
		vec2f out;
		norm_safe_v2(out, _rhs);
		return out;
	}
	
	MATH_INLINE void perp(vec2f&	_out, const vec2f& _rhs)
	{
		_out = vec2f(-_rhs.y,_rhs.x);
	}

	MATH_INLINE vec2f perp(const vec2f&	_rhs)
	{
		return	vec2f(-_rhs.y,_rhs.x);
	}

	MATH_INLINE float is_left_v2(const vec2f& p0, const vec2f& p1, const vec2f& p2)
	{
		return ((p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y));
	}

	MATH_INLINE float pseudo_cross_v2(const mt::vec2f& a, const mt::vec2f& b)
	{
		return a.x * b.y - a.y * b.x;
	}

	MATH_INLINE mt::vec2f pseudo_cross_v2(const mt::vec2f& a, float s)
	{
		vec2f v(s * a.y, -s * a.x);
		return v;
	}

	MATH_INLINE mt::vec2f pseudo_cross_v2(float s, const mt::vec2f& a)
	{
		vec2f v(s * a.y, -s * a.x);
		return v;
	}

	MATH_INLINE void project_v2_v2(const vec2f& a, const vec2f& b, mt::vec2f& result) 
	{
		float dp = dot_v2_v2(a, b);

		result.x = dp * b.x;
		result.y = dp * b.y;
	}

	MATH_INLINE vec2f slerp_v2_v2(const vec2f& a, const vec2f& b, float t)
	{
		float s = sqrtf(a.sqrlength() * b.sqrlength());
		float cos = mt::dot_v2_v2(a,b) / s;
		if( cos > 1.0f ) cos = 1.0f;
		else if( cos < -1.0f ) cos = -1.0f;
		float theta = acosf( cos );

		if (theta != 0.0f)
		{
			float d = 1.0f / sinf( theta );
			float s0 = sinf((1.0f - t) * theta);
			float s1 = sinf(t * theta);  
			return mt::vec2f((a.x * s0 + b.x * s1) * d, (a.y * s0 + b.y * s1) * d);
		}
		
		return a;
	}

	MATH_INLINE void rotate_v2(vec2f& _out, float _angle)
	{
		vec2f _v = _out;
		float sin_angle = sinf(_angle);
		float cos_angle = cosf(_angle);
		_out.x = cos_angle * _v.x - sin_angle * _v.y;
		_out.y = cos_angle * _v.y + sin_angle * _v.x;
	}

	MATH_INLINE vec2f b2Min(const vec2f& a, const vec2f& b)
	{
		vec2f c;
		c.x = (((a.x) < (b.x)) ? (a.x) : (b.x));
		c.y = (((a.y) < (b.y)) ? (a.y) : (b.y));
		return c;
	}

	MATH_INLINE vec2f b2Max(const vec2f& a, const vec2f& b)
	{
		vec2f c;
		c.x = (((a.x) > (b.x)) ? (a.x) : (b.x));
		c.y = (((a.y) > (b.y)) ? (a.y) : (b.y));
		return c;
	}
}
