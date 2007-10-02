#	include "Region.h"

#	include "WayPoint.h"

#	include "ObjectImplement.h"

#	include "SceneManager.h"

#	include "XmlParser/XmlParser.h"

#	include "RenderEngine.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Region)
	//////////////////////////////////////////////////////////////////////////
	Region::Region()
	{};
	//////////////////////////////////////////////////////////////////////////
	Region::~Region()
	{
		for each( WayPoint * wp in m_helperWayPoints )
		{
			delete wp;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	void Region::loader( TiXmlElement * _xml )
	{
		SceneNode2D::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE_FOR_EACH("Points")
			{
				XML_CHECK_NODE("Point")
				{
					mt::vec2f point;
					XML_VALUE_ATTRIBUTE("Value", point);
					m_polygon.add_point(point);
				}
			}
			XML_CHECK_NODE_FOR_EACH("HelperWayPoints")
			{
				XML_CHECK_NODE("WayPoint")
				{
					mt::vec2f point;
					XML_VALUE_ATTRIBUTE("Value", point);
					m_helperWayPoints.push_back(new WayPoint(point));
				}
			}
		}
	}
	////////////////////////////////////////////////////////////////////////
	void Region::_debugRender()
	{
		//RenderEngine *renderEng = Holder<RenderEngine>::hostage();

		//size_t size = m_polygon.num_points();

		//for( size_t i = 0; i < size; ++i )
		//{
		//	renderEng->drawLine2D(m_polygon[i],m_polygon[ (i+1) % size ], 0xffffCCff);
		//}

		//for each( WayPoint * wp in m_helperWayPoints )
		//{
		//	wp->debugRender();
		//}
	}
}