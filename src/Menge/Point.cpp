#	include "Point.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "HotSpot.h"

#	include "RenderEngine.h"
#	include "Sprite.h"
//#define DEBUG_RENDER

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
		SceneNode2D::_update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Point::_activate()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::loader( XmlElement * _xml )
	{
		SceneNode2D::loader( _xml );
#ifdef DEBUG_RENDER
		Sprite* point = new Sprite();
		point->setImageResource( "AttrGood" );
		//point->setName( "point" + std::string(::itoa ) );
		point->activate();
		point->enable();
		point->compile();
		point->setLocalPosition( getLocalPosition() );

		activate();
		
		addChildren( point );	
#endif
	}
	//////////////////////////////////////////////////////////////////////////
	bool Point::testHotSpot( HotSpot * _hotspot )
	{
		const mt::vec2f& pos = this->getScreenPosition();
		bool result = _hotspot->testPoint( pos );
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::_render( bool _enableDebug )
	{
		if( _enableDebug )
		{
			const mt::vec2f& pos = getScreenPosition();
			mt::vec2f offs( 5.0f, 5.0f );
			Holder<RenderEngine>::hostage()->renderLine( 0xFF0000FF, pos + mt::vec2f( -5.0f, -5.0f ), pos + mt::vec2f( 5.0f, -5.0f ) );
			Holder<RenderEngine>::hostage()->renderLine( 0xFF0000FF, pos + mt::vec2f( 5.0f, -5.0f ), pos + mt::vec2f( 5.0f, 5.0f ) );
			Holder<RenderEngine>::hostage()->renderLine( 0xFF0000FF, pos + mt::vec2f( -5.0f, -5.0f ), pos + mt::vec2f( -5.0f, 5.0f ) );
			Holder<RenderEngine>::hostage()->renderLine( 0xFF0000FF, pos + mt::vec2f( -5.0f, 5.0f ), pos + mt::vec2f( 5.0f, 5.0f ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}