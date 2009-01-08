#include <assert.h>

namespace	mt
{ 
	MATH_INLINE mat3f::mat3f()
	{}

	MATH_INLINE mat3f::mat3f(const mat3f&	_rhs)
	: v0(_rhs.v0)
	, v1(_rhs.v1)
	, v2(_rhs.v2)
	{
	}

	MATH_INLINE vec3f& mat3f::operator [](int i)
	{
		return (&v0)[i];
	}

	MATH_INLINE const vec3f& mat3f::operator [](int i)const
	{
		return (&v0)[i];
	}

	MATH_INLINE mat3f& mat3f::operator=(const mat3f& _rhs)
	{
		v0 = _rhs.v0;
		v1 = _rhs.v1;
		v2 = _rhs.v2;

		return *this;
	}

	MATH_INLINE float * mat3f::buff()
	{
		return v0.buff();
	}

	MATH_INLINE float * buff_m3( const mat3f& _a )
	{
		return (float *)&_a.v0.x;
	}

	MATH_INLINE bool cmp_m3_m3(const mat3f& _a, const mat3f& _b)
	{ 
		return	cmp_v3_v3(_a.v0,_b.v0) &&
				cmp_v3_v3(_a.v1,_b.v1) &&
				cmp_v3_v3(_a.v2,_b.v2);
	}

	MATH_INLINE bool operator==(const mat3f& _a, const mat3f& _b)
	{
		return	cmp_m3_m3(_a, _b);
	}

	MATH_INLINE bool operator!=(const mat3f& _a, const mat3f& _b)
	{
		return !operator==(_a, _b);
	}

	/*	Matrix/Vector Mult	*/
	MATH_INLINE void mul_m3_v3(vec3f& _out, const mat3f& _m, const vec3f& _v)
	{ 
		_out.x = dot_v3_v3(_m.v0,_v);
		_out.y = dot_v3_v3(_m.v1,_v);
		_out.z = dot_v3_v3(_m.v2,_v);
	}

	MATH_INLINE vec3f operator*(const mat3f& m, const vec3f& v)
	{
		vec3f out;
		mul_m3_v3(out,m,v);
		return out;
	}

	/*	Vector/Matrix Mult	*/
	MATH_INLINE void mul_v3_m3(vec3f& _out, const vec3f& _v, const mat3f& _m)
	{
		_out.x = _m.v0.x * _v.x + _m.v1.x * _v.y + _m.v2.z * _v.z;
		_out.y = _m.v0.y * _v.x + _m.v1.y * _v.y + _m.v2.z * _v.z;
		_out.z = _m.v0.z * _v.x + _m.v1.z * _v.y + _m.v2.z * _v.z;
	}

	MATH_INLINE vec3f operator* (const vec3f& _v, const mat3f& _m)
	{
		vec3f out;
		mul_v3_m3(out,_v,_m);
		return out;
	}

	MATH_INLINE void mul_v2_m3(vec2f& _out, const vec2f& _v, const mat3f& _m)
	{
		mul_v2_m3_r( _out, _v, _m );

		_out.x += _m.v2.x;
		_out.y += _m.v2.y;
	}

	MATH_INLINE vec2f operator * (const vec2f& _v, const mat3f& _m)
	{
		vec2f out;
		mul_v2_m3( out,_v,_m);
		return out;
	}

	MATH_INLINE void mul_v2_m3_r(vec2f& _out, const vec2f& _v, const mat3f& _m)
	{
		_out.x = _m.v0.x * _v.x + _m.v1.x * _v.y;
		_out.y = _m.v0.y * _v.x + _m.v1.y * _v.y;
	}

	MATH_INLINE void mul_m3_m3_i( vec3f & _out, const vec3f & _a, const mat3f& _b )
	{
		_out.x = _a.x * _b.v0.x + _a.y * _b.v1.x + _a.z * _b.v2.x; 
		_out.y = _a.x * _b.v0.y + _a.y * _b.v1.y + _a.z * _b.v2.y;
		_out.z = _a.x * _b.v0.z + _a.y * _b.v1.z + _a.z * _b.v2.z;
	}

	/*	Matrix/Matrix Mult  */
	MATH_INLINE void mul_m3_m3(mat3f& _out, const mat3f& _a, const mat3f& _b)
	{
		mul_m3_m3_i( _out.v0, _a.v0, _b );
		mul_m3_m3_i( _out.v1, _a.v1, _b );
		mul_m3_m3_i( _out.v2, _a.v2, _b );
	}

	MATH_INLINE mat3f operator* (const mat3f& _a, const mat3f& _b)
	{
		mat3f out;
		mul_m3_m3(out,_a,_b);
		return out;
	}

	/*  Addition of matrix  */
	MATH_INLINE void add_m3_m3(mat3f& _out, const mat3f& _a, const mat3f& _b)
	{
		add_v3_v3(_out.v0,_a.v0,_b.v0);
		add_v3_v3(_out.v1,_a.v1,_b.v1);
		add_v3_v3(_out.v2,_a.v2,_b.v2);
	}

	MATH_INLINE mat3f operator+(const mat3f& _a, const mat3f& _b)
	{
		mat3f out;
		add_m3_m3(out,_a,_b);
		return out;
	}

	/*	Substraction of Matrix  */
	MATH_INLINE void sub_m3_m3(mat3f& _out, const mat3f& _a, const mat3f& _b)
	{
		sub_v3_v3(_out.v0,_a.v0,_b.v0);
		sub_v3_v3(_out.v1,_a.v1,_b.v1);
		sub_v3_v3(_out.v2,_a.v2,_b.v2);
	}

	MATH_INLINE mat3f operator-(const mat3f& _a, const mat3f& _b)
	{
		mat3f out;
		sub_m3_m3(out,_a,_b);
		return out;
	}

	/*	Scale by Scalar	 */
	MATH_INLINE void scale_m3_s(mat3f& _out, const mat3f& _rhs, float _val)
	{
		scale_v3_v3(_out.v0,_rhs.v0,_val);
		scale_v3_v3(_out.v1,_rhs.v1,_val);
		scale_v3_v3(_out.v2,_rhs.v2,_val);
	}

	MATH_INLINE mat3f operator*(const mat3f& _rhs, float _val)
	{
		mat3f out;
		scale_m3_s(out,_rhs,_val);
		return out;
	}

	MATH_INLINE void scale_m3_s(mat3f& _out, float _val)
	{
		scale_v3_v3(_out.v0,_out.v0,_val);
		scale_v3_v3(_out.v1,_out.v1,_val);
		scale_v3_v3(_out.v2,_out.v2,_val);
	}

	MATH_INLINE void scale_m3_m3(mat3f& _out, const mat3f& _rhs, const mt::vec3f & _val)
	{
		scale_v3_v3(_out.v0,_rhs.v0,_val.x);
		scale_v3_v3(_out.v1,_rhs.v1,_val.y);
		scale_v3_v3(_out.v2,_rhs.v2,_val.z);
	}

	MATH_INLINE void scale_m3(mat3f& _out, const mt::vec3f &_val)
	{
		scale_m3_m3( _out, _out, _val );
	}


	/*	Transpose Matrix  */
	MATH_INLINE void transpose_m3(mat3f& _out, const mat3f& _rhs)
	{
		_out.v0.x = _rhs.v0.x;
		_out.v1.y = _rhs.v1.y;
		_out.v2.z = _rhs.v2.z;

		_out.v1.x = _rhs.v0.y;
		_out.v2.x = _rhs.v0.z;
		_out.v2.y = _rhs.v1.z;

		_out.v0.y = _rhs.v1.x;
		_out.v0.z = _rhs.v2.x;
		_out.v1.z = _rhs.v2.y;
	}

	MATH_INLINE mat3f transpose_m3(const mat3f& _rhs)
	{
		mat3f out;
		transpose_m3(out, _rhs);
		return out;
	}

	/*	Determinant	of Matrix  */
	MATH_INLINE float det_m3(const mat3f& _rhs)
	{
		float cof00 = _rhs.v1[1] * _rhs.v2[2] - _rhs.v1[2] * _rhs.v2[1];
		float cof10 = _rhs.v1[2] * _rhs.v2[0] - _rhs.v1[0] * _rhs.v2[2];
		float cof20 = _rhs.v1[0] * _rhs.v2[1] - _rhs.v1[1] * _rhs.v2[0];

		return _rhs.v0.x * cof00 + _rhs.v0.y * cof10 + _rhs.v0.z * cof20;
	}

	/*	Inverse of Matrix  */
	MATH_INLINE void inv_m3(mat3f& _out, const mat3f& _rhs)
	{ 
	    _out.v0[0] = _rhs.v1[1]*_rhs.v2[2] - _rhs.v1[2]*_rhs.v2[1];
        _out.v0[1] = _rhs.v0[2]*_rhs.v2[1] - _rhs.v0[1]*_rhs.v2[2];
        _out.v0[2] = _rhs.v0[1]*_rhs.v1[2] - _rhs.v0[2]*_rhs.v1[1];
        _out.v1[0] = _rhs.v1[2]*_rhs.v2[0] - _rhs.v1[0]*_rhs.v2[2];
        _out.v1[1] = _rhs.v0[0]*_rhs.v2[2] - _rhs.v0[2]*_rhs.v2[0];
        _out.v1[2] = _rhs.v0[2]*_rhs.v1[0] - _rhs.v0[0]*_rhs.v1[2];
        _out.v2[0] = _rhs.v1[0]*_rhs.v2[1] - _rhs.v1[1]*_rhs.v2[0];
        _out.v2[1] = _rhs.v0[1]*_rhs.v2[0] - _rhs.v0[0]*_rhs.v2[1];
        _out.v2[2] = _rhs.v0[0]*_rhs.v1[1] - _rhs.v0[1]*_rhs.v1[0];

        float det = _rhs.v0[0]*_out.v0[0] + _rhs.v0[1]*_out.v1[0]+ _rhs.v0[2]*_out.v2[0];

		assert(fabsf(det) >= 0.000001f);

        float idet = 1.0f / det;

        for( int i = 0; i < 3; i++ )
        {
            _out.v0[i] *= idet;
			_out.v1[i] *= idet;
			_out.v2[i] *= idet;
        }
	}

	/*	Identity	matrix	*/
	MATH_INLINE void ident_m3(mat3f& _out)
	{
		ident_v3( _out.v0 );
		ident_v3( _out.v1 );
		ident_v3( _out.v2 );
		
		_out.v0.x = 1.f;
		_out.v1.y = 1.f;
		_out.v2.z = 1.f;
	}

	/*	Get/Set Column	*/
	MATH_INLINE void get_col_m3(vec3f& out, const mat3f& _rhs, int _index)
	{
		out.x = _rhs.v0[_index];
		out.y = _rhs.v1[_index];
		out.z = _rhs.v2[_index];
	}

	MATH_INLINE vec3f get_col_m3(const mat3f& _rhs, int _index)
	{
		vec3f ret;
		get_col_m3(ret,_rhs,_index);
		return ret;
	}

	MATH_INLINE void set_col_m3(mat3f& out, const vec3f& _rhs, int _index)
	{
		out.v0[_index] = _rhs.x;
		out.v1[_index] = _rhs.y;	
		out.v2[_index] = _rhs.z;
	}

	/*	Set Up, Dir, Left	*/
	MATH_INLINE void set_m3_from_axes(mat3f& _out, const vec3f& _a, const vec3f& _b, const vec3f& _c)
	{
		_out.v0 = _a;
		_out.v1 = _b;
		_out.v2 = _c;
	}

	MATH_INLINE mat3f set_m3_from_axes(const vec3f& _a, const vec3f& _b, const vec3f& _c)
	{
		mat3f m;
		set_m3_from_axes( m, _a, _b, _c );
		return	m;
	}
}