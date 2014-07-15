#	pragma once

#	include "config.h"

namespace mt
{
	struct vec2f
	{
		float x;
		float y;

		MATH_METHOD_INLINE vec2f();
		MATH_METHOD_INLINE vec2f(float _x, float _y);
		MATH_METHOD_INLINE vec2f(const vec2f & _v);

		MATH_METHOD_INLINE vec2f & operator = (const vec2f & _rhs);
		

		MATH_METHOD_INLINE float operator [] ( int i ) const;
		MATH_METHOD_INLINE float & operator [] ( int i );

		template <int K>
		MATH_INLINE float get() const
		{
			return this->operator [] (K);
		}

		template <int K>
		MATH_INLINE void set( float _value )
		{
			this->operator [] (K) = _value;
		}

		MATH_METHOD_INLINE vec2f& operator+=(const vec2f & _rhs);
		MATH_METHOD_INLINE vec2f& operator-=(const vec2f &	_rhs);
		MATH_METHOD_INLINE vec2f& operator/=(const float _rhs);
		MATH_METHOD_INLINE vec2f& operator*=(const float _rhs);
		MATH_METHOD_INLINE vec2f& operator/=(const vec2f & _rhs);
		MATH_METHOD_INLINE vec2f& operator*=(const vec2f & _rhs);

		MATH_METHOD_INLINE float sqrlength() const;
		MATH_METHOD_INLINE float length() const;

		MATH_METHOD_INLINE float * buff();
		MATH_METHOD_INLINE const float * buff() const;
	};

	MATH_FUNCTION_INLINE bool operator==(const vec2f& _a, const vec2f& _b);
	MATH_FUNCTION_INLINE bool operator!=(const vec2f& _a, const vec2f& _b);
	MATH_FUNCTION_INLINE vec2f operator+(const vec2f& _a, const vec2f& _b);
	MATH_FUNCTION_INLINE vec2f operator-(const vec2f& _a, const vec2f& _b);
	MATH_FUNCTION_INLINE vec2f operator*(const vec2f& _a, const vec2f& _b);
	MATH_FUNCTION_INLINE vec2f operator*(const vec2f& _rhs, float _val);
	MATH_FUNCTION_INLINE vec2f operator*(float _val, const vec2f& _rhs);
	MATH_FUNCTION_INLINE vec2f operator/(const vec2f& _a, const vec2f& _b);
	MATH_FUNCTION_INLINE vec2f operator/(const vec2f& _rhs, float _val);
	MATH_FUNCTION_INLINE vec2f operator-(vec2f _rhs);

	MATH_FUNCTION_INLINE void ident_v2(vec2f& _a);
	MATH_FUNCTION_INLINE float length_v2(const vec2f& _a);
	MATH_FUNCTION_INLINE float sqrlength_v2_v2(const vec2f& _a, const vec2f& _b);
	MATH_FUNCTION_INLINE float length_v2_v2(const vec2f& _a, const vec2f& _b);

	MATH_FUNCTION_INLINE bool cmp_v2_v2( const vec2f& _a, const vec2f& _b );

	MATH_FUNCTION_INLINE void add_v2_v2(vec2f& _out,const vec2f& _a, const vec2f& _b);
	MATH_FUNCTION_INLINE void sub_v2_v2(vec2f& _out,const vec2f& _a, const vec2f& _b);
	MATH_FUNCTION_INLINE void mul_v2_f(vec2f& _out, const vec2f& _a, float _val);
	MATH_FUNCTION_INLINE void mul_v2_v2(vec2f& _out, const vec2f& _a, const vec2f& _b);
	MATH_FUNCTION_INLINE void div_v2_v2(vec2f& _out, const vec2f& _a, const vec2f& _b);
	MATH_FUNCTION_INLINE void neg_v2(vec2f& _out);

	MATH_FUNCTION_INLINE float dot_v2_v2(const vec2f& a, const vec2f& b);

	MATH_FUNCTION_INLINE float norm_v2_f(vec2f& _out, const vec2f& _rhs);
	MATH_FUNCTION_INLINE void norm_v2(vec2f& _out, const vec2f& _rhs);
	MATH_FUNCTION_INLINE vec2f norm_v2(const vec2f& _rhs);
	MATH_FUNCTION_INLINE void norm_safe_v2(vec2f& out, const vec2f& _rhs, float err = 0.00001f);
	MATH_FUNCTION_INLINE vec2f norm_safe(const vec2f& _rhs);

	MATH_FUNCTION_INLINE void perp_v2(vec2f& out, const vec2f& in);
	MATH_FUNCTION_INLINE vec2f perp(const vec2f&	in);

	MATH_FUNCTION_INLINE void perp_left_v2(vec2f& out, const vec2f& in);
	MATH_FUNCTION_INLINE vec2f perp_left(const vec2f&	in);

	MATH_FUNCTION_INLINE float is_left_v2(const vec2f& P0, const vec2f& P1, const vec2f& P2);
	MATH_FUNCTION_INLINE float pseudo_cross_v2(const vec2f& a, const vec2f& b);

	MATH_FUNCTION_INLINE void project_v2_v2(const vec2f& a, const vec2f& b, vec2f& result);

	MATH_FUNCTION_INLINE vec2f slerp_v2_v2(const vec2f& a, const vec2f& b, float t);

	MATH_FUNCTION_INLINE void rotate_v2(vec2f& _out, float _angle);

	MATH_FUNCTION_INLINE void direction( mt::vec2f & _vec, float _angle );
	MATH_FUNCTION_INLINE float signed_angle(const mt::vec2f & _vec);
};

#	if MATH_FORCE_INLINE == 1
#	include "vec2_inline.h"
#	endif
