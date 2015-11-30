#	pragma once

#	include "config.h"

#	include "vec2.h"

namespace mt
{
	struct vec3f
	{
		typedef float value_type;
		typedef float element_type;

		float x;
		float y;
		float z;

		MENGINE_MATH_METHOD_INLINE vec3f();
		MENGINE_MATH_METHOD_INLINE vec3f( float _x, float _y, float _z );
		MENGINE_MATH_METHOD_INLINE vec3f( const vec3f & _v );

		MENGINE_MATH_METHOD_INLINE explicit vec3f( const vec2f & _v, float _z );

		MENGINE_MATH_METHOD_INLINE vec3f & operator = ( const vec3f & _rhs );
	
		MENGINE_MATH_METHOD_INLINE float operator [] ( size_t i ) const;
		MENGINE_MATH_METHOD_INLINE float & operator [] ( size_t i );

		MENGINE_MATH_METHOD_INLINE vec3f & operator += (const vec3f & _rhs);
		MENGINE_MATH_METHOD_INLINE vec3f & operator -= (const vec3f & _rhs);
		MENGINE_MATH_METHOD_INLINE vec3f & operator /= (const float _rhs);
		MENGINE_MATH_METHOD_INLINE vec3f & operator *= (const float _rhs);
		MENGINE_MATH_METHOD_INLINE float sqrlength() const;
		MENGINE_MATH_METHOD_INLINE float length() const;

		MENGINE_MATH_METHOD_INLINE vec2f to_vec2f() const;

		MENGINE_MATH_METHOD_INLINE float * buff();
		MENGINE_MATH_METHOD_INLINE const float * buff() const;
	};

	MENGINE_MATH_FUNCTION_INLINE bool operator==(const vec3f& _a, const vec3f& _b);
	MENGINE_MATH_FUNCTION_INLINE bool operator!=(const vec3f& _a, const vec3f& _b);

	MENGINE_MATH_FUNCTION_INLINE vec3f operator+(const vec3f& _a, const vec3f& _b);
	MENGINE_MATH_FUNCTION_INLINE vec3f operator-(const vec3f& _a, const vec3f& _b);
	MENGINE_MATH_FUNCTION_INLINE vec3f operator*(const vec3f& _a, const vec3f & _b);
	MENGINE_MATH_FUNCTION_INLINE vec3f operator*(const vec3f& _rhs, const float _val);
	MENGINE_MATH_FUNCTION_INLINE vec3f operator*(const float _val, const vec3f& _rhs);
	MENGINE_MATH_FUNCTION_INLINE vec3f operator/(const vec3f& _a, const vec3f & _b);
	MENGINE_MATH_FUNCTION_INLINE vec3f operator/(const vec3f& _rhs, const float _val);
	MENGINE_MATH_FUNCTION_INLINE vec3f operator-(vec3f _rhs);

	MENGINE_MATH_FUNCTION_INLINE void ident_v3( vec3f& _a );

	MENGINE_MATH_FUNCTION_INLINE float length_v3_v3( const vec3f& _a, const vec3f& _b );
	MENGINE_MATH_FUNCTION_INLINE float length_v3( const vec3f& _a );
	MENGINE_MATH_FUNCTION_INLINE float sqrlength_v3_v3(const vec3f& _a, const vec3f& _b);
	MENGINE_MATH_FUNCTION_INLINE void dir_v3_v3(vec3f & _out, const vec3f& _a, const vec3f& _b);

	MENGINE_MATH_FUNCTION_INLINE bool cmp_v3_v3( const vec3f& _a, const vec3f& _b );

	MENGINE_MATH_FUNCTION_INLINE void add_v3_v3( vec3f& _out, const vec3f& _a, const vec3f& _b );
	MENGINE_MATH_FUNCTION_INLINE void sub_v3_v3( vec3f& _out, const vec3f& _a, const vec3f& _b );
	MENGINE_MATH_FUNCTION_INLINE void scale_v3_v3( vec3f& _out, const vec3f& _a, float _val );
	MENGINE_MATH_FUNCTION_INLINE void mul_v3_v3( vec3f& _out, const vec3f& _a, const vec3f& _b );
	MENGINE_MATH_FUNCTION_INLINE void neg_v3( vec3f& _out );

	MENGINE_MATH_FUNCTION_INLINE float dot_v3_v3( const vec3f &a, const vec3f &b );
	MENGINE_MATH_FUNCTION_INLINE float dot_v3_v3_clamp( const vec3f &a, const vec3f &b );

    MENGINE_MATH_FUNCTION_INLINE float dot_v3_v2( const vec3f &a, const vec2f &b );

	MENGINE_MATH_FUNCTION_INLINE void reflect_v3_v3( vec3f& _out, const vec3f& _a, const vec3f& _b );
	MENGINE_MATH_FUNCTION_INLINE vec3f reflect_v3_v3( const vec3f& _a, const vec3f& _b );

	MENGINE_MATH_FUNCTION_INLINE void cross_v3_v3( vec3f& _out, const vec3f& _a, const vec3f& _b );
	MENGINE_MATH_FUNCTION_INLINE vec3f cross_v3_v3( const vec3f& _a, const vec3f& _b );

	MENGINE_MATH_FUNCTION_INLINE void cross_v3_v3_norm( vec3f& _out, const vec3f& _a, const vec3f& _b );

	MENGINE_MATH_FUNCTION_INLINE void norm_v3( vec3f& _out, const vec3f& _rhs );
	MENGINE_MATH_FUNCTION_INLINE float norm_v3_f( vec3f& _out, const vec3f& _rhs );
	MENGINE_MATH_FUNCTION_INLINE vec3f norm_v3( const vec3f& _rhs );
	MENGINE_MATH_FUNCTION_INLINE void norm_safe_v3( vec3f& out, const vec3f& _rhs, float err = 1e-06 );
	MENGINE_MATH_FUNCTION_INLINE vec3f norm_safe_v3( const vec3f& _rhs );

	MENGINE_MATH_FUNCTION_INLINE float get_axis_angle( const vec3f & dir, int axis );		
};

#	if MENGINE_MATH_FORCE_INLINE == 1
#	include "vec3_inline.h"
#	endif
