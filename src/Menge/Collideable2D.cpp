#	include "Collideable2D.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	Collideable2D::Collideable2D()
	{
		reset(m_bbox,0,0);
	}

	const mt::box2f& Collideable2D::getBoundingBox()
	{
		return m_bbox;
	}

	void Collideable2D::loader( TiXmlElement * _xml )
	{
		mt::vec2f	point;

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE_FOR_EACH("Hull")
			{
				XML_VALUE_ATTRIBUTE("Value", point);
				m_polygon.add_point(point);
				mt::add_internal_point(m_bbox,point);
			}
		}
	}
}