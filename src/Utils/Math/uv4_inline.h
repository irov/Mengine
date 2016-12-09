#	include "uv4.h"

#	include "line2.h"

#	include "utils.h"

namespace mt
{
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_METHOD_INLINE uv4f::uv4f()
		: p0(0.f, 0.f)
		, p1(1.f, 0.f)
		, p2(1.f, 1.f)
		, p3(0.f, 1.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_METHOD_INLINE uv4f::uv4f( const vec2f & _p0, const vec2f & _p1, const vec2f & _p2, const vec2f & _p3 )
		: p0(_p0)
		, p1(_p1)
		, p2(_p2)
		, p3(_p3)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_METHOD_INLINE uv4f::uv4f( const vec4f & _mask )
		: p0( _mask.x, _mask.y )
		, p1( _mask.z, _mask.y )
		, p2( _mask.z, _mask.w )
		, p3( _mask.x, _mask.w )
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_METHOD_INLINE uv4f::uv4f( const uv4f & _uv )
		: p0(_uv.p0)
		, p1(_uv.p1)
		, p2(_uv.p2)
		, p3(_uv.p3)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_METHOD_INLINE const mt::vec2f & uv4f::operator [] ( size_t i ) const
	{
		return (&p0)[i];
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_METHOD_INLINE mt::vec2f & uv4f::operator [] ( size_t i )
	{
		return (&p0)[i];
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_METHOD_INLINE uv4f & uv4f::operator = ( const uv4f & _rhs )
	{
		p0 = _rhs.p0;
		p1 = _rhs.p1;
		p2 = _rhs.p2;
		p3 = _rhs.p3;

		return *this;
	}
    //////////////////////////////////////////////////////////////////////////
    MENGINE_MATH_METHOD_INLINE const uv4f & uv4f::identity()
    {
        static mt::uv4f uv;

        return uv;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_MATH_FUNCTION_INLINE bool operator==( const uv4f& _a, const uv4f& _b )
    {
        return cmp_uv4_uv4( _a, _b );
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_MATH_FUNCTION_INLINE bool operator!=( const uv4f& _a, const uv4f& _b )
    {
        return !operator==( _a, _b );
    }
    MENGINE_MATH_FUNCTION_INLINE bool cmp_uv4_uv4( const mt::uv4f& _a, const mt::uv4f& _b )
    {
        return	mt::cmp_v2_v2( _a.p0, _b.p0 ) == true &&
            mt::cmp_v2_v2( _a.p1, _b.p1 ) == true &&
            mt::cmp_v2_v2( _a.p2, _b.p2 ) == true &&
            mt::cmp_v2_v2( _a.p3, _b.p3 ) == true;
    }
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE bool uv4_identity( const mt::uv4f & _uv )
	{ 
		float uv_c = _uv.p0.x + _uv.p0.y +
			_uv.p1.x + _uv.p1.y +
			_uv.p2.x + _uv.p2.y +
			_uv.p3.x + _uv.p3.y;

		if( mt::equal_f_f( uv_c, 4.f ) == false )
		{
			return false;
		}

		return mt::equal_f_z( _uv.p0.x ) == true &&
			mt::equal_f_z( _uv.p0.y ) == true &&
			mt::equal_f_1( _uv.p1.x ) == true &&
			mt::equal_f_z( _uv.p1.y ) == true &&
			mt::equal_f_1( _uv.p2.x ) == true &&
			mt::equal_f_1( _uv.p2.y ) == true &&
			mt::equal_f_z( _uv.p3.x ) == true &&
			mt::equal_f_1( _uv.p3.y ) == true;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void uv4_from_mask( mt::uv4f & _out, const mt::vec4f & _mask )
	{
		_out[0] = mt::vec2f(_mask.x, _mask.y);
		_out[1] = mt::vec2f(_mask.z, _mask.y);
		_out[2] = mt::vec2f(_mask.z, _mask.w);
		_out[3] = mt::vec2f(_mask.x, _mask.w);
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void uv4_rotate( mt::uv4f & _out )
	{
		mt::vec2f tmp = _out[0];
		_out[0] = _out[1];
		_out[1] = _out[2];
		_out[2] = _out[3];
		_out[3] = tmp;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void uv4_swap_u( mt::uv4f & _out )
	{
		mt::vec2f tmp_0 = _out[0];
		_out[0] = _out[1];
		_out[1] = tmp_0;

		mt::vec2f tmp_1 = _out[2];
		_out[2] = _out[3];
		_out[3] = tmp_1;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void uv4_swap_v( mt::uv4f & _out )
	{
		mt::vec2f tmp_0 = _out[0];
		_out[0] = _out[3];
		_out[3] = tmp_0;

		mt::vec2f tmp_1 = _out[1];
		_out[1] = _out[2];
		_out[2] = tmp_1;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void uv4_scale( mt::uv4f & _out, const mt::uv4f & _uv, const mt::vec2f & _scale )
	{
		_out[0] = _uv[0];
		_out[1] = _uv[0] + (_uv[1] - _uv[0]) * _scale;
		_out[2] = _uv[1] + (_uv[2] - _uv[1]) * _scale;
		_out[3] = _uv[0] + (_uv[3] - _uv[0]) * _scale;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void multiply_tetragon_uv4( mt::uv4f & _out, const mt::uv4f & _uv1, const mt::uv4f & _uv2 )
	{
		const mt::vec2f & A = _uv1.p0;
		const mt::vec2f & B = _uv1.p1;
		const mt::vec2f & C = _uv1.p2;
		const mt::vec2f & D = _uv1.p3;

		for( size_t i = 0; i != 4; ++i )
		{
			const mt::vec2f & uv = _uv2[i];

			mt::vec2f AB = A + (B - A) * uv.x;
			mt::vec2f DC = D + (C - D) * uv.x;

			mt::vec2f AD = A + (D - A) * uv.y;
			mt::vec2f BC = B + (C - B) * uv.y;

			mt::line2f uv_l1;
			mt::line_from_two_point_v2( uv_l1, AB, DC );

			mt::line2f uv_l2;
			mt::line_from_two_point_v2( uv_l2, AD, BC );

			mt::vec2f new_uv;
			mt::line_intersect_v2_np( uv_l1, uv_l2, new_uv );

			_out[i] = new_uv;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void multiply_tetragon_uv4_vp( mt::uv4f & _out, const mt::uv4f & _uv1, const mt::vec2f * _uv2 )
	{
		const mt::vec2f & A = _uv1.p0;
		const mt::vec2f & B = _uv1.p1;
		const mt::vec2f & C = _uv1.p2;
		const mt::vec2f & D = _uv1.p3;

		for( size_t i = 0; i != 4; ++i )
		{
			const mt::vec2f & uv = _uv2[i];

			mt::vec2f AB = A + (B - A) * uv.x;
			mt::vec2f DC = D + (C - D) * uv.x;

			mt::vec2f AD = A + (D - A) * uv.y;
			mt::vec2f BC = B + (C - B) * uv.y;

			mt::line2f uv_l1;
			mt::line_from_two_point_v2( uv_l1, AB, DC );

			mt::line2f uv_l2;
			mt::line_from_two_point_v2( uv_l2, AD, BC );

			mt::vec2f new_uv;
			mt::line_intersect_v2_np( uv_l1, uv_l2, new_uv );

			_out[i] = new_uv;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void multiply_tetragon_uv4_v2( mt::vec2f & _out, const mt::uv4f & _uv, const mt::vec2f & _p )
	{
		mt::vec2f A = _uv.p0;
		mt::vec2f B = _uv.p1;
		mt::vec2f C = _uv.p2;
		mt::vec2f D = _uv.p3;

		mt::vec2f AB = A + (B - A) * _p.x;
		mt::vec2f DC = D + (C - D) * _p.x;

		mt::vec2f AD = A + (D - A) * _p.y;
		mt::vec2f BC = B + (C - B) * _p.y;

		mt::line2f uv_l1;
		mt::line_from_two_point_v2( uv_l1, AB, DC );

		mt::line2f uv_l2;
		mt::line_from_two_point_v2( uv_l2, AD, BC );

		mt::vec2f new_uv;
		mt::line_intersect_v2_np( uv_l1, uv_l2, new_uv );

		_out = new_uv;
	}
	//////////////////////////////////////////////////////////////////////////
	MENGINE_MATH_FUNCTION_INLINE void multiply_tetragon_uv4_v4( mt::uv4f & _out, const mt::uv4f & _uv1, const mt::vec4f & _mask )
	{
		mt::uv4f uv2;
		uv4_from_mask( uv2, _mask );

		multiply_tetragon_uv4( _out, _uv1, uv2 );
	}
}