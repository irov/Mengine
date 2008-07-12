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
	void Point::_update( float _timing )
	{
		Node::_update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Point::_activate()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::loader( XmlElement * _xml )
	{
		Node::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Point::testHotSpot( HotSpot * _hotspot )
	{
	//	const mt::vec2f& pos = this->getScreenPosition();
	//	bool result = _hotspot->testPoint( pos );
	//	return result;
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::_render( const Viewport & _viewport, bool _enableDebug )
	{
		if( _enableDebug )
		{
			const mt::vec2f& pos = getWorldPosition();
			mt::vec2f offs( 5.0f, 5.0f );
			Holder<RenderEngine>::hostage()->renderLine( 0xFF0000FF, pos + mt::vec2f( -5.0f, -5.0f ), pos + mt::vec2f( 5.0f, -5.0f ) );
			Holder<RenderEngine>::hostage()->renderLine( 0xFF0000FF, pos + mt::vec2f( 5.0f, -5.0f ), pos + mt::vec2f( 5.0f, 5.0f ) );
			Holder<RenderEngine>::hostage()->renderLine( 0xFF0000FF, pos + mt::vec2f( -5.0f, -5.0f ), pos + mt::vec2f( -5.0f, 5.0f ) );
			Holder<RenderEngine>::hostage()->renderLine( 0xFF0000FF, pos + mt::vec2f( -5.0f, 5.0f ), pos + mt::vec2f( 5.0f, 5.0f ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}