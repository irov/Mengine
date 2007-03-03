#include "vec2.h"
#include "mat2.h"

namespace	mt
{
	vec2f::vec2f() 
	{
	}

	vec2f::vec2f(float _x, float _y)
		:x(_x)
		,y(_y)
	{
	}

	vec2f::vec2f(const vec2f&	_v)
		:x(_v.x)
		,y(_v.y)
	{}

	vec2f& vec2f::operator=(const vec2f& _rhs)
    {
        x = _rhs.x;
        y = _rhs.y;
        return *this;
    }

	const float vec2f::operator[](int i) const
	{
		assert(i < 2);
		return m[i];
	}

	float&	vec2f::operator[](int i)
	{
		assert(i < 2);
		return m[i];
	}

	vec2f&		vec2f::operator+=(const vec2f&	_rhs)
	{
		x += _rhs.x;
		y += _rhs.y;
		return *this;
	}

	vec2f&		vec2f::operator-=(const vec2f&	_rhs)
	{
		x -= _rhs.x;
		y -= _rhs.y;
		return *this;
	}

	vec2f&		vec2f::operator/=(const float _rhs)
	{
		assert(_rhs != 0.0f);
		operator *= (1.f/_rhs); 
		return *this;
	}

	vec2f&		vec2f::operator*=(const float _rhs)
	{
		x *= _rhs;
		y *= _rhs;
		return *this;
	}

	float vec2f::sqrlength()	const
	{
		return	x * x + y * y;
	}

	float vec2f::length()	const
	{
		return sqrtf(sqrlength());
	}

	float	length_v2_v2(const vec2f& _a, const vec2f& _b)
	{
		vec2f c = _a - _b;
		return c.length();
	}

	bool	cmp_v2_v2(const vec2f& _a, const vec2f& _b, float eps)
	{
		return	(fabs(_a.x - _b.x) < eps) && (fabs(_a.y - _b.y) < eps);
	}

	bool operator==(const vec2f& _a, const vec2f& _b)
	{
		return cmp_v2_v2(_a, _b);
	}

	bool operator!=(const vec2f& _a, const vec2f& _b)
	{
		return !operator==(_a, _b);
	}

	/*	Addition of vecs  */
	void	add_v2_v2(vec2f& _out,const vec2f& _a, const vec2f& _b)
	{
		_out.x = _a.x + _b.x;
		_out.y = _a.y + _b.y;
	}

	vec2f operator+(const vec2f& _a, const vec2f& _b) 
	{
		vec2f	out;
		add_v2_v2(out, _a, _b);
		return	out;
	}

	/*	Addition of vecs  */
	void	sub_v2_v2(vec2f& _out,const vec2f& _a, const vec2f& _b)
	{
		_out.x = _a.x - _b.x;
		_out.y = _a.y - _b.y;
	}

	vec2f operator-(const vec2f& _a, const vec2f& _b) 
	{
		vec2f	out;
		sub_v2_v2(out, _a, _b);
		return	out;
	}

	/*	Scale of vecs  */
	void	scale_v2_v2(vec2f& _out, const vec2f& _a, float _val)
	{
		_out.x = _a.x * _val;
		_out.y = _a.y * _val;
	}

	vec2f	operator*(const vec2f& _rhs, const float _val) 
	{
		vec2f	out;
		scale_v2_v2(out, _rhs, _val);
		return out;
	}

	vec2f	operator/(const vec2f& _rhs, const float _val) 
	{
		return operator*(_rhs, 1/_val);
	}

	/*	Negation */
	void	neg_v2(vec2f& _out)
	{
		_out.x = -_out.x;
		_out.y = -_out.y;
	}

	vec2f operator-(vec2f _rhs)
	{
		neg_v2(_rhs);
		return	_rhs;
	}

	/*	Dot	Product			*/
	float	dot_v2_v2(const vec2f &a, const vec2f &b)
	{
		return a.x * b.x + a.y * b.y ;
	}

	/*	Normalize Vector*/
	void	norm_v2(vec2f& _out, const vec2f& _rhs)
	{
		_out = _rhs / _rhs.length();
	}

	/*	Normalize Vector*/
	float	norm_v2_f(vec2f& _out, const vec2f& _rhs)
	{
		float l = _rhs.length();
		_out = _rhs / l;
		return l;
	}

	vec2f	norm_v2(const vec2f& _rhs)
	{
		vec2f	out;
		norm_v2(out, _rhs);
		return	out;
	}

	void	norm_safe_v2(vec2f& _out, const vec2f& _rhs, float _err)
	{
		float	len = _rhs.length();
		if (len > _err)
        {
			_out = _rhs / len;
			return;
		}
		_out = _rhs;
	}

	vec2f	norm_safe_v2(const vec2f& _rhs)
	{
		vec2f	out;
		norm_safe_v2(out, _rhs);
		return	out;
	}
	
	/*	Perp to Vector		*/
	void	perp(vec2f&	_out, const vec2f& _rhs)
	{
		_out = vec2f(-_rhs.y,_rhs.x);
	}

	vec2f	perp(const vec2f&	_rhs)
	{
		return	vec2f(-_rhs.y,_rhs.x);
	}

	float	is_left_v2(const vec2f& P0, const vec2f& P1, const vec2f& P2)
	{
		return ((P1.x - P0.x) * (P2.y - P0.y) - (P2.x - P0.x) * (P1.y - P0.y));
	}

	float	pseudo_cross_v2(const mt::vec2f& a, const mt::vec2f& b)
	{
		return a.x * b.y - a.y * b.x;
	}

	void	project_v2_v2(const vec2f& a, const vec2f& b, mt::vec2f& result) 
	{
		float dp = dot_v2_v2(a,b);

		result.x = dp * b.x;
		result.y = dp * b.y;
	}

	int clip_segment_to_line_v2(mt::vec2f vOut[2], const mt::vec2f vIn[2],
		const mt::vec2f& normal, float offset, char clipEdge)
	{
		int numOut = 0;

		float distance0 = dot_v2_v2(normal, vIn[0]) - offset;
		float distance1 = dot_v2_v2(normal, vIn[1]) - offset;

		if (distance0 <= 0.0f) vOut[numOut++] = vIn[0];
		if (distance1 <= 0.0f) vOut[numOut++] = vIn[1];

		if (distance0 * distance1 < 0.0f)
		{
			float interp = distance0 / (distance0 - distance1);
			vOut[numOut] = vIn[0] + (vIn[1] - vIn[0]) * interp;
			++numOut;
		}

		return numOut;
	}

	void compute_incident_edge_v2(mt::vec2f c[2], const mt::vec2f& h, const mt::vec2f& pos,
		float angle, const mt::vec2f& normal)
	{
		float cs = cosf(angle);
		float ss = sinf(angle);

		mt::vec2f n = -mt::vec2f(cs * normal.x + ss * normal.y,-ss * normal.x + cs * normal.y);

		mt::vec2f nAbs(abs(n.x),abs(n.y));

		if (nAbs.x > nAbs.y)
		{
			if (n.x > 0.0f)
			{
				c[0]=mt::vec2f(h.x, -h.y);
				c[1]=mt::vec2f(h.x, h.y);
			}
			else
			{
				c[0]=mt::vec2f(-h.x, h.y);
				c[1]=mt::vec2f(-h.x, -h.y);
			}
		}
		else
		{
			if (n.y > 0.0f)
			{
				c[0]=mt::vec2f(h.x, h.y);
				c[1]=mt::vec2f(-h.x, h.y);
			}
			else
			{
				c[0]=mt::vec2f(-h.x, -h.y);
				c[1]=mt::vec2f(h.x, -h.y);
			}
		}

		c[0] = pos + mt::vec2f(cs * c[0].x + -ss * c[0].y,ss * c[0].x + cs * c[0].y);
		c[1] = pos + mt::vec2f(cs * c[1].x + -ss * c[1].y,ss * c[1].x + cs * c[1].y);
	}
}