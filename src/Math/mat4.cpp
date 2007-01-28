#include "mat4.h"

namespace	mt
{
	mat4f::mat4f()
	{}

	mat4f::mat4f(const mat4f& _rhs)
		:v0(_rhs.v0)
		,v1(_rhs.v1)
		,v2(_rhs.v2)
		,v3(_rhs.v3)
	{}

	mat4f::mat4f(const vec4f& _a0, const vec4f& _a1, const vec4f& _a2, const vec4f& _a3)
		:v0(_a0)
		,v1(_a1)
		,v2(_a2)
		,v3(_a3)
	{}


	vec4f& mat4f::operator[](int i)
	{
		return v[i];
	}

	const vec4f& mat4f::operator[](int i)const
	{
		return v[i];
	}

	mat4f& mat4f::operator=(const mat4f& _rhs)
	{
		v0 = _rhs.v0;
		v1 = _rhs.v1;
		v2 = _rhs.v2;
		v3 = _rhs.v3;
		return *this;
	}

	mat4f& mat4f::operator=(const mat3f& _rhs)
	{
		v3_0 = _rhs.v0;
		v3_1 = _rhs.v1;
		v3_2 = _rhs.v2;
		f_0 = f_1 = f_2 = f_3 = 0.0f;
		return *this;
	}

	bool cmp_m4_m4(const mat4f& _a, const mat4f& _b)
	{ 
		return	cmp_v4_v4(_a.v0,_b.v0) &&
				cmp_v4_v4(_a.v1,_b.v1) &&
				cmp_v4_v4(_a.v2,_b.v2) &&
				cmp_v4_v4(_a.v3,_b.v3);
	}

	bool operator==(const mat4f& _a, const mat4f& _b)
	{
		return	cmp_m4_m4(_a, _b);
	}

	bool operator!=(const mat4f& _a, const mat4f& _b)
	{
		return !operator==(_a, _b);
	}

	/*	Matrix/Vector Mult	*/
	void mul_m4_v4(vec4f& _out, const mat4f& _m,const vec4f& _v)
	{
		_out[0] = _m[0][0] * _v.x + _m[0][1] * _v.y + _m[0][2] * _v.z + _m[0][3] * _v.w;
		_out[1] = _m[1][0] * _v.x + _m[1][1] * _v.y + _m[1][2] * _v.z + _m[1][3] * _v.w;
		_out[2] = _m[2][0] * _v.x + _m[2][1] * _v.y + _m[2][2] * _v.z + _m[2][3] * _v.w;
		_out[3] = _m[3][0] * _v.x + _m[3][1] * _v.y + _m[3][2] * _v.z + _m[3][3] * _v.w;
	}

	vec4f operator*(const mat4f& m, const vec4f& v)
	{
		vec4f out;
		mul_m4_v4(out,m,v);
		return out;
	}

	void mul_v4_m4(vec4f& _out, const vec4f& _v, const mat4f& _m)
	{
		_out[0] = _m[0][0] * _v.x + _m[1][0] * _v.y + _m[2][0] * _v.z + _m[3][0] * _v.w;
		_out[1] = _m[0][1] * _v.x + _m[1][1] * _v.y + _m[2][1] * _v.z + _m[3][1] * _v.w;
		_out[2] = _m[0][2] * _v.x + _m[1][2] * _v.y + _m[2][2] * _v.z + _m[3][2] * _v.w;
		_out[3] = _m[0][3] * _v.x + _m[1][3] * _v.y + _m[2][3] * _v.z + _m[3][3] * _v.w;
	}

	vec4f operator* (const vec4f& v, const mat4f& m)
	{
		vec4f out;
		mul_v4_m4(out,v,m);
		return out;
	}

	/*	Matrix/Matrix Mult  */
	void mul_m4_m4(mat4f& _out, const mat4f& _a, const mat4f& _b)
	{
		_out[0][0] = _a[0][0] * _b[0][0] + _a[0][1] * _b[1][0]+ _a[0][2] * _b[2][0]+ _a[0][3] * _b[3][0];
		_out[0][1] = _a[0][0] * _b[0][1] + _a[0][1] * _b[1][1]+ _a[0][2] * _b[2][1]+ _a[0][3] * _b[3][1];
		_out[0][2] = _a[0][0] * _b[0][2] + _a[0][1] * _b[1][2]+ _a[0][2] * _b[2][2]+ _a[0][3] * _b[3][2];
		_out[0][3] = _a[0][0] * _b[0][3] + _a[0][1] * _b[1][3]+ _a[0][2] * _b[2][3]+ _a[0][3] * _b[3][3];
		_out[1][0] = _a[1][0] * _b[0][0] + _a[1][1] * _b[1][0]+ _a[1][2] * _b[2][0]+ _a[1][3] * _b[3][0];
		_out[1][1] = _a[1][0] * _b[0][1] + _a[1][1] * _b[1][1]+ _a[1][2] * _b[2][1]+ _a[1][3] * _b[3][1];
		_out[1][2] = _a[1][0] * _b[0][2] + _a[1][1] * _b[1][2]+ _a[1][2] * _b[2][2]+ _a[1][3] * _b[3][2];
		_out[1][3] = _a[1][0] * _b[0][3] + _a[1][1] * _b[1][3]+ _a[1][2] * _b[2][3]+ _a[1][3] * _b[3][3];
		_out[2][0] = _a[2][0] * _b[0][0] + _a[2][1] * _b[1][0]+ _a[2][2] * _b[2][0]+ _a[2][3] * _b[3][0];
		_out[2][1] = _a[2][0] * _b[0][1] + _a[2][1] * _b[1][1]+ _a[2][2] * _b[2][1]+ _a[2][3] * _b[3][1];
		_out[2][2] = _a[2][0] * _b[0][2] + _a[2][1] * _b[1][2]+ _a[2][2] * _b[2][2]+ _a[2][3] * _b[3][2];
		_out[2][3] = _a[2][0] * _b[0][3] + _a[2][1] * _b[1][3]+ _a[2][2] * _b[2][3]+ _a[2][3] * _b[3][3];
		_out[3][0] = _a[3][0] * _b[0][0] + _a[3][1] * _b[1][0]+ _a[3][2] * _b[2][0]+ _a[3][3] * _b[3][0];
		_out[3][1] = _a[3][0] * _b[0][1] + _a[3][1] * _b[1][1]+ _a[3][2] * _b[2][1]+ _a[3][3] * _b[3][1];
		_out[3][2] = _a[3][0] * _b[0][2] + _a[3][1] * _b[1][2]+ _a[3][2] * _b[2][2]+ _a[3][3] * _b[3][2];
		_out[3][3] = _a[3][0] * _b[0][3] + _a[3][1] * _b[1][3]+ _a[3][2] * _b[2][3]+ _a[3][3] * _b[3][3];
	}

	mat4f operator* (const mat4f& _a, const mat4f& _b)
	{
		mat4f out;
		mul_m4_m4(out,_a,_b);
		return out;
	}

	/*  Addition of matrix  */
	void add_m4_m4(mat4f& _out, const mat4f& _a, const mat4f& _b)
	{
		add_v4_v4(_out.v0,_a.v0,_b.v0);
		add_v4_v4(_out.v1,_a.v1,_b.v1);
		add_v4_v4(_out.v2,_a.v2,_b.v2);
		add_v4_v4(_out.v3,_a.v3,_b.v3);
	}

	mat4f operator+(const mat4f& _a, const mat4f& _b)
	{
		mat4f out;
		add_m4_m4(out,_a,_b);
		return out;
	}

	/*	Substraction of Matrix  */
	void sub_m4_m4(mat4f& _out, const mat4f& _a, const mat4f& _b)
	{
		sub_v4_v4(_out.v0,_a.v0,_b.v0);
		sub_v4_v4(_out.v1,_a.v1,_b.v1);
		sub_v4_v4(_out.v2,_a.v2,_b.v2);
		sub_v4_v4(_out.v3,_a.v3,_b.v3);
	}
	
	mat4f operator-(const mat4f& _a, const mat4f& _b)
	{
		mat4f out;
		sub_m4_m4(out,_a,_b);
		return out;
	}

	/*	Scale by Scalar	 */
	void scale_m4_m4(mat4f& _out, const mat4f& _rhs, const mt::vec4f & _val)
	{
		scale_v4_v4(_out.v0,_rhs.v0,_val.x);
		scale_v4_v4(_out.v1,_rhs.v1,_val.y);
		scale_v4_v4(_out.v2,_rhs.v2,_val.z);
		scale_v4_v4(_out.v3,_rhs.v3,_val.w);
	}

	void scale_m4(mat4f& _out, const mt::vec4f & _val)
	{
		scale_m4_m4( _out, _out, _val );
	}

	void	scale_rotate_m4_m4(mat4f& _out, const mat4f& _rhs, const mt::vec3f & _val)
	{
		scale_v4_v4(_out.v0,_rhs.v0,_val.x);
		scale_v4_v4(_out.v1,_rhs.v1,_val.y);
		scale_v4_v4(_out.v2,_rhs.v2,_val.z);
	}

	void	scale_rotate_m4(mat4f& _out, const mt::vec3f &_val)
	{
		scale_rotate_m4_m4( _out, _out, _val );
	}


	/*	Identity	matrix	*/
	void ident_m4(mat4f& _out)
	{
		_out[1][0]=_out[2][0]=_out[3][0]
		=_out[0][1]=_out[2][1]=_out[3][1]
		=_out[0][2]=_out[1][2]=_out[3][2]
		=_out[0][3]=_out[1][3]=_out[2][3]
		=0.0f;

		_out[0][0]
		=_out[1][1]
		=_out[2][2]
		=_out[3][3]
		=1.0f;
	}

	/*	Get/Set Column	*/
	void get_col_m4(vec4f& out, const mat4f& _rhs, int _index)
    {
        out.x = _rhs[0][_index];
        out.y = _rhs[1][_index];
        out.z = _rhs[2][_index];
		out.w = _rhs[3][_index];
    }
 
	vec4f get_col_m4(const mat4f& _rhs, int _index)
	{
		vec4f ret;
		get_col_m4(ret,_rhs,_index);
		return ret;
	}

	void set_col_m4(mat4f& out, const vec4f& _rhs, int _index)
	{
		out[0][_index] = _rhs.x;
		out[1][_index] = _rhs.y;	
		out[2][_index] = _rhs.z;
		out[3][_index] = _rhs.z;
	}

	/*	Transpose  */
	void transpose_m4(mat4f& _out, const mat4f& _rhs)
	{
		_out[0][0] = _rhs[0][0];
		_out[1][1] = _rhs[1][1];
		_out[2][2] = _rhs[2][2];
		_out[3][3] = _rhs[3][3];

		_out[1][0] = _rhs[0][1];
		_out[2][0] = _rhs[0][2];
		_out[2][1] = _rhs[1][2];
		_out[3][0] = _rhs[0][3];
		_out[3][1] = _rhs[1][3]; 
		_out[3][2] = _rhs[2][3]; 

		_out[0][1] = _rhs[1][0];
		_out[0][2] = _rhs[2][0];
		_out[1][2] = _rhs[2][1];
		_out[0][3] = _rhs[3][0];
		_out[1][3] = _rhs[3][1];
		_out[2][3] = _rhs[3][2];
	} 

	mat4f transpose_m4(const mat4f& _rhs)
	{
		mat4f out;
		transpose_m4(out, _rhs);
		return out;
	}

	void extract_m3(mat3f& _out, const mat4f& _rhs)
	{
		_out.v0 = _rhs.v3_0;
		_out.v1 = _rhs.v3_1;
		_out.v2 = _rhs.v3_2;
	}

	mat3f extract_m3(const mat4f& _rhs)
	{
		mat3f out;
		extract_m3(out, _rhs);
		return out;
	}

	float minor_m4(const mat4f& _rhs, const int r0, const int r1, const int r2, const int c0, const int c1, const int c2)
	{
		return	_rhs[r0][c0] * (_rhs[r1][c1] * _rhs[r2][c2] - _rhs[r2][c1] * _rhs[r1][c2]) -
				_rhs[r0][c1] * (_rhs[r1][c0] * _rhs[r2][c2] - _rhs[r2][c0] * _rhs[r1][c2]) +
				_rhs[r0][c2] * (_rhs[r1][c0] * _rhs[r2][c1] - _rhs[r2][c0] * _rhs[r1][c1]);
	}

	float det_m4(const mat4f& _rhs)
	{
		return	_rhs[0][0] * minor_m4(_rhs, 1, 2, 3, 1, 2, 3) -
				_rhs[0][1] * minor_m4(_rhs, 1, 2, 3, 0, 2, 3) +
				_rhs[0][2] * minor_m4(_rhs, 1, 2, 3, 0, 1, 3) -
				_rhs[0][3] * minor_m4(_rhs, 1, 2, 3, 0, 1, 2);
	}

	void adj_m4(mat4f& _out, const mat4f& _rhs)
	{
		_out[0][0] =  minor_m4(_rhs, 1, 2, 3, 1, 2, 3);
		_out[0][1] = -minor_m4(_rhs, 0, 2, 3, 1, 2, 3);
		_out[0][2] =  minor_m4(_rhs, 0, 1, 3, 1, 2, 3);
		_out[0][3] = -minor_m4(_rhs, 0, 1, 2, 1, 2, 3);

		_out[1][0] = -minor_m4(_rhs, 1, 2, 3, 0, 2, 3);
		_out[1][1] =  minor_m4(_rhs, 0, 2, 3, 0, 2, 3);
		_out[1][2] = -minor_m4(_rhs, 0, 1, 3, 0, 2, 3);
		_out[1][3] =  minor_m4(_rhs, 0, 1, 2, 0, 2, 3);

		_out[2][0] =  minor_m4(_rhs, 1, 2, 3, 0, 1, 3);
		_out[2][1] = -minor_m4(_rhs, 0, 2, 3, 0, 1, 3);
		_out[2][2] =  minor_m4(_rhs, 0, 1, 3, 0, 1, 3);
		_out[2][3] = -minor_m4(_rhs, 0, 1, 2, 0, 1, 3);

		_out[3][0] = -minor_m4(_rhs, 1, 2, 3, 0, 1, 2);
		_out[3][1] =  minor_m4(_rhs, 0, 2, 3, 0, 1, 2);
		_out[3][2] = -minor_m4(_rhs, 0, 1, 3, 0, 1, 2);
		_out[3][3] =  minor_m4(_rhs, 0, 1, 2, 0, 1, 2);
	}

	mat4f adj_m4(const mat4f& _rhs)
	{
		mat4f out;
		adj_m4(out, _rhs);
		return out;
	}

	/*	Inverse of Matrix  */
	void inv_m4(mat4f& _out, const mat4f& _rhs)
	{ 
		adj_m4(_out, _rhs);
		float	invdet = 1.0f / det_m4(_out);
		scale_m4(_out, mt::vec4f(invdet,invdet,invdet,invdet) );
	}

	mat4f inv_m4(const mat4f& _rhs)
	{
		mat4f out;
		inv_m4(out, _rhs);
		return out;
	}
}