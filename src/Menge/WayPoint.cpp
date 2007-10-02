#	include "WayPoint.h"

#	include "ObjectImplement.h"

#	include "XmlParser/XmlParser.h"

#	include "RenderEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(WayPoint)
	//////////////////////////////////////////////////////////////////////////
	WayPoint::WayPoint()
	{}
	//////////////////////////////////////////////////////////////////////////
	WayPoint::WayPoint(const mt::vec2f& _pos)
	{
		setPosition(_pos);
	}
	//////////////////////////////////////////////////////////////////////////
	void WayPoint::loader( TiXmlElement * _xml)
	{
		SceneNode2D::loader(_xml);

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE("WayPoint")
			{
				mt::vec2f point;
				XML_VALUE_ATTRIBUTE("Value", point);
				setPosition(point);
			}
		}
	};
	//////////////////////////////////////////////////////////////////////////
	void WayPoint::_debugRender()
	{
		//RenderEngine *renderEng = Holder<RenderEngine>::hostage();

		//mt::vec2f p = getLocalPosition();
		//renderEng->drawLine2D(mt::vec2f(p.x-3,p.y-3),mt::vec2f(p.x+3,p.y+3),0xFFFfFCff);
		//renderEng->drawLine2D(mt::vec2f(p.x-3,p.y+3),mt::vec2f(p.x+3,p.y-3),0xFFFfFCff);
	}
	//////////////////////////////////////////////////////////////////////////
}