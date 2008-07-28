#include <assert.h>

namespace	mt
{ 
	MATH_INLINE mat3f::mat3f()
	{}

	MATH_INLINE mat3f::mat3f(const mat3f&	_rhs)
		:v0(_rhs.v0)
		,v1(_rhs.v1)
		,v2(_rhs.v2)
	{}

	MATH_INLINE mat3f::mat3f(const vec3f&	_v0, const vec3f&	_v1, const vec3f&	_v2)
		:v0(_v0)
		,v1(_v1)
		,v2(_v2)
	{}

	MATH_INLINE vec3f& mat3f::operator [](int i)
	{
		return v[i];
	}

	MATH_INLINE const vec3f& mat3f::operator [](int i)const
	{
		return v[i];
	}

	MATH_INLINE mat3f& mat3f::operator=(const mat3f& _rhs)
	{
		v0 = _rhs.v0;
		v1 = _rhs.v1;
		v2 = _rhs.v2;
		return *this;
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
		_out.x = _m[0][0] * _v.x + _m[1][0] * _v.y + _m[2][0] * _v.z;
		_out.y = _m[0][1] * _v.x + _m[1][1] * _v.y + _m[2][1] * _v.z;
		_out.z = _m[0][2] * _v.x + _m[1][2] * _v.y + _m[2][2] * _v.z;
	}

	MATH_INLINE vec3f operator* (const vec3f& _v, const mat3f& _m)
	{
		vec3f out;
		mul_v3_m3(out,_v,_m);
		return out;
	}

	MATH_INLINE void mul_v2_m3(vec2f& _out, const vec2f& _v, const mat3f& _m)
	{
		_out.x = _m.m[0] * _v.x + _m.m[3] * _v.y + _m.m[6];
		_out.y = _m.m[1] * _v.x + _m.m[4] * _v.y + _m.m[7];
	}

	MATH_INLINE void mul_v2_m3_r(vec2f& _out, const vec2f& _v, const mat3f& _m)
	{
		_out.x = _m[0][0] * _v.x + _m[1][0] * _v.y;
		_out.y = _m[0][1] * _v.x + _m[1][1] * _v.y;
	}

	/*	Matrix/Matrix Mult  */
	MATH_INLINE void mul_m3_m3(mat3f& _out, const mat3f& _a, const mat3f& _b)
	{
		_out.v0.x = _a.m[0] * _b.m[0] + _a.m[1] * _b.m[3] + _a.m[2] * _b.m[6]; 
		_out.v0.y = _a.m[0] * _b.m[1] + _a.m[1] * _b.m[4] + _a.m[2] * _b.m[7];
		_out.v0.z = _a.m[0] * _b.m[2] + _a.m[1] * _b.m[5] + _a.m[2] * _b.m[8];
		_out.v1.x = _a.m[3] * _b.m[0] + _a.m[4] * _b.m[3] + _a.m[5] * _b.m[6];
		_out.v1.y = _a.m[3] * _b.m[1] + _a.m[4] * _b.m[4] + _a.m[5] * _b.m[7];
		_out.v1.z = _a.m[3] * _b.m[2] + _a.m[4] * _b.m[5] + _a.m[5] * _b.m[8];
		_out.v2.x = _a.m[6] * _b.m[0] + _a.m[7] * _b.m[3] + _a.m[8] * _b.m[6];
		_out.v2.y = _a.m[6] * _b.m[1] + _a.m[7] * _b.m[4] + _a.m[8] * _b.m[7];
		_out.v2.z = _a.m[6] * _b.m[2] + _a.m[7] * _b.m[5] + _a.m[8] * _b.m[8];
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
		_out[0][0] = _rhs[0][0];
		_out[1][1] = _rhs[1][1];
		_out[2][2] = _rhs[2][2];

		_out[1][0] = _rhs[0][1];
		_out[2][0] = _rhs[0][2];
		_out[2][1] = _rhs[1][2];

		_out[0][1] = _rhs[1][0];
		_out[0][2] = _rhs[2][0];
		_out[1][2] = _rhs[2][1];
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
		float cof00 = _rhs[1][1] * _rhs[2][2] - _rhs[1][2] * _rhs[2][1];
		float cof10 = _rhs[1][2] * _rhs[2][0] - _rhs[1][0] * _rhs[2][2];
		float cof20 = _rhs[1][0] * _rhs[2][1] - _rhs[1][1] * _rhs[2][0];

		return _rhs[0][0] * cof00 + _rhs[0][1] * cof10 + _rhs[0][2] * cof20;
	}

	/*	Inverse of Matrix  */
	MATH_INLINE void inv_m3(mat3f& _out, const mat3f& _rhs)
	{ 
	    _out[0][0] = _rhs[1][1]*_rhs[2][2] - _rhs[1][2]*_rhs[2][1];
        _out[0][1] = _rhs[0][2]*_rhs[2][1] - _rhs[0][1]*_rhs[2][2];
        _out[0][2] = _rhs[0][1]*_rhs[1][2] - _rhs[0][2]*_rhs[1][1];
        _out[1][0] = _rhs[1][2]*_rhs[2][0] - _rhs[1][0]*_rhs[2][2];
        _out[1][1] = _rhs[0][0]*_rhs[2][2] - _rhs[0][2]*_rhs[2][0];
        _out[1][2] = _rhs[0][2]*_rhs[1][0] - _rhs[0][0]*_rhs[1][2];
        _out[2][0] = _rhs[1][0]*_rhs[2][1] - _rhs[1][1]*_rhs[2][0];
        _out[2][1] = _rhs[0][1]*_rhs[2][0] - _rhs[0][0]*_rhs[2][1];
        _out[2][2] = _rhs[0][0]*_rhs[1][1] - _rhs[0][1]*_rhs[1][0];

        float det = _rhs[0][0]*_out[0][0] + _rhs[0][1]*_out[1][0]+ _rhs[0][2]*_out[2][0];

		assert(fabsf(det) >= 0.000001f);

        float idet = 1.0f / det;

        for( int i = 0; i < 3; i++ )
        {
            for( int j = 0; j < 3; j++ )
			{
                _out[i][j] *= idet;
			}
        }

	}

	/*	Identity	matrix	*/
	MATH_INLINE void ident_m3(mat3f& out)
	{
		out[1][0]=out[2][0]=out[0][1]=out[2][1]=out[0][2]=out[1][2]=0.0f;
		out[0][0]=out[1][1]=out[2][2]=1.0f;
	}

	/*	Get/Set Column	*/
	MATH_INLINE void get_col_m3(vec3f& out, const mat3f& _rhs, int _index)
	{
		out.x = _rhs[0][_index];
		out.y = _rhs[1][_index];
		out.z = _rhs[2][_index];
	}

	MATH_INLINE vec3f get_col_m3(const mat3f& _rhs, int _index)
	{
		vec3f ret;
		get_col_m3(ret,_rhs,_index);
		return ret;
	}

	MATH_INLINE void set_col_m3(mat3f& out, const vec3f& _rhs, int _index)
	{
		out[0][_index] = _rhs.x;
		out[1][_index] = _rhs.y;	
		out[2][_index] = _rhs.z;
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
		return	mat3f(_a, _b, _c);
	}
}