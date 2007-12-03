#	pragma once

namespace	Menge
{
	class Color
	{
	public:
		float a;
		float r;
		float g;
		float b;

		Color( float _red = 1.0f, float _green = 1.0f, float _blue = 1.0f, float _alpha = 1.0f );

		void set( unsigned int _val );
		unsigned int get() const;

		float operator [] ( const size_t i ) const;
		float& operator [] ( const size_t i );

		Color operator * ( float _s ) const;
		Color operator + ( const Color & _color ) const;
	};
}