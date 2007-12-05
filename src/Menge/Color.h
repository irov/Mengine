#	pragma once

#	include "math/vec4.h"

namespace	Menge
{
	class Color
		: public mt::vec4f
	{
	public:
		Color( float _red = 1.0f, float _green = 1.0f, float _blue = 1.0f, float _alpha = 1.0f );

		void set( unsigned int _val );
		unsigned int get() const;

		float operator [] ( const size_t _i ) const;
		float& operator [] ( const size_t _i );

		Color operator * ( float _s ) const;
		Color operator + ( const Color & _color ) const;
	};
}