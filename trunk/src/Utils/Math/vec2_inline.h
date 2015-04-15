#	include "angle.h"

#	include "utils.h"

#	include <math.h>

namespace mt
{
	MENGINE_MATH_METHOD_INLINE vec2f::vec2f() 
	{}

	MENGINE_MATH_METHOD_INLINE vec2f::vec2f( float _x, float _y )
		:x(_x)
		,y(_y)
	{}

	MENGINE_MATH_METHOD_INLINE vec2f::vec2f( const vec2f & _v )
		:x(_v.x)
		,y(_v.y)
	{}

	MENGINE_MATH_METHOD_INLINE vec2f & vec2f::operator = ( const vec2f & _rhs )
    {
        x = _rhs.x;	
        y = _rhs.y;	

        return *this;
    }

	MENGINE_MATH_METHOD_INLINE float vec2f::operator [] ( size_t i ) const
	{
		return (&x)[i];
	}

	MENGINE_MATH_METHOD_INLINE float& vec2f::operator [] ( size_t i )
	{
		return (&x)[i];
	}

	MENGINE_MATH_METHOD_INLINE vec2f& vec2f::operator+=(const vec2f& _rhs)
	{
		x += _rhs.x;
		y += _rhs.y;

		return *this;
	}

	MENGINE_MATH_METHOD_INLINE vec2f& vec2f::operator-=(const vec2f& _rhs)
	{
		x -= _rhs.x;
		y -= _rhs.y;

		return *this;
	}

	MENGINE_MATH_METHOD_INLINE vec2f& vec2f::operator/=(const float _rhs)
	{
		operator *= (1.f/_rhs); 

		return *this;
	}

	MENGINE_MATH_METHOD_INLINE vec2f& vec2f::operator*=(const float _rhs)
	{
		x *= _rhs;
		y *= _rhs;

		return *this;
	}

	MENGINE_MATH_METHOD_INLINE vec2f& vec2f::operator/=(const vec2f & _rhs)
	{
		x /= _rhs.x;
		y /= _rhs.y;

		return *this;
	}

	MENGINE_MATH_METHOD_INLINE vec2f& vec2f::operator*=(const vec2f & _rhs)
	{
		x *= _rhs.x;
		y *= _rhs.y;

		return *this;
	}

	MENGINE_MATH_METHOD_INLINE float vec2f::sqrlength() const
	{
		return	x * x + y * y;
	}

	MENGINE_MATH_METHOD_INLINE float vec2f::length() const
	{
		float sql = sqrlength();

		return sqrtf(sql);
	}

	MENGINE_MATH_METHOD_INLINE float * vec2f::buff()
	{
		return &x;
	}

	MENGINE_MATH_METHOD_INLINE const float * vec2f::buff() const
	{
		return &x;
	}

	MENGINE_MATH_FUNCTION_INLINE void ident_v2(vec2f& _a)
	{
		_a.x = 0.f;
		_a.y = 0.f;
	}

	MENGINE_MATH_FUNCTION_INLINE float length_v2(const vec2f& _a)
	{
		float length = _a.length();

		return length;
	}
	
	MENGINE_MATH_FUNCTION_INLINE float sqrlength_v2_v2(const vec2f& _a, const vec2f& _b)
	{
		vec2f c = _a - _b;
		float sqrlength = c.sqrlength();

		return sqrlength;
	}

	MENGINE_MATH_FUNCTION_INLINE float length_v2_v2(const vec2f& _a, const vec2f& _b)
	{
		vec2f c = _a - _b;
		float length = c.length();

		return length;
	}

	MENGINE_MATH_FUNCTION_INLINE bool cmp_v2_v2(const vec2f& _a, const vec2f& _b )
	{
		return mt::equal_f_f( _a.x, _b.x ) == true &&
			mt::equal_f_f( _a.y, _b.y ) == true;
	}

	MENGINE_MATH_FUNCTION_INLINE bool operator==(const vec2f& _a, const vec2f& _b)
	{
		return cmp_v2_v2(_a, _b);
	}

	MENGINE_MATH_FUNCTION_INLINE bool operator!=(const vec2f& _a, const vec2f& _b)
	{
		return !operator==(_a, _b);
	}

	MENGINE_MATH_FUNCTION_INLINE void add_v2_v2(vec2f& _out,const vec2f& _a, const vec2f& _b)
	{
		_out.x = _a.x + _b.x;
		_out.y = _a.y + _b.y;
	}

	MENGINE_MATH_FUNCTION_INLINE vec2f operator+(const vec2f& _a, const vec2f& _b) 
	{
		vec2f out;
		add_v2_v2(out, _a, _b);
		return	out;
	}

	MENGINE_MATH_FUNCTION_INLINE void sub_v2_v2(vec2f& _out,const vec2f& _a, const vec2f& _b)
	{
		_out.x = _a.x - _b.x;
		_out.y = _a.y - _b.y;
	}

	MENGINE_MATH_FUNCTION_INLINE void dir_v2_v2(vec2f& _out,const vec2f& _a, const vec2f& _b)
	{
		vec2f out;
		mt::sub_v2_v2( out, _a, _b );
		mt::norm_v2_v2( _out, out );
	}

	MENGINE_MATH_FUNCTION_INLINE vec2f operator-(const vec2f& _a, const vec2f& _b) 
	{
		vec2f out;
		sub_v2_v2(out, _a, _b);
		return	out;
	}

	MENGINE_MATH_FUNCTION_INLINE void mul_v2_f(vec2f& _out, const vec2f& _a, float _val)
	{
		_out.x = _a.x * _val;
		_out.y = _a.y * _val;
	}

	MENGINE_MATH_FUNCTION_INLINE void mul_v2_v2(vec2f& _out, const vec2f& _a, const vec2f& _b)
	{
		_out.x = _a.x * _b.x;
		_out.y = _a.y * _b.y;
	}

	MENGINE_MATH_FUNCTION_INLINE void div_v2_v2(vec2f& _out, const vec2f& _a, const vec2f& _b)
	{
		_out.x = _a.x / _b.x;
		_out.y = _a.y / _b.y;
	}

	MENGINE_MATH_FUNCTION_INLINE vec2f operator*(const vec2f& _a, const vec2f& _b)
	{
		vec2f out;
		mul_v2_v2(out, _a, _b);
		return out;
	}

	MENGINE_MATH_FUNCTION_INLINE vec2f operator*(const vec2f& _rhs, float _val) 
	{
		vec2f out;
		mul_v2_f(out, _rhs, _val);
		return out;
	}

	MENGINE_MATH_FUNCTION_INLINE vec2f operator * (float _val, const vec2f& _rhs)
	{
		vec2f out;
		mul_v2_f(out, _rhs, _val);
		return out;
	}

	MENGINE_MATH_FUNCTION_INLINE vec2f operator/(const vec2f& _a, const vec2f& _b)
	{
		vec2f out;
		div_v2_v2(out, _a, _b);
		return out;
	}

	MENGINE_MATH_FUNCTION_INLINE vec2f operator / (const vec2f& _rhs, float _val) 
	{
		return operator*(_rhs, 1/_val);
	}

	MENGINE_MATH_FUNCTION_INLINE vec2f operator / (float _val, const vec2f& _rhs)
	{
		return vec2f(_val / _rhs.x, _val / _rhs.y);
	}

	MENGINE_MATH_FUNCTION_INLINE void inv_v2(vec2f& _out, const vec2f & _in)
	{
		_out.x = 1.f / _in.x;
		_out.y = 1.f / _in.y;
	}

	MENGINE_MATH_FUNCTION_INLINE void neg_v2(vec2f& _out)
	{
		_out.x = -_out.x;
		_out.y = -_out.y;
	}

	MENGINE_MATH_FUNCTION_INLINE vec2f operator-(vec2f _rhs)
	{
		neg_v2(_rhs);
		return _rhs;
	}

	MENGINE_MATH_FUNCTION_INLINE float dot_v2_v2(const vec2f &a, const vec2f &b)
	{
		return a.x * b.x + a.y * b.y;
	}

	MENGINE_MATH_FUNCTION_INLINE void norm_v2_v2(vec2f& _out, const vec2f& _rhs)
	{
		_out = _rhs / _rhs.length();
	}

	MENGINE_MATH_FUNCTION_INLINE float norm_v2_f(vec2f& _out, const vec2f& _rhs)
	{
		float l = _rhs.length();

		_out = _rhs / l;

		return l;
	}

	MENGINE_MATH_FUNCTION_INLINE vec2f norm_v2(const vec2f& _rhs)
	{
		vec2f out;

		norm_v2_v2(out, _rhs);

		return out;
	}

	MENGINE_MATH_FUNCTION_INLINE void norm_safe_v2(vec2f& _out, const vec2f& _rhs, float _err)
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

	MENGINE_MATH_FUNCTION_INLINE vec2f norm_safe(const vec2f& _rhs)
	{
		vec2f out;
		norm_safe_v2(out, _rhs);

		return out;
	}

	MENGINE_MATH_FUNCTION_INLINE void perp_v2(vec2f&	_out, const vec2f& _rhs)
	{
		_out = vec2f(-_rhs.y, _rhs.x);
	}

	MENGINE_MATH_FUNCTION_INLINE vec2f perp(const vec2f&	_rhs)
	{
		vec2f pv;
		perp_v2(pv, _rhs);

		return pv;
	}
	
	MENGINE_MATH_FUNCTION_INLINE void perp_left_v2(vec2f& _out, const vec2f& _rhs)
	{
		_out = vec2f(_rhs.y,-_rhs.x);
	}

	MENGINE_MATH_FUNCTION_INLINE vec2f perp_left(const vec2f& _rhs)
	{
		vec2f pv;
		perp_left_v2(pv, _rhs);

		return pv;
	}

	MENGINE_MATH_FUNCTION_INLINE float is_left_v2(const vec2f& p0, const vec2f& p1, const vec2f& p2)
	{
		return ((p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y));
	}

	MENGINE_MATH_FUNCTION_INLINE float pseudo_cross_v2(const mt::vec2f& a, const mt::vec2f& b)
	{
		return a.x * b.y - a.y * b.x;
	}	

	MENGINE_MATH_FUNCTION_INLINE void project_v2_v2(vec2f & out, const vec2f& a, const vec2f& b) 
	{
		float dp = dot_v2_v2(a, b);

		out.x = dp * a.x;
		out.y = dp * a.y;
	}

	MENGINE_MATH_FUNCTION_INLINE vec2f slerp_v2_v2(const vec2f& a, const vec2f& b, float t)
	{
		float s = sqrtf(a.sqrlength() * b.sqrlength());
		float cos = mt::dot_v2_v2(a,b) / s;
		if( cos > 1.0f ) cos = 1.0f;
		else if( cos < -1.0f ) cos = -1.0f;
		float theta = acosf( cos );

		if (theta != 0.0f)
		{
			float d = 1.0f / sinf_fast( theta );
			float s0 = sinf_fast((1.0f - t) * theta);
			float s1 = sinf_fast(t * theta);

			mt::vec2f s2((a.x * s0 + b.x * s1) * d, (a.y * s0 + b.y * s1) * d);

			return s2;
		}
		
		return a;
	}

	MENGINE_MATH_FUNCTION_INLINE void rotate_v2(vec2f& _out, float _angle)
	{
		vec2f _v = _out;
		float sin_angle = sinf_fast(_angle);
		float cos_angle = cosf_fast(_angle);
		_out.x = cos_angle * _v.x - sin_angle * _v.y;
		_out.y = cos_angle * _v.y + sin_angle * _v.x;
	}

	MENGINE_MATH_FUNCTION_INLINE void direction( mt::vec2f & _vec, float _angle )
	{
		float cos_angle = cosf_fast( _angle );
		float sin_angle = sinf_fast( _angle );

		_vec.x = cos_angle;
		_vec.y = sin_angle;
	}

	MENGINE_MATH_FUNCTION_INLINE float signed_angle(const mt::vec2f & _vec)
	{
		float len = _vec.length();

		if( len < 0.00001f )
		{			
			return 0.f;
		}

		float cos = _vec.x / len;

		float x = mt::acos32( cos );

		if( _vec.y > 0.f )
		{
			return -x;
		}

		return x;
	}
}
