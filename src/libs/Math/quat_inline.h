#	include <assert.h>

#	include <math.h>

#	include "mat4.h"

namespace mt
{
	MATH_INLINE quatf::quatf()
	{}

	MATH_INLINE quatf::quatf(float _w, float _x, float _y, float _z)
		:w(_w)
		,x(_x)
		,y(_y)
		,z(_z)
	{}

	MATH_INLINE quatf::quatf(float _angle, const mt::vec3f & _v)
	{
		*this = q_from_angle_axis(_v,_angle);
	}

	MATH_INLINE quatf::quatf(const quatf& _q)
		:w(_q.w)
		,x(_q.x)
		,y(_q.y)
		,z(_q.z)
	{}

	MATH_INLINE float& quatf::operator[](int i)
	{
		assert(i < 4);
		return (&x)[i];
	}

	MATH_INLINE const float quatf::operator[](int i)const
	{
		assert(i < 4);
		return (&x)[i];
	}

	MATH_INLINE quatf& quatf::operator=(const quatf& q)
	{
		x = q.x;
		y = q.y;
		z = q.z;
		w = q.w;
		return *this;
	}

	MATH_INLINE float quatf::norm() const
	{
		return x*x+y*y+z*z+w*w;
	}

	MATH_INLINE void quatf::multiply(const quatf& left, const vec3f& right)		
	{
		float a,b,c,d;

		a = - left.x*right.x - left.y*right.y - left.z *right.z;
		b =   left.w*right.x + left.y*right.z - right.y*left.z;
		c =   left.w*right.y + left.z*right.x - right.z*left.x;
		d =   left.w*right.z + left.x*right.y - right.x*left.y;

		w = a;
		x = b;
		y = c;
		z = d;
	}

	MATH_INLINE void quatf::rotate(vec3f & v) const						
	{
		quatf myInverse;
		myInverse.x = -x;//*msq;
		myInverse.y = -y;//*msq;
		myInverse.z = -z;//*msq;
		myInverse.w =  w;//*msq;

		quatf left;

		left.multiply(*this,v);

		v.x =left.w*myInverse.x + myInverse.w*left.x + left.y*myInverse.z - myInverse.y*left.z;
		v.y =left.w*myInverse.y + myInverse.w*left.y + left.z*myInverse.x - myInverse.z*left.x;
		v.z =left.w*myInverse.z + myInverse.w*left.z + left.x*myInverse.y - myInverse.x*left.y;
	}

	MATH_INLINE float quatf::getPitch(bool _reprojectAxis) const
	{
		if (_reprojectAxis)
		{
			// pitch = atan2(localy.z, localy.y)
			// pick parts of yAxis() implementation that we need
			float fTx  = 2.f*x;
			float fTy  = 2.f*y;
			float fTz  = 2.f*z;
			float fTwx = fTx*w;
			float fTxx = fTx*x;
			float fTyz = fTz*y;
			float fTzz = fTz*z;

			// Vector3(fTxy-fTwz, 1.0-(fTxx+fTzz), fTyz+fTwx);
			return (atan2f(fTyz+fTwx, 1.0f-(fTxx+fTzz))) * 180.f/3.14f;
		}
		else
		{
			// internal version
			return (atan2f(2*(y*z + w*x), w*w - x*x - y*y + z*z));
		}
	}

	MATH_INLINE float quatf::getYaw(bool _reprojectAxis) const
	{
		if (_reprojectAxis)
		{
			float fTx  = 2.f*x;
			float fTy  = 2.f*y;
			float fTz  = 2.f*z;
			float fTwy = fTy*w;
			float fTxx = fTx*x;
			float fTxz = fTz*x;
			float fTyy = fTy*y;

			return (atan2f(fTxz+fTwy, 1.0f-(fTxx+fTyy)));

		}
		else
		{
			return (asinf(-2*(x*z - w*y)));
		}
	}

	MATH_INLINE bool cmp_q_q(const quatf& _a, const quatf& _b, float eps)
	{
		return	(fabsf(_a.w - _b.w) < eps) && (fabsf(_a.x - _b.x) < eps) && (fabsf(_a.y - _b.y) < eps) && (fabsf(_a.z - _b.z) < eps);
	}

	MATH_INLINE bool operator==(const quatf& _a, const quatf& _b) 
	{
		return cmp_q_q(_a, _b);
	}

	MATH_INLINE bool operator!=(const quatf& _a, const quatf& _b) 
	{
		return !operator==(_a, _b);
	}

	/* Addition */
	MATH_INLINE void add_q_q(quatf&	_out, const quatf& _a,const quatf& _b)
	{
		_out[0] = _a[0] + _b[0];
		_out[1] = _a[1] + _b[1];
		_out[2] = _a[2] + _b[2];
		_out[3] = _a[3] + _b[3];
	}

	MATH_INLINE quatf operator+(const quatf& _a, const quatf& _b)
	{
		quatf out;
		add_q_q(out,_a,_b);
		return out;
	}

	/*  Substr  */
	MATH_INLINE void sub_q_q(quatf&	_out, const quatf& _a,const quatf& _b)
	{
		_out[0] = _a[0] - _b[0];
		_out[1] = _a[1] - _b[1];
		_out[2] = _a[2] - _b[2];
		_out[3] = _a[3] - _b[3];
	}

	MATH_INLINE quatf operator-(const quatf& _a, const quatf& _b)
	{
		quatf out;
		sub_q_q(out,_a,_b);
		return out;
	}

	/*	Mult  Q &  Q */
	MATH_INLINE void mul_q_q(quatf& out, const quatf& _rhs, const quatf& in2)
	{
		out[0]=_rhs[0] * in2[0] - _rhs[1] * in2[1] - _rhs[2] * in2[2] - _rhs[3]*in2[3];
		out[1]=_rhs[0] * in2[1] + _rhs[1] * in2[0] + _rhs[2] * in2[3] - _rhs[3]*in2[2];
		out[2]=_rhs[0] * in2[2] + _rhs[2] * in2[0] + _rhs[3] * in2[1] - _rhs[1]*in2[3];
		out[3]=_rhs[0] * in2[3] + _rhs[3] * in2[0] + _rhs[1] * in2[2] - _rhs[2]*in2[1];
	}

	MATH_INLINE quatf operator*(const quatf& _a, const quatf& _b)
	{
		quatf out;
		mul_q_q(out,_a,_b);
		return out;
	}

	/*	Mult  Q &  V */
	MATH_INLINE void mul_q_v3(vec3f &out,const quatf& _rhs,const vec3f& _v)
	{
		vec3f uv; 
		vec3f uuv;
		cross_v3_v3(uv,mt::vec3f( _rhs.x, _rhs.y, _rhs.z ),_v);
		cross_v3_v3(uuv,mt::vec3f( _rhs.x, _rhs.y, _rhs.z ),uv); 
		uv*=2.0f*_rhs.w;
		uuv*=2.0f;
		add_v3_v3(uv,uv,uuv);
		add_v3_v3(out,_v,uv);
	};

	MATH_INLINE vec3f operator*(const quatf& _rhs, const vec3f& _v)
	{
		vec3f out;
		mul_q_v3(out,_rhs,_v);
		return out;
	}

	MATH_INLINE void scale_q_s(quatf &out, const quatf& _rhs, float _val)
	{
		out.w = _rhs.w * _val;
		out.x = _rhs.x * _val;
		out.y = _rhs.y * _val;
		out.z = _rhs.z * _val;
	}

	MATH_INLINE quatf operator*(const quatf& _rhs, float _val)
	{
		quatf out;
		scale_q_s(out,_rhs,_val);
		return out;
	}

	MATH_INLINE quatf operator/(const quatf& _rhs, float _val)
	{
		return operator*(_rhs, 1/_val);;
	}

	/*	Dot	Product			*/
	MATH_INLINE float dot_q_q(const quatf& _a, const quatf& _b)
	{
		return  _a.w * _b.w + _a.x * _b.x + _a.y * _b.y + _a.z * _b.z;
	}

	/*	Normalize Vector*/
	MATH_INLINE void norm_q(quatf& _out, const quatf& _rhs)
	{
		_out = _rhs / _rhs.norm();
	}

	MATH_INLINE quatf norm_q(const quatf& _rhs)
	{
		quatf	out;
		norm_q(out, _rhs);
		return	out;
	}

	MATH_INLINE void norm_safe_q(quatf& out, const quatf& _rhs, float err)
	{
		float len = _rhs.norm();
		if (len > err)
        {
			out = _rhs / len;
		}
		else
		{
			out = _rhs;
		}
	}

	MATH_INLINE quatf norm_safe_q(const quatf& _rhs)
	{
		quatf out;
		norm_safe_q(out, _rhs);
		return out;
	}

	/* ohter stuff */
	MATH_INLINE void q_from_angle_axis(quatf& out, const vec3f& _rhs, float _val)
	{
		float hangle = 0.01745329251994329547f * _val * 0.5f;
		float fsin = sinf(hangle);

		float i_length =  1.0f / sqrtf( _rhs.x*_rhs.x + _rhs.y*_rhs.y + _rhs.z*_rhs.z );
	
		out.w = cosf(hangle);
		out.x = fsin * _rhs[0] * i_length;
		out.y = fsin * _rhs[1] * i_length;
		out.z = fsin * _rhs[2] * i_length;
	};

	MATH_INLINE quatf q_from_angle_axis(const vec3f& _rhs, float _val)
	{
		quatf out;
		q_from_angle_axis(out,_rhs,_val);
		return out;
	};

	MATH_INLINE void inverse_q(quatf& _out, const quatf& _rhs)
	{
		float norm = _rhs.norm();

		if (norm > 0.0f)
		{
			float invnorm = 1.0f/norm;
			_out.w=_rhs.w*invnorm;
			_out.x=-_rhs.x*invnorm;
			_out.y=-_rhs.y*invnorm;
			_out.z=-_rhs.z*invnorm;
		}
		else
		{
			_out.w = _out.x = _out.y = _out.z = 0.0f;
		}
	}

	MATH_INLINE quatf inverse_q(const quatf& _rhs)
	{
		quatf out;
		inverse_q(out,_rhs);
		return	out;
	};

	MATH_INLINE void exp_q(quatf& _out, const quatf& _rhs)
	{
		/* 
			If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
			exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
			use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.
		*/

		float angle = sqrtf(_rhs.x*_rhs.x+_rhs.y*_rhs.y+_rhs.z*_rhs.z);
		float fsin = sinf(angle);

		_out.w = cosf(angle);

		if (fabsf(fsin) >= 0.00001f)
		{
			float coef = fsin/angle;
			_out.x = coef * _rhs.x;
			_out.y = coef * _rhs.y;
			_out.z = coef * _rhs.z;
		}
		else
		{
			_out.x = _rhs.x;
			_out.y = _rhs.y;
			_out.z = _rhs.z;
		}
	};

	MATH_INLINE quatf exp_q(const quatf& _rhs)
	{
		quatf out;
		exp_q(out,_rhs);
		return out;
	};

	MATH_INLINE void log_q(quatf& _out, const quatf& _rhs)
	{
		/*
			If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
			log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
			sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.
		*/

		_out.w = 0.0f;

		if (fabsf(_rhs[0]) < 1.0f)
		{
			float angle = cosf(_rhs.w);
			float fsin = sinf(angle);
			if (fabsf(fsin) >= 0.00001f)
			{
				float coef = angle/fsin;
				_out.x = coef * _rhs.x;
				_out.y = coef * _rhs.y;
				_out.z = coef * _rhs.z;
			}
		}

		_out.x = _rhs.x;
		_out.y = _rhs.y;
		_out.z = _rhs.z;
	};

	MATH_INLINE quatf log_q(const quatf& _rhs)
	{
		quatf	out;
		log_q(out,_rhs);
		return	out;
	};

	MATH_INLINE void q_from_rot_m3(quatf& out, const mat3f& _rhs)
	{
		float ftrace = _rhs.v0.x+_rhs.v1.y+_rhs.v2.z;
		float froot;

		if (ftrace > 0.0f)
		{
			froot = sqrtf(ftrace + 1.0f );  
			out.w = 0.5f*froot;
			froot = 0.5f/froot;  
			out.x = (_rhs.v2.y-_rhs.v1.z)*froot;
			out.y = (_rhs.v0.z-_rhs.v2.x)*froot;
			out.z = (_rhs.v1.x-_rhs.v0.y)*froot;
		}
		else
		{
			int s_iNext[3] = { 1, 2, 0 };
			int i = 0;
			if ( _rhs.v1.y > _rhs.v0.x )
			{
				i = 1;
			}

			if ( _rhs.v2.z > _rhs[i][i] )
			{
				i = 2;
			}

			int j = s_iNext[i];
			int k = s_iNext[j];

			froot = sqrtf(_rhs[i][i]-_rhs[j][j]-_rhs[k][k] + 1.0f);
			out[i] =	0.5f*froot;
			froot =		0.5f/froot;
			out.w	   = (_rhs[k][j]-_rhs[j][k])*froot;
			out[j]   = (_rhs[j][i]+_rhs[i][j])*froot;
			out[k]   = (_rhs[k][i]+_rhs[i][k])*froot;
		}
	};

	MATH_INLINE quatf q_from_rot_m3(const mat3f& _rhs)
	{
		quatf out;
		q_from_rot_m3(out,_rhs);
		return out;
	};

	MATH_INLINE void q_from_rot_m4(quatf& out, const mat4f& _rhs)
	{
		float ftrace = _rhs.v0.x +_rhs.v1.y+_rhs.v2.z;
		float froot;

		if (ftrace > 0.0f)
		{
			froot = sqrtf(ftrace + 1.0f );  
			out.w = 0.5f*froot;
			froot = 0.5f/froot;  
			out.x = (_rhs[2][1]-_rhs[1][2])*froot;
			out.y = (_rhs[0][2]-_rhs[2][0])*froot;
			out.z = (_rhs[1][0]-_rhs[0][1])*froot;
		}
		else
		{
			int s_iNext[3] = { 1, 2, 0 };
			int i = 0;
			if ( _rhs.v1.y > _rhs.v0.x )
			{
				i = 1;
			}

			if ( _rhs.v2.z > _rhs[i][i] )
			{
				i = 2;
			}

			int j = s_iNext[i];
			int k = s_iNext[j];

			froot = sqrtf(_rhs[i][i]-_rhs[j][j]-_rhs[k][k] + 1.0f);
			out[i] =	0.5f*froot;
			froot =		0.5f/froot;
			out.w	   = (_rhs[k][j]-_rhs[j][k])*froot;
			out[j]   = (_rhs[j][i]+_rhs[i][j])*froot;
			out[k]   = (_rhs[k][i]+_rhs[i][k])*froot;
		}
	};

	MATH_INLINE quatf q_from_rot_m4(const mat4f& _rhs)
	{
		quatf out;
		q_from_rot_m4(out,_rhs);
		return out;
	};

	MATH_INLINE void q_to_rot_m3(mat3f& out, const quatf& _rhs)
	{
		float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
		x2 = _rhs.x + _rhs.x;
		y2 = _rhs.y + _rhs.y;
		z2 = _rhs.z + _rhs.z;
		xx = _rhs.x * x2;   xy = _rhs.x * y2;   xz = _rhs.x * z2;
		yy = _rhs.y * y2;   yz = _rhs.y * z2;   zz = _rhs.z * z2;
		wx = _rhs.w * x2;   wy = _rhs.w * y2;   wz = _rhs.w * z2;

		out[0][0]=1.0f-(yy+zz);		out[0][1]=xy-wz;			out[0][2]=xz+wy;
		out[1][0]=xy+wz;			out[1][1]=1.0f-(xx+zz);		out[1][2]=yz-wx;
		out[2][0]=xz-wy;			out[2][1]=yz+wx;			out[2][2]=1.0f-(xx+yy);
	}

	MATH_INLINE mat3f q_to_rot_m3(const quatf& _rhs)
	{
		mat3f out;
		q_to_rot_m3(out,_rhs);
		return out;
	}

	MATH_INLINE void q_from_axes(quatf& out, const vec3f& _x, const vec3f& _y, const vec3f& _z)
	{
		//mat3f rot(
		//	vec3f(_x.x,_y.x,_z.x),
		//	vec3f(_x.y,_y.y,_z.y),
		//	vec3f(_x.z,_y.z,_z.z)
		//	);
		mat3f rot;
		set_m3_from_axes( rot,
			vec3f(_x.x,_y.x,_z.x),
			vec3f(_x.y,_y.y,_z.y),
			vec3f(_x.z,_y.z,_z.z)
			);
		q_from_rot_m3(out,rot);
	}

	MATH_INLINE quatf q_from_axes(const vec3f& _x, const vec3f& _y, const vec3f& _z)
	{
		quatf out;
		q_from_axes(out,_x,_y,_z);
		return out;
	}

	MATH_INLINE void q_to_angle_axis(vec3f& _out, float& _out1, const quatf& _rhs)
	{
		float fsqrlen = _rhs[1]*_rhs[1]+_rhs[2]*_rhs[2]+_rhs[3]*_rhs[3];
		if (fsqrlen > 0.0f)
		{
			_out1 = 2.0f * acosf(_rhs[0]);
			float invl = 1.0f/sqrtf(fsqrlen);
			_out[0] = _rhs[0]*invl;
			_out[1] = _rhs[1]*invl;
			_out[2] = _rhs[2]*invl;
		}
		else
		{
			_out1 = 0.0f;
			_out[0] = 1.0f;
			_out[1] = 0.0f;
			_out[2] = 0.0f;
		}
	}

	MATH_INLINE vec3f q_to_angle_axis(float& _out, const quatf& _rhs)
	{
		vec3f out;
		q_to_angle_axis(out,_out,_rhs);
		return	out;
	}

	MATH_INLINE void rotate_q(quatf& _out, const vec3f& axis, float angle)
	{
		quatf q = q_from_angle_axis(axis,angle);
		_out = _out * q;
	}

	MATH_INLINE void qpos_to_rot_m4(mat4f& out, const quatf& _rhs, const vec3f& _pos)
	{
		float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
		x2 = _rhs.x + _rhs.x;
		y2 = _rhs.y + _rhs.y;
		z2 = _rhs.z + _rhs.z;
		xx = _rhs.x * x2;   xy = _rhs.x * y2;   xz = _rhs.x * z2;
		yy = _rhs.y * y2;   yz = _rhs.y * z2;   zz = _rhs.z * z2;
		wx = _rhs.w * x2;   wy = _rhs.w * y2;   wz = _rhs.w * z2;

		out[0][0] = 1.0f-(yy+zz);		out[0][1] = xy-wz;			out[0][2] = xz+wy;			out[0][3] = 0.0f;
		out[1][0] = xy+wz;				out[1][1] = 1.0f-(xx+zz);	out[1][2] = yz-wx;			out[1][3] = 0.0f;
		out[2][0] = xz-wy;				out[2][1] = yz+wx;			out[2][2] = 1.0f-(xx+yy);	out[2][3] = 0.0f;
		out[3][0] = _pos.x;				out[3][1] = _pos.y;			out[3][2] = _pos.z;			out[3][3] = 1.0f;
	}

	MATH_INLINE mat4f qpos_to_rot_m4(const quatf& _rhs, const vec3f& _pos)
	{
		mat4f	out;
		qpos_to_rot_m4(out,_rhs,_pos);
		return	out;
	}
}
