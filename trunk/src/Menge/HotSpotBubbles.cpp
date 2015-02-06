#	include "HotSpotBubbles.h"

#	include "Interface/ResourceInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "ResourceHIT.h"

#	include "Logger/Logger.h"

#	include "Core/String.h"
#	include "Consts.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	HotSpotBubbles::HotSpotBubbles()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	HotSpotBubbles::~HotSpotBubbles()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t HotSpotBubbles::addBubble( const mt::vec2f & _pos, float _radius, bool _outward )
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotBubbles::testPoint( const mt::vec2f & _point ) const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool HotSpotBubbles::testRadius( const mt::vec2f & _point, float _radius ) const
	{
		return false;
	}
}	// namespace Menge
