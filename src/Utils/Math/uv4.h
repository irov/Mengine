#	pragma once

#	include "config.h"

#	include "vec4.h"

namespace mt
{
	//////////////////////////////////////////////////////////////////////////
	struct uv4f
	{
		vec2f p0;
		vec2f p1;
		vec2f p2;
		vec2f p3;

		MENGINE_MATH_METHOD_INLINE uv4f();
		MENGINE_MATH_METHOD_INLINE uv4f( const uv4f & _uv );
		MENGINE_MATH_METHOD_INLINE explicit uv4f( const vec2f & _p0, const vec2f & _p1, const vec2f & _p2, const vec2f & _p3 );
		MENGINE_MATH_METHOD_INLINE explicit uv4f( const vec4f & _mask );
		MENGINE_MATH_METHOD_INLINE const mt::vec2f & operator [] ( size_t i ) const;
		MENGINE_MATH_METHOD_INLINE mt::vec2f & operator [] ( size_t i );
		MENGINE_MATH_METHOD_INLINE uv4f & operator = ( const uv4f & _rhs );
	};

	MENGINE_MATH_FUNCTION_INLINE bool uv4_identity( const mt::uv4f & _uv );
	MENGINE_MATH_FUNCTION_INLINE void uv4_from_mask( mt::uv4f & _out, const mt::vec4f & _mask );
	MENGINE_MATH_FUNCTION_INLINE void uv4_rotate( mt::uv4f & _out );
	MENGINE_MATH_FUNCTION_INLINE void uv4_swap_u( mt::uv4f & _out );
	MENGINE_MATH_FUNCTION_INLINE void uv4_swap_v( mt::uv4f & _out );
	MENGINE_MATH_FUNCTION_INLINE void uv4_scale( mt::uv4f & _out, const mt::uv4f & _uv, const mt::vec2f & _scale );
	MENGINE_MATH_FUNCTION_INLINE void multiply_tetragon_uv4( mt::uv4f & _out, const mt::uv4f & _uv1, const mt::uv4f & _uv2 );
	MENGINE_MATH_FUNCTION_INLINE void multiply_tetragon_uv4_v4( mt::uv4f & _out, const mt::uv4f & _uv1, const mt::vec4f & _uv2 );
	MENGINE_MATH_FUNCTION_INLINE void multiply_tetragon_uv4_vp( mt::uv4f & _out, const mt::uv4f & _uv1, const mt::vec2f * _uv2 );
	MENGINE_MATH_FUNCTION_INLINE void multiply_tetragon_uv4_v2( mt::vec2f & _out, const mt::uv4f & _uv, const mt::vec2f & _p );
}

#	if MENGINE_MATH_FORCE_INLINE == 1
#		include "uv4_inline.h"
#	endif