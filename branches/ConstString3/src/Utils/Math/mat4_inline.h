namespace mt
{
	MATH_METHOD_INLINE mat4f::mat4f()
	{}

	MATH_METHOD_INLINE mat4f::mat4f(const mat4f& _rhs)
		:v0(_rhs.v0)
		,v1(_rhs.v1)
		,v2(_rhs.v2)
		,v3(_rhs.v3)
	{
	}

	MATH_METHOD_INLINE vec4f& mat4f::operator[](int i)
	{
		return (&v0)[i];
	}

	MATH_METHOD_INLINE const vec4f& mat4f::operator[](int i)const
	{
		return (&v0)[i];
	}

	MATH_METHOD_INLINE mat4f& mat4f::operator=(const mat4f& _rhs)
	{
		v0 = _rhs.v0;
		v1 = _rhs.v1;
		v2 = _rhs.v2;
		v3 = _rhs.v3;
		return *this;
	}

	MATH_METHOD_INLINE float * mat4f::buff()
	{
		return v0.buff();
	}

	MATH_METHOD_INLINE const float * mat4f::buff() const
	{
		return v0.buff();
	}

	MATH_FUNCTION_INLINE bool cmp_m4_m4(const mat4f& _a, const mat4f& _b)
	{ 
		return	cmp_v4_v4(_a.v0,_b.v0) &&
				cmp_v4_v4(_a.v1,_b.v1) &&
				cmp_v4_v4(_a.v2,_b.v2) &&
				cmp_v4_v4(_a.v3,_b.v3);
	}

	MATH_FUNCTION_INLINE bool operator==(const mat4f& _a, const mat4f& _b)
	{
		return	cmp_m4_m4(_a, _b);
	}

	MATH_FUNCTION_INLINE bool operator!=(const mat4f& _a, const mat4f& _b)
	{
		return !operator==(_a, _b);
	}

	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE void mul_m4_v3(vec3f& _out, const mat4f& _m,const vec3f& _v)
	{
		_out.x = dot_v4_v3( _m.v0, _v );
		_out.y = dot_v4_v3( _m.v1, _v );
		_out.z = dot_v4_v3( _m.v2, _v );
	}

	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE vec3f operator*(const mat4f& m, const vec3f& v)
	{
		vec3f out;
		mul_m4_v3(out,m,v);
		return out;
	}

	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE void mul_v3_m4(vec3f& _out, const vec3f& _v,const mat4f& _m)
	{
		mul_v3_m4_r( _out, _v, _m );

		_out.x += _m.v3.x;
		_out.y += _m.v3.y;
		_out.z += _m.v3.z;
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE void mul_v3_m4_r(vec3f& _out, const vec3f& _v, const mat4f& _m)
	{
		_out.x = _m.v0.x * _v.x + _m.v1.x * _v.y + _m.v2.x * _v.z;
		_out.y = _m.v0.y * _v.x + _m.v1.y * _v.y + _m.v2.y * _v.z;
		_out.z = _m.v0.z * _v.x + _m.v1.z * _v.y + _m.v2.z * _v.z;
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE float mul_v3_m4_proj(vec3f& _out, const vec3f& _v,const mat4f& _m)
	{
		mul_v3_m4( _out, _v, _m );

		float w = 1.f / _out.z;
		_out *= w;

		return w;
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE void mul_v2_m4(vec2f& _out, const vec2f& _v,const mat4f& _m)
	{
		mul_v2_m4_r( _out, _v, _m );

		_out.x += _m.v3[0];
		_out.y += _m.v3[1];
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE void mul_v2_m4_r(vec2f& _out, const vec2f& _v,const mat4f& _m)
	{
		_out.x = _m.v0.x * _v.x + _m.v1.x * _v.y;
		_out.y = _m.v0.y * _v.x + _m.v1.y * _v.y;
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE vec3f operator*(const vec3f& v, const mat4f& m)
	{
		vec3f out;
		mul_v3_m4(out,v,m);
		return out;
	}
	/*	Matrix/Vector Mult	*/
	MATH_FUNCTION_INLINE void mul_m4_v4(vec4f& _out, const mat4f& _m,const vec4f& _v)
	{
		_out.x = dot_v4_v4( _m.v0, _v );
		_out.y = dot_v4_v4( _m.v1, _v );
		_out.z = dot_v4_v4( _m.v2, _v );
		_out.w = dot_v4_v4( _m.v3, _v );
	}

	MATH_FUNCTION_INLINE vec4f operator*(const mat4f& m, const vec4f& v)
	{
		vec4f out;
		mul_m4_v4(out,m,v);
		return out;
	}

	MATH_FUNCTION_INLINE void mul_v4_m4(vec4f& _out, const vec4f& _a, const mat4f& _b )
	{
		_out.x = _a.x * _b.v0.x + _a.y * _b.v1.x + _a.z * _b.v2.x + _b.v3.x; 
		_out.y = _a.x * _b.v0.y + _a.y * _b.v1.y + _a.z * _b.v2.y + _b.v3.y;
		_out.z = _a.x * _b.v0.z + _a.y * _b.v1.z + _a.z * _b.v2.z + _b.v3.z;
		_out.w = _a.x * _b.v0.w + _a.y * _b.v1.w + _a.z * _b.v2.w + _b.v3.w;
	}

	MATH_FUNCTION_INLINE vec4f operator* (const vec4f& v, const mat4f& m)
	{
		vec4f out;
		mul_v4_m4(out,v,m);
		return out;
	}

	MATH_FUNCTION_INLINE void mul_v4_m4_i( vec4f & _out, const vec4f & _a, const mat4f& _b )
	{	
		_out.x = _a.x * _b.v0.x + _a.y * _b.v1.x + _a.z * _b.v2.x + _a.w * _b.v3.x;
		_out.y = _a.x * _b.v0.y + _a.y * _b.v1.y + _a.z * _b.v2.y + _a.w * _b.v3.y;
		_out.z = _a.x * _b.v0.z + _a.y * _b.v1.z + _a.z * _b.v2.z + _a.w * _b.v3.z;
		_out.w = _a.x * _b.v0.w + _a.y * _b.v1.w + _a.z * _b.v2.w + _a.w * _b.v3.w;
	}
		
	MATH_FUNCTION_INLINE void mul_m4_m4(mat4f& _out, const mat4f& _a, const mat4f& _b)
	{
		mul_v4_m4_i( _out.v0, _a.v0, _b );
		mul_v4_m4_i( _out.v1, _a.v1, _b );
		mul_v4_m4_i( _out.v2, _a.v2, _b );
		mul_v4_m4_i( _out.v3, _a.v3, _b );
	}

	MATH_FUNCTION_INLINE void mul_v4_m3_i(vec4f & _out, const vec4f & _a, const mat3f& _b)
	{
		_out.x = _a.x * _b.v0.x + _a.y * _b.v1.x+ _a.z * _b.v2.x;
		_out.y = _a.x * _b.v0.y + _a.y * _b.v1.y+ _a.z * _b.v2.y;
		_out.z = _a.x * _b.v0.z + _a.y * _b.v1.z+ _a.z * _b.v2.z;
		_out.w = _a.w;
	}
		
	MATH_FUNCTION_INLINE void mul_m4_m3(mat4f& _out, const mat4f& _a, const mat3f& _b)
	{
		mul_v4_m3_i( _out.v0, _a.v0, _b );
		mul_v4_m3_i( _out.v1, _a.v1, _b );
		mul_v4_m3_i( _out.v2, _a.v2, _b );

		_out.v3 = _a.v3;
	}

	MATH_FUNCTION_INLINE mat4f operator* (const mat4f& _a, const mat4f& _b)
	{
		mat4f out;
		mul_m4_m4(out,_a,_b);
		return out;
	}
	
	MATH_FUNCTION_INLINE mat4f operator* (const mat4f& _a, const mat3f& _b)
	{
		mat4f out;
		mul_m4_m3(out,_a,_b);
		return out;
	}

	/*  Addition of matrix  */
	MATH_FUNCTION_INLINE void add_m4_m4(mat4f& _out, const mat4f& _a, const mat4f& _b)
	{
		add_v4_v4(_out.v0,_a.v0,_b.v0);
		add_v4_v4(_out.v1,_a.v1,_b.v1);
		add_v4_v4(_out.v2,_a.v2,_b.v2);
		add_v4_v4(_out.v3,_a.v3,_b.v3);
	}

	MATH_FUNCTION_INLINE mat4f operator+(const mat4f& _a, const mat4f& _b)
	{
		mat4f out;
		add_m4_m4(out,_a,_b);
		return out;
	}

	/*	Substraction of Matrix  */
	MATH_FUNCTION_INLINE void sub_m4_m4(mat4f& _out, const mat4f& _a, const mat4f& _b)
	{
		sub_v4_v4(_out.v0,_a.v0,_b.v0);
		sub_v4_v4(_out.v1,_a.v1,_b.v1);
		sub_v4_v4(_out.v2,_a.v2,_b.v2);
		sub_v4_v4(_out.v3,_a.v3,_b.v3);
	}
	
	MATH_FUNCTION_INLINE mat4f operator-(const mat4f& _a, const mat4f& _b)
	{
		mat4f out;
		sub_m4_m4(out,_a,_b);
		return out;
	}

	/*	Scale by Scalar	 */
	MATH_FUNCTION_INLINE void scale_m4_m4(mat4f& _out, const mat4f& _rhs, const mt::vec4f & _val)
	{
		scale_v4_v4(_out.v0,_rhs.v0,_val.x);
		scale_v4_v4(_out.v1,_rhs.v1,_val.y);
		scale_v4_v4(_out.v2,_rhs.v2,_val.z);
		scale_v4_v4(_out.v3,_rhs.v3,_val.w);
	}

	MATH_FUNCTION_INLINE void scale_m4(mat4f& _out, const mt::vec4f & _val)
	{
		scale_m4_m4( _out, _out, _val );
	}

	MATH_FUNCTION_INLINE void scale_rotate_m4_m4(mat4f& _out, const mat4f& _rhs, const mt::vec3f & _val)
	{
		scale_v4_v4(_out.v0,_rhs.v0,_val.x);
		scale_v4_v4(_out.v1,_rhs.v1,_val.y);
		scale_v4_v4(_out.v2,_rhs.v2,_val.z);
	}

	MATH_FUNCTION_INLINE void scale_rotate_m4(mat4f& _out, const mt::vec3f &_val)
	{
		scale_rotate_m4_m4( _out, _out, _val );
	}

	MATH_FUNCTION_INLINE void zero_m4(mat4f& _out)
	{
		ident_v4( _out.v0 );
		ident_v4( _out.v1 );
		ident_v4( _out.v2 );
		ident_v4( _out.v3 );
	}

	/*	Identity	matrix	*/
	MATH_FUNCTION_INLINE void ident_m4(mat4f& _out)
	{
		zero_m4( _out );

		_out.v0.x = 1.f;
		_out.v1.y = 1.f;
		_out.v2.z = 1.f;
		_out.v3.w = 1.f;
	}

	/*	Get/Set Column	*/
	MATH_FUNCTION_INLINE void get_col_m4(vec4f& out, const mat4f& _rhs, int _index)
    {
        out.x = _rhs.v0[_index];
        out.y = _rhs.v1[_index];
        out.z = _rhs.v2[_index];
		out.w = _rhs.v3[_index];
    }
 
	MATH_FUNCTION_INLINE vec4f get_col_m4(const mat4f& _rhs, int _index)
	{
		vec4f ret;
		get_col_m4(ret,_rhs,_index);
		return ret;
	}

	MATH_FUNCTION_INLINE void set_col_m4(mat4f& out, const vec4f& _rhs, int _index)
	{
		out.v0[_index] = _rhs.x;
		out.v1[_index] = _rhs.y;	
		out.v2[_index] = _rhs.z;
		out.v3[_index] = _rhs.w;
	}

	/*	Transpose  */
	MATH_FUNCTION_INLINE void transpose_m4(mat4f& _out, const mat4f& _rhs)
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

	MATH_FUNCTION_INLINE mat4f transpose_m4(const mat4f& _rhs)
	{
		mat4f out;
		transpose_m4(out, _rhs);
		return out;
	}

	MATH_FUNCTION_INLINE void extract_m4_m3(mat3f& _out, const mat4f& _rhs)
	{
		extract_v4_v3( _out.v0, _rhs.v0 );
		extract_v4_v3( _out.v1, _rhs.v1 );
		extract_v4_v3( _out.v2, _rhs.v2 );
	}

	MATH_FUNCTION_INLINE float minor_m4(const mat4f& _rhs, const int r0, const int r1, const int r2, const int c0, const int c1, const int c2)
	{
		return	_rhs[r0][c0] * (_rhs[r1][c1] * _rhs[r2][c2] - _rhs[r2][c1] * _rhs[r1][c2]) -
				_rhs[r0][c1] * (_rhs[r1][c0] * _rhs[r2][c2] - _rhs[r2][c0] * _rhs[r1][c2]) +
				_rhs[r0][c2] * (_rhs[r1][c0] * _rhs[r2][c1] - _rhs[r2][c0] * _rhs[r1][c1]);			
	}

	MATH_FUNCTION_INLINE float det_m4(const mat4f& _rhs)
	{
		return	_rhs[0][0] * minor_m4(_rhs, 1, 2, 3, 1, 2, 3) -
				_rhs[0][1] * minor_m4(_rhs, 1, 2, 3, 0, 2, 3) +
				_rhs[0][2] * minor_m4(_rhs, 1, 2, 3, 0, 1, 3) -
				_rhs[0][3] * minor_m4(_rhs, 1, 2, 3, 0, 1, 2);
	}

	MATH_FUNCTION_INLINE void adj_m4(mat4f& _out, const mat4f& _rhs)
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

	MATH_FUNCTION_INLINE mat4f adj_m4(const mat4f& _rhs)
	{
		mat4f out;
		adj_m4(out, _rhs);
		return out;
	}

	/*	Inverse of Matrix  */
	MATH_FUNCTION_INLINE void inv_m4(mat4f& _out, const mat4f& _rhs)
	{ 
		adj_m4(_out, _rhs);
		float	invdet = 1.0f / det_m4(_rhs);
		scale_m4(_out, mt::vec4f(invdet,invdet,invdet,invdet) );
	}

	MATH_FUNCTION_INLINE mat4f inv_m4(const mat4f& _rhs)
	{
		mat4f out;
		inv_m4(out, _rhs);
		return out;
	}

	MATH_FUNCTION_INLINE void rotate_axis_m4( mat4f &out, const vec3f &u, float degrees )
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

	MATH_FUNCTION_INLINE void make_projection_ortho_lh_m4( mat4f & _out, float _left, float _right, float _top, float _bottom, float _near, float _far )
	{
		// 2/(r-l)      0            0           0
		_out.v0.x = 2.f / (_right - _left);
		_out.v0.y = 0.f;
		_out.v0.z = 0.f;
		_out.v0.w = 0.f;

		//0            2/(t-b)      0           0
		_out.v1.x = 0.f;
		_out.v1.y = 2.f / (_top - _bottom);
		_out.v1.z = 0.f;
		_out.v1.w = 0.f;

		//0            0            1/(zf-zn)   0
		_out.v2.x = 0.f;
		_out.v2.y = 0.f;
		_out.v2.z = 1.f / (_far - _near);
		_out.v2.w = 0.f;

		//(l+r)/(l-r)  (t+b)/(b-t)  zn/(zn-zf)  1
		_out.v3.x = (_left + _right) / (_left - _right);
		_out.v3.y = (_top + _bottom) / (_bottom - _top);
		_out.v3.z = _near / (_near - _far);
		_out.v3.w = 1.f;
	}

	MATH_FUNCTION_INLINE void make_projection_frustum_m4( mat4f & _out, float _left, float _right, float _top, float _bottom, float _near, float _far )
	{
		//2*zn/(r-l)   0            0              0
		//0            2*zn/(t-b)   0              0
		//(l+r)/(l-r)  (t+b)/(b-t)  zf/(zf-zn)     1
		//0            0            zn*zf/(zn-zf)  0

		_out.v0.x = 2.f * _near / (_right - _left);
		_out.v0.y = 0.f;
		_out.v0.z = 0.f;
		_out.v0.w = 0.f;

		_out.v1.x = 0.f;
		_out.v1.y = 2.f * _near / (_top - _bottom);
		_out.v1.z = 0.f;
		_out.v1.w = 0.f;

		_out.v2.x = (_left + _right) / (_left - _right);
		_out.v2.y = (_top + _bottom) / (_bottom - _top);
		//_out.v2.x = 0.f;
		//_out.v2.y = 0.f;
		_out.v2.z = (_far) / (_far - _near);
		_out.v2.w = 1.f;

		_out.v3.x = 0.f;
		_out.v3.y = 0.f;
		_out.v3.z = (_near * _far) / (_near - _far);
		_out.v3.w = 0.f;
	}

	MATH_FUNCTION_INLINE void make_projection_fov_m4( mat4f & _out, float fovy, float aspect, float zn, float zf )
	{
		float yscale = 1.f / tanf( fovy * 0.5f );
		float xscale = yscale / aspect;

		_out.v0.x = xscale;	
		_out.v0.y = 0.f;	
		_out.v0.z = 0.f;
		_out.v0.w = 0.f;

		_out.v1.x = 0.f;
		_out.v1.y = yscale;
		_out.v1.z = 0.f;					
		_out.v1.w = 0.f;

		_out.v2.x = 0.f;
		_out.v2.y = 0.f;	
		_out.v2.z = zf / (zf - zn);
		_out.v2.w = 1.f;

		_out.v3.x = 0.f;	
		_out.v3.y = 0.f;	
		_out.v3.z = -zn * zf / (zf - zn);
		_out.v3.w = 0.f;
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE void make_projection_fov2_m4( mat4f &_out , float _fov, float _aspect, float _zn, float _zf )
	{
		float yscale = 1.f / tanf( _fov * 0.5f );
		float xscale = yscale / _aspect;

		_out.v0.x = xscale;	
		_out.v0.y = 0.f;	
		_out.v0.z = 0.f;
		_out.v0.w = 0.f;

		_out.v1.x = 0.f;
		_out.v1.y = yscale;
		_out.v1.z = 0.f;					
		_out.v1.w = 0.f;

		_out.v2.x = 0.f;
		_out.v2.y = 0.f;	
		_out.v2.z = - (_zf + _zn) / (_zf - _zn);
		_out.v2.w = -1.f;

		_out.v3.x = 0.f;	
		_out.v3.y = 0.f;	
		_out.v3.z = - 2.f * _zn * _zf / (_zf - _zn);
		_out.v3.w = 0.f;
	}
	//////////////////////////////////////////////////////////////////////////
	MATH_FUNCTION_INLINE void make_perspective_projection_m4( mat4f & _out, float _fov, float _aspect, float zn, float zf )
	{
		float thetaY = _fov * 0.5f;
		float tanThetaY = ::tanf( thetaY );
		float tanThetaX = tanThetaY * _aspect;

		float half_w = tanThetaX * zn;
		float half_h = tanThetaY * zn;

		float left   = - half_w;
		float right  = + half_w;
		float bottom = - half_h;
		float top    = + half_h;

		// The code below will dealing with general projection 
		// parameters, similar glFrustum and glOrtho.
		// Doesn't optimise manually except division operator, so the 
		// code more self-explaining.

		float inv_w = 1 / (right - left);
		float inv_h = 1 / (top - bottom);
		float inv_d = 1 / (zf - zn);

		// Recalc if frustum params changed

		// Calc matrix elements
		float A = 2 * zn * inv_w;
		float B = 2 * zn * inv_h;
		float C = (right + left) * inv_w;
		float D = (top + bottom) * inv_h;
		float q, qn;

		q = - (zf + zn) * inv_d;
		qn = -2 * (zf * zn) * inv_d;

		// NB: This creates 'uniform' perspective projection matrix,
		// which depth range [-1,1], left-handed rules
		//
		// [ A   0   0   0  ]
		// [ 0   B   0   0  ]
		// [ C   D   q   -1 ]
		// [ 0   0   qn  0  ]
		//
		// A = 2 * near / (right - left)
		// B = 2 * near / (top - bottom)
		// C = (right + left) / (right - left)
		// D = (top + bottom) / (top - bottom)
		// q = - (far + near) / (far - near)
		// qn = - 2 * (far * near) / (far - near)
		mt::zero_m4(_out);

		_out[0][0] = A;
		_out[2][0] = C;
		_out[1][1] = B;
		_out[2][1] = D;
		_out[2][2] = q;
		_out[3][2] = qn;
		_out[2][3] = -1;

		// Convert depth range from [-1,+1] to [0,1]
		_out[0][2] = (_out[0][2] + _out[0][3]) * 0.5f;
		_out[1][2] = (_out[1][2] + _out[1][3]) * 0.5f;
		_out[2][2] = (_out[2][2] + _out[2][3]) * 0.5f;
		_out[3][2] = (_out[3][2] + _out[3][3]) * 0.5f;

		_out[2][0] = -_out[2][0];
		_out[2][1] = -_out[2][1];
		_out[2][2] = -_out[2][2];
		_out[2][3] = -_out[2][3];
	}

	MATH_FUNCTION_INLINE void make_rotate_m4(mat4f & _out, float _x, float _y, float _z)
	{
		float ca = cosf(_x);
		float cb = cosf(_y);
		float cy = cosf(_z);

		float sa = sinf(_x);
		float sb = sinf(_y);
		float sy = sinf(_z);

		_out.v0.x = ca * cb;
		_out.v0.y = ca * sb * sy - sa * cy;
		_out.v0.z = ca * sb * cy + sa * sy;
		_out.v0.w = 0.f;

		_out.v1.x = sa * cb;
		_out.v1.y = sa * sb * sy + ca * cy;
		_out.v1.z = sa * sb * cy - ca * sy;
		_out.v1.w = 0.f;

		_out.v2.x = -sb;
		_out.v2.y = cb * sy;
		_out.v2.z = cb * cy;
		_out.v2.w = 0.f;

		_out.v3.x = 0.f;
		_out.v3.y = 0.f;
		_out.v3.z = 0.f;
		_out.v3.w = 1.f;
	}


	MATH_FUNCTION_INLINE void make_rotate_x_axis_m4(mat4f & _out, float _angle)
	{
		float cosa = cosf(_angle);
		float sina = sinf(_angle);

		_out.v0.x = 1.f;
		_out.v0.y = 0.f;
		_out.v0.z = 0.f;
		_out.v0.w = 0.f;

		_out.v1.x = 0.f;
		_out.v1.y = cosa;
		_out.v1.z = -sina;
		_out.v1.w = 0.f;

		_out.v2.x = 0.f;
		_out.v2.y = sina;
		_out.v2.z = cosa;
		_out.v2.w = 0.f;

		_out.v3.x = 0.f;
		_out.v3.y = 0.f;
		_out.v3.z = 0.f;
		_out.v3.w = 1.f;
	}

	MATH_FUNCTION_INLINE void make_rotate_y_axis_m4(mat4f & _out, float _angle)
	{
		float cosa = cosf(_angle);
		float sina = sinf(_angle);

		_out.v0.x = cosa;
		_out.v0.y = 0.f;
		_out.v0.z = sina;
		_out.v0.w = 0.f;

		_out.v1.x = 0.f;
		_out.v1.y = 1.f;
		_out.v1.z = 0.f;
		_out.v1.w = 0.f;

		_out.v2.x = -sina;
		_out.v2.y = 0.f;
		_out.v2.z = cosa;
		_out.v2.w = 0.f;

		_out.v3.x = 0.f;
		_out.v3.y = 0.f;
		_out.v3.z = 0.f;
		_out.v3.w = 1.f;
	}

	MATH_FUNCTION_INLINE void make_rotate_z_axis_m4(mat4f & _out, float _angle)
	{
		float cosa = cosf(_angle);
		float sina = sinf(_angle);

		_out.v0.x = cosa;
		_out.v0.y = -sina;
		_out.v0.z = 0.f;
		_out.v0.w = 0.f;

		_out.v1.x = sina;
		_out.v1.y = cosa;
		_out.v1.z = 0.f;
		_out.v1.w = 0.f;

		_out.v2.x = 0.f;
		_out.v2.y = 0.f;
		_out.v2.z = 1.f;
		_out.v2.w = 0.f;

		_out.v3.x = 0.f;
		_out.v3.y = 0.f;
		_out.v3.z = 0.f;
		_out.v3.w = 1.f;
	}

	MATH_FUNCTION_INLINE void make_scale_m4( mat4f & _out, float _x, float _y, float _z )
	{
		_out.v0.x = _x;
		_out.v0.y = 0.f;
		_out.v0.z = 0.f;
		_out.v0.w = 0.f;

		_out.v1.x = 0.f;
		_out.v1.y = _y;
		_out.v1.z = 0.f;
		_out.v1.w = 0.f;

		_out.v2.x = 0.f;
		_out.v2.y = 0.f;
		_out.v2.z = _z;
		_out.v2.w = 0.f;

		_out.v3.x = 0.f;
		_out.v3.y = 0.f;
		_out.v3.z = 0.f;
		_out.v3.w = 1.f;
	}

	MATH_FUNCTION_INLINE void make_translation_m4( mat4f & _out, float _x, float _y, float _z )
	{
		_out.v0.x = 1.f;
		_out.v0.y = 0.f;
		_out.v0.z = 0.f;
		_out.v0.w = 0.f;

		_out.v1.x = 0.f;
		_out.v1.y = 1.f;
		_out.v1.z = 0.f;
		_out.v1.w = 0.f;

		_out.v2.x = 0.f;
		_out.v2.y = 0.f;
		_out.v2.z = 1.f;
		_out.v2.w = 0.f;

		_out.v3.x = _x;
		_out.v3.y = _y;
		_out.v3.z = _z;
		_out.v3.w = 1.f;
	}

	MATH_FUNCTION_INLINE void make_lookat_m4( mat4f & _out, const vec3f & _eye, const vec3f & _at, const vec3f & _up )
	{
		//zaxis = normal(At - Eye)
		vec3f look;
		sub_v3_v3( look, _at, _eye );
	
		vec3f zaxis;
		norm_v3( zaxis, look );

		//xaxis = normal(cross(Up, zaxis))
		vec3f xaxis;
		cross_v3_v3_norm( xaxis, _up, zaxis );

		//yaxis = cross(zaxis, xaxis)
		vec3f yaxis;
		cross_v3_v3( yaxis, zaxis, xaxis );

		//xaxis.x           yaxis.x           zaxis.x          0
		//xaxis.y           yaxis.y           zaxis.y          0
		//xaxis.z           yaxis.z           zaxis.z          0
		//-dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  l
		_out.v0.x = xaxis.x;
		_out.v0.y = yaxis.x;
		_out.v0.z = zaxis.x;
		_out.v0.w = 0.f;

		_out.v1.x = xaxis.y;
		_out.v1.y = yaxis.y;
		_out.v1.z = zaxis.y;
		_out.v1.w = 0.f;

		_out.v2.x = xaxis.z;
		_out.v2.y = yaxis.z;
		_out.v2.z = zaxis.z;
		_out.v2.w = 0.f;

		_out.v3.x = - dot_v3_v3( xaxis, _eye );
		_out.v3.y = - dot_v3_v3( yaxis, _eye );
		_out.v3.z = - dot_v3_v3( zaxis, _eye );
		_out.v3.w = 1.f;
	}

	MATH_FUNCTION_INLINE void project_m4( vec3f & _out, const vec3f & _vertex, float _width, float _height, const mat4f & _projection, const mat4f & _view, const mat4f & _world )
	{
		mat4f m1;
		mat4f m2; 

		vec3f vec;
		vec3f vec_i;

		mul_m4_m4( m1, _world, _view );
		mul_m4_m4( m2, m1, _projection ); 
		mul_v3_m4_proj( vec, _vertex, m2 );

		_out.x = ( 1.0f + vec.x ) * _width * 0.5f; 
		_out.y = ( 1.0f + vec.y ) * _height * 0.5f;
		_out.z = vec.z; 
	}
}
