namespace mt
{
	MATH_INLINE mat4f::mat4f()
	{}

	MATH_INLINE mat4f::mat4f(const mat4f& _rhs)
		:v0(_rhs.v0)
		,v1(_rhs.v1)
		,v2(_rhs.v2)
		,v3(_rhs.v3)
	{}

	MATH_INLINE mat4f::mat4f(const vec4f& _a0, const vec4f& _a1, const vec4f& _a2, const vec4f& _a3)
		:v0(_a0)
		,v1(_a1)
		,v2(_a2)
		,v3(_a3)
	{}

	MATH_INLINE vec4f& mat4f::operator[](int i)
	{
		return v[i];
	}

	MATH_INLINE const vec4f& mat4f::operator[](int i)const
	{
		return v[i];
	}

	MATH_INLINE mat4f& mat4f::operator=(const mat4f& _rhs)
	{
		v0 = _rhs.v0;
		v1 = _rhs.v1;
		v2 = _rhs.v2;
		v3 = _rhs.v3;
		return *this;
	}

	MATH_INLINE mat4f& mat4f::operator=(const mat3f& _rhs)
	{
		v3_0 = _rhs.v0;
		v3_1 = _rhs.v1;
		v3_2 = _rhs.v2;
		f_0 = f_1 = f_2 = f_3 = 0.0f;
		return *this;
	}

	MATH_INLINE bool cmp_m4_m4(const mat4f& _a, const mat4f& _b)
	{ 
		return	cmp_v4_v4(_a.v0,_b.v0) &&
				cmp_v4_v4(_a.v1,_b.v1) &&
				cmp_v4_v4(_a.v2,_b.v2) &&
				cmp_v4_v4(_a.v3,_b.v3);
	}

	MATH_INLINE bool operator==(const mat4f& _a, const mat4f& _b)
	{
		return	cmp_m4_m4(_a, _b);
	}

	MATH_INLINE bool operator!=(const mat4f& _a, const mat4f& _b)
	{
		return !operator==(_a, _b);
	}

	//////////////////////////////////////////////////////////////////////////
	MATH_INLINE void mul_m4_v3(vec3f& _out, const mat4f& _m,const vec3f& _v)
	{
		_out[0] = _m[0][0] * _v.x + _m[0][1] * _v.y + _m[0][2] * _v.z + _m[0][3];
		_out[1] = _m[1][0] * _v.x + _m[1][1] * _v.y + _m[1][2] * _v.z + _m[1][3];
		_out[2] = _m[2][0] * _v.x + _m[2][1] * _v.y + _m[2][2] * _v.z + _m[2][3];
	}

	//////////////////////////////////////////////////////////////////////////
	MATH_INLINE vec3f operator*(const mat4f& m, const vec3f& v)
	{
		vec3f out;
		mul_m4_v3(out,m,v);
		return out;
	}

	//////////////////////////////////////////////////////////////////////////
	MATH_INLINE void mul_v3_m4(vec3f& _out, const vec3f& _v,const mat4f& _m)
	{
		_out[0] = _m[0][0] * _v.x + _m[1][0] * _v.y + _m[2][0] * _v.z + _m[3][0];
		_out[1] = _m[0][1] * _v.x + _m[1][1] * _v.y + _m[2][1] * _v.z + _m[3][1];
		_out[2] = _m[0][2] * _v.x + _m[1][2] * _v.y + _m[2][2] * _v.z + _m[3][2];
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_INLINE void mul_v2_m4(vec2f& _out, const vec2f& _v,const mat4f& _m)
	{
		_out[0] = _m[0][0] * _v.x + _m[1][0] * _v.y + _m[3][0];
		_out[1] = _m[0][1] * _v.x + _m[1][1] * _v.y + _m[3][1];
		_out[2] = _m[0][2] * _v.x + _m[1][2] * _v.y + _m[3][2];
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_INLINE vec3f operator*(const vec3f& v, const mat4f& m)
	{
		vec3f out;
		mul_v3_m4(out,v,m);
		return out;
	}
	/*	Matrix/Vector Mult	*/
	MATH_INLINE void mul_m4_v4(vec4f& _out, const mat4f& _m,const vec4f& _v)
	{
		_out[0] = _m[0][0] * _v.x + _m[0][1] * _v.y + _m[0][2] * _v.z + _m[0][3] * _v.w;
		_out[1] = _m[1][0] * _v.x + _m[1][1] * _v.y + _m[1][2] * _v.z + _m[1][3] * _v.w;
		_out[2] = _m[2][0] * _v.x + _m[2][1] * _v.y + _m[2][2] * _v.z + _m[2][3] * _v.w;
		_out[3] = _m[3][0] * _v.x + _m[3][1] * _v.y + _m[3][2] * _v.z + _m[3][3] * _v.w;
	}

	MATH_INLINE vec4f operator*(const mat4f& m, const vec4f& v)
	{
		vec4f out;
		mul_m4_v4(out,m,v);
		return out;
	}

	MATH_INLINE void mul_v4_m4(vec4f& _out, const vec4f& _v, const mat4f& _m)
	{
		_out[0] = _m[0][0] * _v.x + _m[1][0] * _v.y + _m[2][0] * _v.z + _m[3][0] * _v.w;
		_out[1] = _m[0][1] * _v.x + _m[1][1] * _v.y + _m[2][1] * _v.z + _m[3][1] * _v.w;
		_out[2] = _m[0][2] * _v.x + _m[1][2] * _v.y + _m[2][2] * _v.z + _m[3][2] * _v.w;
		_out[3] = _m[0][3] * _v.x + _m[1][3] * _v.y + _m[2][3] * _v.z + _m[3][3] * _v.w;
	}

	MATH_INLINE vec4f operator* (const vec4f& v, const mat4f& m)
	{
		vec4f out;
		mul_v4_m4(out,v,m);
		return out;
	}
	/*	Matrix/Matrix Mult  */
	MATH_INLINE void mul_m4_m4(mat4f& _out, const mat4f& _a, const mat4f& _b)
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
	/*	Matrix/Matrix Mult  */
	MATH_INLINE void mul_m4_m3(mat4f& _out, const mat4f& _a, const mat3f& _b)
	{
		_out[0][0] = _a[0][0] * _b[0][0] + _a[0][1] * _b[1][0]+ _a[0][2] * _b[2][0];
		_out[0][1] = _a[0][0] * _b[0][1] + _a[0][1] * _b[1][1]+ _a[0][2] * _b[2][1];
		_out[0][2] = _a[0][0] * _b[0][2] + _a[0][1] * _b[1][2]+ _a[0][2] * _b[2][2];
		_out[1][0] = _a[1][0] * _b[0][0] + _a[1][1] * _b[1][0]+ _a[1][2] * _b[2][0];
		_out[1][1] = _a[1][0] * _b[0][1] + _a[1][1] * _b[1][1]+ _a[1][2] * _b[2][1];
		_out[1][2] = _a[1][0] * _b[0][2] + _a[1][1] * _b[1][2]+ _a[1][2] * _b[2][2];
		_out[2][0] = _a[2][0] * _b[0][0] + _a[2][1] * _b[1][0]+ _a[2][2] * _b[2][0];
		_out[2][1] = _a[2][0] * _b[0][1] + _a[2][1] * _b[1][1]+ _a[2][2] * _b[2][1];
		_out[2][2] = _a[2][0] * _b[0][2] + _a[2][1] * _b[1][2]+ _a[2][2] * _b[2][2];
		_out[3][0] = _a[3][0];
		_out[3][1] = _a[3][1];
		_out[3][2] = _a[3][2];
		_out[0][3] = _a[0][3];
		_out[1][3] = _a[1][3];
		_out[2][3] = _a[2][3];
		_out[3][3] = _a[3][3];
	}

	MATH_INLINE mat4f operator* (const mat4f& _a, const mat4f& _b)
	{
		mat4f out;
		mul_m4_m4(out,_a,_b);
		return out;
	}
	
	MATH_INLINE mat4f operator* (const mat4f& _a, const mat3f& _b)
	{
		mat4f out;
		mul_m4_m3(out,_a,_b);
		return out;
	}

	/*  Addition of matrix  */
	MATH_INLINE void add_m4_m4(mat4f& _out, const mat4f& _a, const mat4f& _b)
	{
		add_v4_v4(_out.v0,_a.v0,_b.v0);
		add_v4_v4(_out.v1,_a.v1,_b.v1);
		add_v4_v4(_out.v2,_a.v2,_b.v2);
		add_v4_v4(_out.v3,_a.v3,_b.v3);
	}

	MATH_INLINE mat4f operator+(const mat4f& _a, const mat4f& _b)
	{
		mat4f out;
		add_m4_m4(out,_a,_b);
		return out;
	}

	/*	Substraction of Matrix  */
	MATH_INLINE void sub_m4_m4(mat4f& _out, const mat4f& _a, const mat4f& _b)
	{
		sub_v4_v4(_out.v0,_a.v0,_b.v0);
		sub_v4_v4(_out.v1,_a.v1,_b.v1);
		sub_v4_v4(_out.v2,_a.v2,_b.v2);
		sub_v4_v4(_out.v3,_a.v3,_b.v3);
	}
	
	MATH_INLINE mat4f operator-(const mat4f& _a, const mat4f& _b)
	{
		mat4f out;
		sub_m4_m4(out,_a,_b);
		return out;
	}

	/*	Scale by Scalar	 */
	MATH_INLINE void scale_m4_m4(mat4f& _out, const mat4f& _rhs, const mt::vec4f & _val)
	{
		scale_v4_v4(_out.v0,_rhs.v0,_val.x);
		scale_v4_v4(_out.v1,_rhs.v1,_val.y);
		scale_v4_v4(_out.v2,_rhs.v2,_val.z);
		scale_v4_v4(_out.v3,_rhs.v3,_val.w);
	}

	MATH_INLINE void scale_m4(mat4f& _out, const mt::vec4f & _val)
	{
		scale_m4_m4( _out, _out, _val );
	}

	MATH_INLINE void scale_rotate_m4_m4(mat4f& _out, const mat4f& _rhs, const mt::vec3f & _val)
	{
		scale_v4_v4(_out.v0,_rhs.v0,_val.x);
		scale_v4_v4(_out.v1,_rhs.v1,_val.y);
		scale_v4_v4(_out.v2,_rhs.v2,_val.z);
	}

	MATH_INLINE void scale_rotate_m4(mat4f& _out, const mt::vec3f &_val)
	{
		scale_rotate_m4_m4( _out, _out, _val );
	}


	/*	Identity	matrix	*/
	MATH_INLINE void ident_m4(mat4f& _out)
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
	MATH_INLINE void get_col_m4(vec4f& out, const mat4f& _rhs, int _index)
    {
        out.x = _rhs[0][_index];
        out.y = _rhs[1][_index];
        out.z = _rhs[2][_index];
		out.w = _rhs[3][_index];
    }
 
	MATH_INLINE vec4f get_col_m4(const mat4f& _rhs, int _index)
	{
		vec4f ret;
		get_col_m4(ret,_rhs,_index);
		return ret;
	}

	MATH_INLINE void set_col_m4(mat4f& out, const vec4f& _rhs, int _index)
	{
		out[0][_index] = _rhs.x;
		out[1][_index] = _rhs.y;	
		out[2][_index] = _rhs.z;
		out[3][_index] = _rhs.z;
	}

	/*	Transpose  */
	MATH_INLINE void transpose_m4(mat4f& _out, const mat4f& _rhs)
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

	MATH_INLINE mat4f transpose_m4(const mat4f& _rhs)
	{
		mat4f out;
		transpose_m4(out, _rhs);
		return out;
	}

	MATH_INLINE void extract_m3(mat3f& _out, const mat4f& _rhs)
	{
		_out.v0 = _rhs.v3_0;
		_out.v1 = _rhs.v3_1;
		_out.v2 = _rhs.v3_2;
	}

	MATH_INLINE mat3f extract_m3(const mat4f& _rhs)
	{
		mat3f out;
		extract_m3(out, _rhs);
		return out;
	}

	MATH_INLINE float minor_m4(const mat4f& _rhs, const int r0, const int r1, const int r2, const int c0, const int c1, const int c2)
	{
		return	_rhs[r0][c0] * (_rhs[r1][c1] * _rhs[r2][c2] - _rhs[r2][c1] * _rhs[r1][c2]) -
				_rhs[r0][c1] * (_rhs[r1][c0] * _rhs[r2][c2] - _rhs[r2][c0] * _rhs[r1][c2]) +
				_rhs[r0][c2] * (_rhs[r1][c0] * _rhs[r2][c1] - _rhs[r2][c0] * _rhs[r1][c1]);
	}

	MATH_INLINE float det_m4(const mat4f& _rhs)
	{
		return	_rhs[0][0] * minor_m4(_rhs, 1, 2, 3, 1, 2, 3) -
				_rhs[0][1] * minor_m4(_rhs, 1, 2, 3, 0, 2, 3) +
				_rhs[0][2] * minor_m4(_rhs, 1, 2, 3, 0, 1, 3) -
				_rhs[0][3] * minor_m4(_rhs, 1, 2, 3, 0, 1, 2);
	}

	MATH_INLINE void adj_m4(mat4f& _out, const mat4f& _rhs)
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

	MATH_INLINE mat4f adj_m4(const mat4f& _rhs)
	{
		mat4f out;
		adj_m4(out, _rhs);
		return out;
	}

	/*	Inverse of Matrix  */
	MATH_INLINE void inv_m4(mat4f& _out, const mat4f& _rhs)
	{ 
		adj_m4(_out, _rhs);
		float	invdet = 1.0f / det_m4(_out);
		scale_m4(_out, mt::vec4f(invdet,invdet,invdet,invdet) );
	}

	MATH_INLINE mat4f inv_m4(const mat4f& _rhs)
	{
		mat4f out;
		inv_m4(out, _rhs);
		return out;
	}

	MATH_INLINE void rotate_axis_m4( mat4f &out, const vec3f &u, float degrees )
	{
		float c = cosf( degrees );
		float s = sinf( degrees );
		float ic = 1.f - c;

		float icux = ic*u.x;
		float icuxy = icux*u.y;
		float icuxz = icux*u.z;
		float icuyz = ic*u.y*u.z;

		float suy = s * u.y;
		float suz = s * u.z;
		float sux = s * u.x;

		mat3f m;

		m[0][0] = c+ic*u.x*u.x; m[0][1] = icuxy - suz; m[0][2] = icuxz + suy;
		m[1][0] = icuxy + suz; m[1][1] = c+ic*u.y*u.y; m[1][2] = icuyz - sux;
		m[2][0] = icuxz - suy; m[2][1] = icuyz + sux; m[2][2] = c+ic*u.z*u.z;

		out = out * m;
	}

	MATH_INLINE void make_projection_m4( mat4f & out , float fovy, float aspect, float zn, float zf )
	{
		float yscale = 1.f / tanf( fovy * 0.5f );
		float xscale = yscale / aspect;

		out[0][0] = xscale;	out[0][1] = 0.f;	out[0][2] = 0.f;					out[0][3] = 0.f;
		out[1][0] = 0.f;	out[1][1] = yscale;	out[1][2] = 0.f;					out[1][3] = 0.f;
		out[2][0] = 0.f;	out[2][1] = 0.f;	out[2][2] = zf / (zf - zn);			out[2][3] = 1.f;
		out[3][0] = 0.f;	out[3][1] = 0.f;	out[3][2] = -zn * zf / (zf - zn);	out[3][3] = 0.f;
	}
}