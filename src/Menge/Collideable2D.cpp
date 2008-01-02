#	include "Collideable2D.h"

#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Collideable2D::Collideable2D()
	{
		reset(m_bbox,0,0);
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::box2f& Collideable2D::getBoundingBox()
	{
		return m_bbox;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::polygon& Collideable2D::getPolygon()
	{
		return m_polygon;
	}
	//////////////////////////////////////////////////////////////////////////
	void Collideable2D::addPoint( const mt::vec2f & _point )
	{
		m_polygon.add_point( _point );
		mt::add_internal_point( m_bbox, _point );
	}
	//////////////////////////////////////////////////////////////////////////
	void Collideable2D::loader( XmlElement * _xml )
	{
		//mt::vec2f	point;

		//XML_FOR_EACH_TREE( _xml )
		//{
		//	XML_CHECK_NODE_FOR_EACH("Hull")
		//	{
		//		XML_VALUE_ATTRIBUTE("Value", point);
		//		m_polygon.add_point(point);
		//		mt::add_internal_point(m_bbox,point);
		//	}
		//}
	}
	//////////////////////////////////////////////////////////////////////////
}