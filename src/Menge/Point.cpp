#	include "Point.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "HotSpot.h"

#	include "RenderEngine.h"
#	include "Sprite.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( Point )
	//////////////////////////////////////////////////////////////////////////
	Point::Point()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Point::testHotSpot( HotSpot * _hotspot )
	{
		const mt::vec2f& pos = this->getScreenPosition();
		bool result = _hotspot->testPoint( pos );
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::_render( unsigned int _debugMask )
	{
#	ifndef MENGE_MASTER_RELEASE
		if( _debugMask & MENGE_DEBUG_HOTSPOTS )
		{
			const mt::vec2f& pos = getWorldPosition();
			Holder<RenderEngine>::hostage()->
				renderRect( 0xFF00FFFF, pos + mt::vec2f(-5,-5), pos + mt::vec2f(5,5) );
		}
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
}
