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
		float getA() const;
		float getR() const;
		float getG() const;
		float getB() const;

		float operator [] ( const unsigned int _i ) const;
		float& operator [] ( const unsigned int _i );

		Color operator * ( float _s ) const;
		Color operator + ( const Color & _color ) const;
		
	};
}