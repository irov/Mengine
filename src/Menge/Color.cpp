#	include "Color.h"
#	include <assert.h>

Color::Color( float _red, float _green, float _blue, float _alpha )
	: r( _red )
	, g( _green )
	, b( _blue )
	, a( _alpha )
{ 
}

float Color::operator [] ( const size_t i ) const
{
	assert( i < 4 );
	return *(&r+i);
}

float& Color::operator [] ( const size_t i )
{
	assert( i < 4 );
	return *(&r+i);
}

void Color::set( unsigned int _val )
{
    unsigned int val32 = _val;
    a = ((val32 >> 24) & 0xFF) / 255.0f;
    r = ((val32 >> 16) & 0xFF) / 255.0f;
    g = ((val32 >> 8) & 0xFF) / 255.0f;
    b = (val32 & 0xFF) / 255.0f;
}

unsigned int Color::get() const
{
	unsigned char val8;
	unsigned int val32 = 0;

	val8 = static_cast<unsigned char>(a * 255);
	val32 = val8 << 24;

	val8 = static_cast<unsigned char>(r * 255);
	val32 += val8 << 16;

	val8 = static_cast<unsigned char>(g * 255);
	val32 += val8 << 8;

	val8 = static_cast<unsigned char>(b * 255);
	val32 += val8;

	return val32;
}

Color Color::operator * ( float _s ) const
{
	Color col;

    col.r = _s * r;
    col.g = _s * g;
    col.b = _s * b;
    col.a = _s * a;

    return col;
}

Color Color::operator + ( const Color & _color ) const
{
    Color col;

    col.r = r + _color.r;
    col.g = g + _color.g;
    col.b = b + _color.b;
    col.a = a + _color.a;

    return col;
}