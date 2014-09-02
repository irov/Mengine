#	include "utils.h"

#	include <math.h>

namespace mt
{
	MATH_METHOD_INLINE vec3f::vec3f() 
	{
	}

	MATH_METHOD_INLINE vec3f::vec3f(float _x, float _y, float _z)
		:x(_x)
		,y(_y)
		,z(_z)
	{
	}

	MATH_METHOD_INLINE vec3f::vec3f(const vec3f& _v)
		:x(_v.x)
		,y(_v.y)
		,z(_v.z)
	{}

	MATH_METHOD_INLINE vec3f::vec3f(const vec2f& _v, float _z)
		:x(_v.x)
		,y(_v.y)
		,z(_z)
	{}

	MATH_METHOD_INLINE vec3f& vec3f::operator=(const vec3f& _rhs)
    {
        x = _rhs.x;
        y = _rhs.y;
        z = _rhs.z;     

        return *this;
    }

	MATH_METHOD_INLINE float vec3f::operator[](int i) const
	{
		return (&x)[i];
	}

	MATH_METHOD_INLINE float& vec3f::operator[](int i)
	{
		return (&x)[i];
	}

	MATH_METHOD_INLINE vec3f& vec3f::operator+=(const vec3f& _rhs)
	{
		x += _rhs.x;
		y += _rhs.y;
		z += _rhs.z;

		return *this;
	}

	MATH_METHOD_INLINE vec3f& vec3f::operator-=(const vec3f& _rhs)
	{
		x -= _rhs.x;
		y -= _rhs.y;
		z -= _rhs.z;

		return *this;
	}

	MATH_METHOD_INLINE vec3f& vec3f::operator /= (const float _rhs)
	{
		operator *= (1.f/_rhs); 

		return *this;
	}

	MATH_METHOD_INLINE vec3f& vec3f::operator *= (const float _rhs)
	{
		x *= _rhs;
		y *= _rhs;
		z *= _rhs;

		return *this;
	}

	MATH_METHOD_INLINE float vec3f::sqrlength()	const
	{
		return	x * x + y * y + z * z;
	}

	MATH_METHOD_INLINE float vec3f::length()	const
	{
		float sqrlen = this->sqrlength();
        float len = sqrtf(sqrlen);

		return len;
	}

	MATH_METHOD_INLINE vec2f vec3f::to_vec2f() const
	{
		return mt::vec2f(x, y);
	}

	MATH_METHOD_INLINE float * vec3f::buff()
	{
		return &x;
	}

	MATH_METHOD_INLINE const float * vec3f::buff() const
	{
		return &x;
	}

	MATH_FUNCTION_INLINE void ident_v3( vec3f& _a )
	{
		_a.x = 0.f;
		_a.y = 0.f;
		_a.z = 0.f;
	}

	MATH_FUNCTION_INLINE float length_v3_v3(const vec3f& _a, const vec3f& _b)
	{
		vec3f c = _a - _b;
        float len = c.length();

		return len;
	}

	MATH_FUNCTION_INLINE float sqrlength_v3_v3(const vec3f& _a, const vec3f& _b)
	{
		vec3f c = _a - _b;
		float sqrlength = c.sqrlength();

		return sqrlength;
	}

	MATH_FUNCTION_INLINE float length_v3(const vec3f& _a)
	{
        float len = _a.length();

		return len;
	}

	MATH_FUNCTION_INLINE bool cmp_v3_v3(const vec3f& _a, const vec3f& _b)
	{
		return mt::cmp_f_f( _a.x, _b.x ) == true && 
			mt::cmp_f_f( _a.y, _b.y ) == true && 
			mt::cmp_f_f( _a.z, _b.z ) == true;
	}

	MATH_FUNCTION_INLINE bool operator==(const vec3f& _a, const vec3f& _b) 
	{
		return cmp_v3_v3(_a, _b);
	}

	MATH_FUNCTION_INLINE bool operator!=(const vec3f& _a, const vec3f& _b) 
	{
		return !operator==(_a, _b);
	}

	/*	Addition of vecs  */
	MATH_FUNCTION_INLINE void add_v3_v3(vec3f& _out,const vec3f& _a, const vec3f& _b)
	{
		_out.x = _a.x + _b.x;
		_out.y = _a.y + _b.y;
		_out.z = _a.z + _b.z;
	}

	MATH_FUNCTION_INLINE vec3f operator+(const vec3f& _a, const vec3f& _b) 
	{
		vec3f out;
		add_v3_v3(out, _a, _b);
		return out;
	}

	/*	Addition of vecs  */
	MATH_FUNCTION_INLINE void sub_v3_v3(vec3f& _out,const vec3f& _a, const vec3f& _b)
	{
		_out.x = _a.x - _b.x;
		_out.y = _a.y - _b.y;
		_out.z = _a.z - _b.z;
	}

	MATH_FUNCTION_INLINE vec3f operator-(const vec3f& _a, const vec3f& _b) 
	{
		vec3f out;
		sub_v3_v3(out, _a, _b);
		return out;
	}

	/*	Scale of vecs  */
	MATH_FUNCTION_INLINE void scale_v3_v3(vec3f& _out, const vec3f& _a, float _val)
	{
		_out.x = _a.x * _val;
		_out.y = _a.y * _val;
		_out.z = _a.z * _val;
	}

	MATH_FUNCTION_INLINE vec3f operator*(const vec3f& _rhs, const float _val) 
	{
		vec3f out;
		scale_v3_v3(out, _rhs, _val);
		return out;
	}

	MATH_FUNCTION_INLINE vec3f operator*(const float _val, const vec3f& _rhs)
	{
		vec3f out;
		scale_v3_v3(out, _rhs, _val);
		return out;
	}

	MATH_FUNCTION_INLINE vec3f operator/(const vec3f& _rhs, const float _val) 
	{
		return operator*(_rhs, 1/_val);
	}

	/*	Negation */
	MATH_FUNCTION_INLINE void neg_v3(vec3f& _out)
	{
		_out.x = -_out.x;
		_out.y = -_out.y;
		_out.z = -_out.z;
	}

	MATH_FUNCTION_INLINE vec3f operator-(vec3f _rhs)
	{
		neg_v3(_rhs);
		return _rhs;
	}

	/*	Dot	Product			*/
	MATH_FUNCTION_INLINE float dot_v3_v3(const vec3f &a, const vec3f &b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

    MATH_FUNCTION_INLINE float dot_v3_v2( const vec3f &a, const vec2f &b )
    {
        return a.x * b.x + a.y * b.y;
    }

	MATH_FUNCTION_INLINE float dot_v3_v3_clamp(const vec3f &a, const vec3f &b)
	{
		float dot = a.x * b.x + a.y * b.y + a.z * b.z;
		if(dot > 1.0f) dot = 1.0f;
		if(dot < -1.0f) dot = -1.0f;
		return dot;
	}

	/*	Reflection Vector	*/
	MATH_FUNCTION_INLINE void reflect_v3_v3(vec3f& _out, const vec3f& _a, const vec3f& _b)
	{
		_out = _a-_b*dot_v3_v3(_a,_b)*2.0f;
	}	

	MATH_FUNCTION_INLINE vec3f reflect_v3_v3(const vec3f& _a, const vec3f& _b)
	{
		vec3f out;
		reflect_v3_v3(out, _a, _b);
		return out;
	}	
	
	/*	Cross Product	*/
	MATH_FUNCTION_INLINE void cross_v3_v3(vec3f& _out, const vec3f& _a, const vec3f& _b)
	{
		_out.x = _a.y*_b.z - _a.z*_b.y;
		_out.y = _a.z*_b.x - _a.x*_b.z;
		_out.z = _a.x*_b.y - _a.y*_b.x;
	}

	MATH_FUNCTION_INLINE vec3f cross_v3_v3(const vec3f& _a, const vec3f& _b)
	{
		vec3f out;
		cross_v3_v3(out, _a, _b);
		return out;
	}

	MATH_FUNCTION_INLINE void cross_v3_v3_norm(vec3f& _out, const vec3f& _a, const vec3f& _b)
	{
		_out.x = _a.y*_b.z - _a.z*_b.y;
		_out.y = _a.z*_b.x - _a.x*_b.z;
		_out.z = _a.x*_b.y - _a.y*_b.x;

		norm_v3( _out , _out );
	}
	
	/*	Normalize Vector*/
	MATH_FUNCTION_INLINE void norm_v3(vec3f& _out, const vec3f& _rhs)
	{
		_out = _rhs / _rhs.length();
	}

	MATH_FUNCTION_INLINE vec3f norm_v3(const vec3f& _rhs)
	{
		vec3f out;
		norm_v3(out, _rhs);
		return out;
	}

	MATH_FUNCTION_INLINE void norm_safe_v3(vec3f& out, const vec3f& _rhs, float err)
	{
		float len = _rhs.length();
		if (len > err)
        {
			out = _rhs / len;
		}
		else
		{
			out = _rhs;
		}
	}

	MATH_FUNCTION_INLINE vec3f norm_safe_v3(const vec3f& _rhs)
	{
		vec3f out;
		norm_safe_v3(out, _rhs);
		return out;
	}

	MATH_FUNCTION_INLINE float get_axis_angle(const vec3f& dir, int axis)
	{
		int ind1 = 1; //axis == 0
		int ind2 = 2; //axis == 0

		if( axis == 1 )	
		{
			ind1 = 0; 
			ind2 = 2;
		}
		else if( axis == 2 )
		{
			ind1 = 0; 
			ind2 = 1;
		}

		float result = 0.f;

		vec2f d(dir[ind1], dir[ind2]);
		vec2f n;
		mt::norm_v2_v2(n, d);

		if( (n.x >= 0) && (n.y >= 0) )
		{
			result = asinf(n.y);
		}

		if( (n.x <= 0) && (n.y <= 0) )
		{
			result = m_pi + asinf(-n.y);
		}

		if( (n.x >= 0) && (n.y <= 0) )
		{
			result = m_half_pi - asinf(-n.y);
		}

		if( (n.x <= 0) && (n.y >= 0) )
		{
			result = m_pi - asinf(n.y);
		}

		return m_half_pi - result;
	}
}
