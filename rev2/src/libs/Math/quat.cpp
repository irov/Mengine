#include "quat.h"
#include "mat4.h"
namespace	mt
{
	quatf::quatf()
	{}

	quatf::quatf(float _w, float _x, float _y, float _z)
		:w(_w)
		,x(_x)
		,y(_y)
		,z(_z)
	{}

	quatf::quatf(const quatf& _q)
		:w(_q.w)
		,x(_q.x)
		,y(_q.y)
		,z(_q.z)
	{}

	float& quatf::operator[](int i)
	{
		assert(i < 4);
		return m[i];
	}

	const float quatf::operator[](int i)const
	{
		assert(i < 4);
		return m[i];
	}

	quatf& quatf::operator=(const quatf& q)
	{
		w = q.w;
		v = q.v;
		return *this;
	}

	float quatf::norm() const
	{
		return x*x+y*y+z*z+w*w;
	}

	bool	cmp_q_q(const quatf& _a, const quatf& _b, float eps)
	{
		return	(fabs(_a.w - _b.w) < eps) && (fabs(_a.x - _b.x) < eps) && (fabs(_a.y - _b.y) < eps) && (fabs(_a.z - _b.z) < eps);
	}

	bool operator==(const quatf& _a, const quatf& _b) 
	{
		return cmp_q_q(_a, _b);
	}

	bool operator!=(const quatf& _a, const quatf& _b) 
	{
		return !operator==(_a, _b);
	}

	/* Addition */
	void add_q_q(quatf&	_out, const quatf& _a,const quatf& _b)
	{
		_out[0] = _a[0] + _b[0];
		_out[1] = _a[1] + _b[1];
		_out[2] = _a[2] + _b[2];
		_out[3] = _a[3] + _b[3];
	}

	quatf operator+(const quatf& _a, const quatf& _b)
	{
		quatf out;
		add_q_q(out,_a,_b);
		return out;
	}

	/*  Substr  */
	void sub_q_q(quatf&	_out, const quatf& _a,const quatf& _b)
	{
		_out[0] = _a[0] - _b[0];
		_out[1] = _a[1] - _b[1];
		_out[2] = _a[2] - _b[2];
		_out[3] = _a[3] - _b[3];
	}

	quatf operator-(const quatf& _a, const quatf& _b)
	{
		quatf out;
		sub_q_q(out,_a,_b);
		return out;
	}

	/*	Mult  Q &  Q */
	void mul_q_q(quatf& out, const quatf& _rhs, const quatf& in2)
	{
		out[0]=_rhs[0] * in2[0] - _rhs[1] * in2[1] - _rhs[2] * in2[2] - _rhs[3]*in2[3];
		out[1]=_rhs[0] * in2[1] + _rhs[1] * in2[0] + _rhs[2] * in2[3] - _rhs[3]*in2[2];
		out[2]=_rhs[0] * in2[2] + _rhs[2] * in2[0] + _rhs[3] * in2[1] - _rhs[1]*in2[3];
		out[3]=_rhs[0] * in2[3] + _rhs[3] * in2[0] + _rhs[1] * in2[2] - _rhs[2]*in2[1];
	}

	quatf operator*(const quatf& _a, const quatf& _b)
	{
		quatf out;
		mul_q_q(out,_a,_b);
		return out;
	}

	/*	Mult  Q &  V */
	void mul_q_v3(vec3f &out,const quatf& _rhs,const vec3f& _v)
	{
		vec3f uv; 
		vec3f uuv;
		cross_v3_v3(uv,_rhs.v,_v);
		cross_v3_v3(uuv,_rhs.v,uv); 
		uv*=2.0f*_rhs.x;
		uuv*=2.0f;
		add_v3_v3(uv,uv,uuv);
		add_v3_v3(out,_v,uv);
	};

	vec3f operator*(const quatf& _rhs, const vec3f& _v)
	{
		vec3f out;
		mul_q_v3(out,_rhs,_v);
		return out;
	}

	void scale_q_s(quatf &out, const quatf& _rhs, float _val)
	{
		out.w = _rhs.w * _val;
		out.x = _rhs.x * _val;
		out.y = _rhs.y * _val;
		out.z = _rhs.z * _val;
	}

	quatf operator*(const quatf& _rhs, float _val)
	{
		quatf out;
		scale_q_s(out,_rhs,_val);
		return out;
	}

	quatf operator/(const quatf& _rhs, float _val)
	{
		return operator*(_rhs, 1/_val);;
	}

	/*	Dot	Product			*/
	float	dot_q_q(const quatf& _a, const quatf& _b)
	{
		return  _a.w * _b.w + _a.x * _b.x + _a.y * _b.y + _a.z * _b.z;
	}

	/*	Normalize Vector*/
	void	norm_q(quatf& _out, const quatf& _rhs)
	{
		_out = _rhs / _rhs.norm();
	}

	quatf	norm_q(const quatf& _rhs)
	{
		quatf	out;
		norm_q(out, _rhs);
		return	out;
	}

	void	norm_safe_q(quatf& out, const quatf& _rhs, float err)
	{
		float	len = _rhs.norm();
		if (len > err)
        {
			out = _rhs / len;
			return;
		}
		out = _rhs;
	}

	quatf	norm_safe_q(const quatf& _rhs)
	{
		quatf	out;
		norm_safe_q(out, _rhs);
		return	out;
	}

	/* ohter stuff */
	void q_from_angle_axis(quatf& out, const vec3f& _rhs, float _val)
	{
		float hangle = 0.5f * _val;
		float fsin = sin(hangle);
		out.w = cos(hangle);
		out.x = fsin * _rhs[0];
		out.y = fsin * _rhs[1];
		out.z = fsin * _rhs[2];
	};

	quatf q_from_angle_axis(const vec3f& _rhs, float _val)
	{
		quatf	out;
		q_from_angle_axis(out,_rhs,_val);
		return	out;
	};

	void inverse_q(quatf& _out, const quatf& _rhs)
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

	quatf inverse_q(const quatf& _rhs)
	{
		quatf	out;
		inverse_q(out,_rhs);
		return	out;
	};

	void exp_q(quatf& _out, const quatf& _rhs)
	{
		/* 
			If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
			exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k).  If sin(A) is near zero,
			use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.
		*/

		float angle = sqrtf(_rhs.x*_rhs.x+_rhs.y*_rhs.y+_rhs.z*_rhs.z);
		float fsin = sin(angle);

		_out.w = cos(angle);

		if (fabs(fsin) >= 0.00001f)
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

	quatf exp_q(const quatf& _rhs)
	{
		quatf	out;
		exp_q(out,_rhs);
		return	out;
	};

	void log_q(quatf& _out, const quatf& _rhs)
	{
		/*
			If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
			log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
			sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.
		*/

		_out.w = 0.0f;

		if (fabsf(_rhs[0]) < 1.0f)
		{
			float angle = cos(_rhs.w);
			float fsin = sin(angle);
			if (fabs(fsin) >= 0.00001f)
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

	quatf log_q(const quatf& _rhs)
	{
		quatf	out;
		log_q(out,_rhs);
		return	out;
	};

	void q_from_rot_m3(quatf& out, const mat3f& _rhs)
	{
		float ftrace = _rhs[0][0]+_rhs[1][1]+_rhs[2][2];
		float froot;

		if (ftrace > 0.0f)
		{
			froot = sqrtf(ftrace + 1.0f );  
			out[0] = 0.5f*froot;
			froot = 0.5f/froot;  
			out[1] = (_rhs[2][1]-_rhs[1][2])*froot;
			out[2] = (_rhs[0][2]-_rhs[2][0])*froot;
			out[3] = (_rhs[1][0]-_rhs[0][1])*froot;
		}
		else
		{
			static int s_iNext[3] = { 1, 2, 0 };
			int i = 0;
			if ( _rhs[1][1] > _rhs[0][0] )
			{
				i = 1;
			}

			if ( _rhs[2][2] > _rhs[i][i] )
			{
				i = 2;
			}

			int j = s_iNext[i];
			int k = s_iNext[j];

			froot = sqrtf(_rhs[i][i]-_rhs[j][j]-_rhs[k][k] + 1.0f);
			out.v[i] =	0.5f*froot;
			froot =		0.5f/froot;
			out[0]	   = (_rhs[k][j]-_rhs[j][k])*froot;
			out.v[j]   = (_rhs[j][i]+_rhs[i][j])*froot;
			out.v[k]   = (_rhs[k][i]+_rhs[i][k])*froot;
		}
	};

	quatf q_from_rot_m3(const mat3f& _rhs)
	{
		quatf	out;
		q_from_rot_m3(out,_rhs);
		return	out;
	};

	void q_from_rot_m4(quatf& out, const mat4f& _rhs)
	{
		float ftrace = _rhs[0][0]+_rhs[1][1]+_rhs[2][2];
		float froot;

		if (ftrace > 0.0f)
		{
			froot = sqrtf(ftrace + 1.0f );  
			out[0] = 0.5f*froot;
			froot = 0.5f/froot;  
			out[1] = (_rhs[2][1]-_rhs[1][2])*froot;
			out[2] = (_rhs[0][2]-_rhs[2][0])*froot;
			out[3] = (_rhs[1][0]-_rhs[0][1])*froot;
		}
		else
		{
			static int s_iNext[3] = { 1, 2, 0 };
			int i = 0;
			if ( _rhs[1][1] > _rhs[0][0] )
			{
				i = 1;
			}

			if ( _rhs[2][2] > _rhs[i][i] )
			{
				i = 2;
			}

			int j = s_iNext[i];
			int k = s_iNext[j];

			froot = sqrtf(_rhs[i][i]-_rhs[j][j]-_rhs[k][k] + 1.0f);
			out.v[i] =	0.5f*froot;
			froot =		0.5f/froot;
			out[0]	   = (_rhs[k][j]-_rhs[j][k])*froot;
			out.v[j]   = (_rhs[j][i]+_rhs[i][j])*froot;
			out.v[k]   = (_rhs[k][i]+_rhs[i][k])*froot;
		}
	};

	quatf q_from_rot_m4(const mat4f& _rhs)
	{
		quatf	out;
		q_from_rot_m4(out,_rhs);
		return	out;
	};

	void q_to_rot_m3(mat3f& out, const quatf& _rhs)
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

	mat3f q_to_rot_m3(const quatf& _rhs)
	{
		mat3f	out;
		q_to_rot_m3(out,_rhs);
		return	out;
	}

	void q_from_axes(quatf& out, const vec3f& _x, const vec3f& _y, const vec3f& _z)
	{
		mat3f rot(
			vec3f(_x.x,_y.x,_z.x),
			vec3f(_x.y,_y.y,_z.y),
			vec3f(_x.z,_y.z,_z.z)
			);
		q_from_rot_m3(out,rot);
	}

	quatf q_from_axes(const vec3f& _x, const vec3f& _y, const vec3f& _z)
	{
		quatf	out;
		q_from_axes(out,_x,_y,_z);
		return	out;
	}

	void q_to_angle_axis(vec3f& _out, float& _out1, const quatf& _rhs)
	{
		float fsqrlen = _rhs[1]*_rhs[1]+_rhs[2]*_rhs[2]+_rhs[3]*_rhs[3];
		if (fsqrlen > 0.0f)
		{
			_out1 = 2.0f * acos(_rhs[0]);
			float invl = 1.0f/sqrtf(fsqrlen);
			_out[0] = _rhs[1]*invl;
			_out[1] = _rhs[2]*invl;
			_out[2] = _rhs[3]*invl;
		}
		else
		{
			_out1 = 0.0f;
			_out[0] = 1.0f;
			_out[1] = 0.0f;
			_out[2] = 0.0f;
		}
	}

	vec3f q_to_angle_axis(float& _out, const quatf& _rhs)
	{
		vec3f	out;
		q_to_angle_axis(out,_out,_rhs);
		return	out;
	}
}