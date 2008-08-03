#	pragma once

#	include "math/vec4.h"

namespace	Menge
{
	struct Color
	{
	public:
		union
		{
			struct
			{
				unsigned char r;
				unsigned char g;
				unsigned char b;
				unsigned char a;
			};

			unsigned char m[4];

			unsigned int v;
		};

	public:
		Color( unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a )
			: r(_r)
			, g(_g)
			, b(_b)
			, a(_a)
		{

		}
	};
}