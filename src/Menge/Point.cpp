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
	void Point::_render( const Viewport & _viewport, bool _enableDebug )
	{
		if( _enableDebug )
		{
			const mt::vec2f& pos = getWorldPosition();
			//mt::vec2f pos;
			//mt::mul_v2_m3_r( pos, getWorldPosition(), getWorldMatrix() );
			mt::vec2f pos1( pos + mt::vec2f( -5.0f, -5.0f ) );
			mt::vec2f pos2( pos + mt::vec2f( 5.0f, -5.0f ) );
			mt::vec2f pos3( pos + mt::vec2f( 5.0f, 5.0f ) );
			mt::vec2f pos4( pos + mt::vec2f( -5.0f, 5.0f ) );
			Holder<RenderEngine>::hostage()->renderLine( 0xFF0000FF, pos1, pos2 );
			Holder<RenderEngine>::hostage()->renderLine( 0xFF0000FF, pos2, pos3 );
			Holder<RenderEngine>::hostage()->renderLine( 0xFF0000FF, pos1, pos4 );
			Holder<RenderEngine>::hostage()->renderLine( 0xFF0000FF, pos4, pos3 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}