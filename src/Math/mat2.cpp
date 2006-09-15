#include "mat2.h"

namespace	mt
{ 
	mat2f::mat2f()
	{}

	mat2f::mat2f(float angle)
	{
		float c = cosf(angle), s = sinf(angle);
		v[0].x = c; v[1].x = -s;
		v[0].y = s; v[1].y = c;
	}


	mat2f::mat2f(const mat2f&	_rhs)
		:v0(_rhs.v0)
		,v1(_rhs.v1)
	{}

	mat2f::mat2f(const vec2f&	_v0, const vec2f&	_v1)
		:v0(_v0)
		,v1(_v1)
	{}

	vec2f& mat2f::operator [](int i)
	{
		return v[i];
	}

	const vec2f& mat2f::operator [](int i)const
	{
		return v[i];
	}

	mat2f& mat2f::operator=(const mat2f& _rhs)
	{
		v0 = _rhs.v0;
		v1 = _rhs.v1;
		return *this;
	}	

	bool cmp_m2_m2(const mat2f& _a, const mat2f& _b)
	{ 
		return	cmp_v2_v2(_a.v0,_b.v0) &&
			cmp_v2_v2(_a.v1,_b.v1);
	}

	bool operator==(const mat2f& _a, const mat2f& _b)
	{
		return	cmp_m2_m2(_a, _b);
	}

	bool operator!=(const mat2f& _a, const mat2f& _b)
	{
		return !operator==(_a, _b);
	}

	/*	Matrix/Vector Mult	*/
	void mul_m2_v2(vec2f& _out, const mat2f& _m, const vec2f& _v)
	{ 
		_out.x = dot_v2_v2(_m.v0,_v);
		_out.y = dot_v2_v2(_m.v1,_v);
	}

	vec2f operator*(const mat2f& m, const vec2f& v)
	{
		vec2f out;
		mul_m2_v2(out,m,v);
		return out;
	}

	/*	Vector/Matrix Mult	*/
	void mul_v2_m2(vec2f& _out, const vec2f& _v, const mat2f& _m)
	{
		_out.x = _m[0][0] * _v.x + _m[1][0] * _v.y;
		_out.y = _m[0][1] * _v.x + _m[1][1] * _v.y;
	}

	vec2f operator* (const vec2f& _v, const mat2f& _m)
	{
		vec2f out;
		mul_v2_m2(out,_v,_m);
		return out;
	}

	/*	Matrix/Matrix Mult  */
	void mul_m2_m2(mat2f& _out, const mat2f& _a, const mat2f& _b)
	{
		_out.v0.x = _a[0][0] * _b[0][0] + _a[0][1] * _b[1][0]; 
		_out.v0.y = _a[0][0] * _b[0][1] + _a[0][1] * _b[1][1];
		_out.v1.x = _a[1][0] * _b[0][0] + _a[1][1] * _b[1][0];
		_out.v1.y = _a[1][0] * _b[0][1] + _a[1][1] * _b[1][1];
	}

	mat2f operator* (const mat2f& _a, const mat2f& _b)
	{
		mat2f out;
		mul_m2_m2(out,_a,_b);
		return out;
	}

	/*  Addition of matrix  */
	void add_m2_m2(mat2f& _out, const mat2f& _a,const mat2f& _b)
	{
		add_v2_v2(_out.v0,_a.v0,_b.v0);
		add_v2_v2(_out.v1,_a.v1,_b.v1);
	}

	mat2f operator+(const mat2f& _a, const mat2f& _b)
	{
		mat2f out;
		add_m2_m2(out,_a,_b);
		return out;
	}

	/*	Substraction of Matrix  */
	void sub_m2_m2(mat2f& _out, const mat2f& _a, const mat2f& _b)
	{
		sub_v2_v2(_out.v0,_a.v0,_b.v0);
		sub_v2_v2(_out.v1,_a.v1,_b.v1);
	}

	mat2f operator-(const mat2f& _a, const mat2f& _b)
	{
		mat2f out;
		sub_m2_m2(out,_a,_b);
		return out;
	}

	/*	Scale by Scalar	 */
	void scale_m2_s(mat2f& _out, const mat2f& _rhs, float _val)
	{
		scale_v2_v2(_out.v0,_rhs.v0,_val);
		scale_v2_v2(_out.v1,_rhs.v1,_val);
	}

	mat2f operator*(const mat2f& _rhs, float _val)
	{
		mat2f out;
		scale_m2_s(out,_rhs,_val);
		return out;
	}

	void scale_m2_s(mat2f& _out, float _val)
	{
		scale_v2_v2(_out.v0,_out.v0,_val);
		scale_v2_v2(_out.v1,_out.v1,_val);
	}

	/*	Transpose Matrix  */
	void transpose_m2(mat2f& _out, const mat2f& _rhs)
	{
		_out.v0.x = _rhs.v0.x;
		_out.v0.y = _rhs.v1.x;

		_out.v1.x = _rhs.v0.y;
		_out.v1.y = _rhs.v1.y;
	}

	mat2f transpose_m2(const mat2f& _rhs)
	{
		mat2f out;
		transpose_m2(out, _rhs);
		return out;
	}
}