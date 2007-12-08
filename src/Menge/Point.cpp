#	include "Point.h"

#	include "ObjectImplement.h"

#	include "XmlParser/XmlParser.h"

#	include "HotSpot.h"

#	include "RenderEngine.h"

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

		const Viewport & viewport =
			Holder<RenderEngine>::hostage()
			->getRenderViewport();

		m_offset = viewport.begin;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Point::_activate()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Point::testHotSpot( HotSpot * _hotspot )
	{
		const mt::vec2f& pos = getWorldPosition();
		bool result = _hotspot->testPoint( pos, m_offset );
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Point::_debugRender()
	{}
	//////////////////////////////////////////////////////////////////////////
}