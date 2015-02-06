#	pragma once

#	include "HotSpot.h"

#   include "ResourceHIT.h"

namespace Menge
{
	class HotSpotBubbles
		: public HotSpot
	{
	public:
		HotSpotBubbles();
		~HotSpotBubbles();

	public:
		uint32_t addBubble( const mt::vec2f & _pos, float _radius, bool _outward );

	protected:
		bool testPoint( const mt::vec2f & _point ) const override;
		bool testRadius( const mt::vec2f & _point, float _radius ) const override;
		
	protected:	
	};
}	// namespace Menge
