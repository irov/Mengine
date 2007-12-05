#	include "Color.h"

#	include <assert.h>

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Color::Color()
		: mt::vec4f( 1.0f, 1.0f, 1.0f, 1.0f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Color::Color( float _red, float _green, float _blue, float _alpha )
		: mt::vec4f( _red, _green, _blue, _alpha )
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	float Color::operator [] ( const size_t _i ) const
	{
		assert( _i < 4 );
		return *(&r + _i);
	}
	//////////////////////////////////////////////////////////////////////////
	float& Color::operator [] ( const size_t _i )
	{
		assert( _i < 4 );
		return *(&r + _i);
	}
	//////////////////////////////////////////////////////////////////////////
	void Color::set( unsigned int _val )
	{
		unsigned int val32 = _val;
		a = ( ( val32 >> 24 ) & 0xFF ) / 255.0f;
		r = ( ( val32 >> 16 ) & 0xFF ) / 255.0f;
		g = ( ( val32 >> 8 ) & 0xFF ) / 255.0f;
		b = ( val32 & 0xFF ) / 255.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int Color::get() const
	{
		unsigned int val32 = 0;

		unsigned char val8 = static_cast<unsigned char>( a * 255 );
		val32 = val8 << 24;

		val8 = static_cast<unsigned char>( r * 255 );
		val32 += val8 << 16;

		val8 = static_cast<unsigned char>( g * 255 );
		val32 += val8 << 8;

		val8 = static_cast<unsigned char>( b * 255 );
		val32 += val8;

		return val32;
	}
	//////////////////////////////////////////////////////////////////////////
	Color Color::operator * ( float _s ) const
	{
		Color result(1.0f, 1.0f, 1.0f, 1.0f);

		result.r = _s * r;
		result.g = _s * g;
		result.b = _s * b;
		result.a = _s * a;

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	Color Color::operator + ( const Color & _color ) const
	{
		Color result(1.0f, 1.0f, 1.0f, 1.0f);

		result.r = r + _color.r;
		result.g = g + _color.g;
		result.b = b + _color.b;
		result.a = a + _color.a;

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
}