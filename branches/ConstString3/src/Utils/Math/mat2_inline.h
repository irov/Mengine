namespace mt
{ 
	MATH_INLINE mat2f::mat2f()
	{}

	MATH_INLINE mat2f::mat2f( float angle )
	{
		float c = cosf(angle), s = sinf(angle);
		v0.x = c; v1.x = -s;
		v0.y = s; v1.y = c;
	}

	MATH_INLINE mat2f::mat2f(const mat2f&	_rhs)
		:v0(_rhs.v0)
		,v1(_rhs.v1)
	{}

	MATH_INLINE mat2f::mat2f(const vec2f&	_v0, const vec2f&	_v1)
		:v0(_v0)
		,v1(_v1)
	{}

	MATH_INLINE vec2f& mat2f::operator [](int i)
	{
		return (&v0)[i];
	}

	MATH_INLINE const vec2f& mat2f::operator [](int i)const
	{
		return (&v0)[i];
	}

	MATH_INLINE mat2f& mat2f::operator=(const mat2f& _rhs)
	{
		v0 = _rhs.v0;
		v1 = _rhs.v1;
		return *this;
	}	

	MATH_INLINE bool cmp_m2_m2(const mat2f& _a, const mat2f& _b)
	{ 
		return cmp_v2_v2(_a.v0,_b.v0) &&
			cmp_v2_v2(_a.v1,_b.v1);
	}

	MATH_INLINE bool operator==(const mat2f& _a, const mat2f& _b)
	{
		return cmp_m2_m2(_a, _b);
	}

	MATH_INLINE bool operator!=(const mat2f& _a, const mat2f& _b)
	{
		return !operator==(_a, _b);
	}

	/*	Matrix/Vector Mult	*/
	MATH_INLINE void mul_m2_v2(vec2f& _out, const mat2f& _m, const vec2f& _v)
	{ 
		_out.x = dot_v2_v2(_m.v0,_v);
		_out.y = dot_v2_v2(_m.v1,_v);
	}

	MATH_INLINE vec2f operator*(const mat2f& m, const vec2f& v)
	{
		vec2f out;
		mul_m2_v2(out,m,v);
		return out;
	}

	/*	Vector/Matrix Mult	*/
	MATH_INLINE void mul_v2_m2(vec2f& _out, const vec2f& _v, const mat2f& _m)
	{
		_out.x = _m.v0.x * _v.x + _m.v1.x * _v.y;
		_out.y = _m.v0.y * _v.x + _m.v1.y * _v.y;
	}

	MATH_INLINE vec2f operator* (const vec2f& _v, const mat2f& _m)
	{
		vec2f out;
		mul_v2_m2(out,_v,_m);
		return out;
	}

	/*	Matrix/Matrix Mult  */
	MATH_INLINE void mul_m2_m2(mat2f& _out, const mat2f& _a, const mat2f& _b)
	{
		_out.v0.x = _a.v0.x * _b.v0.x + _a.v0.y * _b.v1.x; 
		_out.v0.y = _a.v0.x * _b.v0.y + _a.v0.y * _b.v1.y;
		_out.v1.x = _a.v1.x * _b.v0.x + _a.v1.y * _b.v1.x;
		_out.v1.y = _a.v1.x * _b.v0.y + _a.v1.y * _b.v1.y;
	}

	MATH_INLINE mat2f operator* (const mat2f& _a, const mat2f& _b)
	{
		mat2f out;
		mul_m2_m2(out,_a,_b);
		return out;
	}

	/*  Addition of matrix  */
	MATH_INLINE void add_m2_m2(mat2f& _out, const mat2f& _a,const mat2f& _b)
	{
		add_v2_v2(_out.v0,_a.v0,_b.v0);
		add_v2_v2(_out.v1,_a.v1,_b.v1);
	}

	MATH_INLINE mat2f operator+(const mat2f& _a, const mat2f& _b)
	{
		mat2f out;
		add_m2_m2(out,_a,_b);
		return out;
	}

	/*	Substraction of Matrix  */
	MATH_INLINE void sub_m2_m2(mat2f& _out, const mat2f& _a, const mat2f& _b)
	{
		sub_v2_v2(_out.v0,_a.v0,_b.v0);
		sub_v2_v2(_out.v1,_a.v1,_b.v1);
	}

	MATH_INLINE mat2f operator-(const mat2f& _a, const mat2f& _b)
	{
		mat2f out;
		sub_m2_m2(out,_a,_b);
		return out;
	}

	/*	Scale by Scalar	 */
	MATH_INLINE void scale_m2_s(mat2f& _out, const mat2f& _rhs, float _val)
	{
		mul_v2_f(_out.v0,_rhs.v0,_val);
		mul_v2_f(_out.v1,_rhs.v1,_val);
	}

	MATH_INLINE mat2f operator*(const mat2f& _rhs, float _val)
	{
		mat2f out;
		scale_m2_s(out,_rhs,_val);
		return out;
	}

	MATH_INLINE void scale_m2_s(mat2f& _out, float _val)
	{
		mul_v2_f(_out.v0,_out.v0,_val);
		mul_v2_f(_out.v1,_out.v1,_val);
	}

	/*	Transpose Matrix  */
	MATH_INLINE void transpose_m2(mat2f& _out, const mat2f& _rhs)
	{
		_out.v0.x = _rhs.v0.x;
		_out.v0.y = _rhs.v1.x;

		_out.v1.x = _rhs.v0.y;
		_out.v1.y = _rhs.v1.y;
	}

	MATH_INLINE mat2f transpose_m2(const mat2f& _rhs)
	{
		mat2f out;
		transpose_m2(out, _rhs);
		return out;
	}
}
