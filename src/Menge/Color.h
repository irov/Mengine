#	pragma once

#	include "math/vec4.h"

namespace	Menge
{
	class Color
		: public mt::vec4f
	{
	public:
		Color();
		Color( float _red, float _green, float _blue, float _alpha );

		void set( unsigned int _val );
		unsigned int get() const;

		float operator [] ( const size_t _i ) const;
		float& operator [] ( const size_t _i );

		Color operator * ( float _s ) const;
		Color operator + ( const Color & _color ) const;
	};
}