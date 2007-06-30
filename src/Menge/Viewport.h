#	pragma once

#	include "Math/vec2.h"

namespace Menge
{
	class Viewport
	{
	public:
		Viewport();
		Viewport( const mt::vec2f & _begin, const mt::vec2f & _end );

	public:
		bool testPoint( const mt::vec2f & _point ) const;

	public:
		mt::vec2f begin;
		mt::vec2f end;
	};
}
