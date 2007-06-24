#	include "HotSpot.h"

#	include "ObjectImplement.h"

#	include "XmlParser.h"

//////////////////////////////////////////////////////////////////////////
OBJECT_IMPLEMENT(HotSpot)
//////////////////////////////////////////////////////////////////////////
void HotSpot::addPoint( const mt::vec2f &p )
{
	m_poligon.add_point(p);
}
//////////////////////////////////////////////////////////////////////////
bool HotSpot::testPoint( const mt::vec2f &p )
{
	return mt::is_point_inside_polygon(m_poligon,p);
}
//////////////////////////////////////////////////////////////////////////
void HotSpot::loader( TiXmlElement *_xml)
{
	Allocator2D::loader(_xml);

	XML_FOR_EACH_TREE( _xml )
	{
		XML_CHECK_NODE("Point")
		{
			mt::vec2f point;
			XML_VALUE_ATTRIBUTE("Value", point);
			addPoint(point);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
#	include "RenderEngine.h"
//////////////////////////////////////////////////////////////////////////
void HotSpot::_debugRender()
{
	RenderEngine *renderEng = Holder<RenderEngine>::hostage();

	size_t size = m_poligon.num_points();

	for( size_t i = 0; i < size; ++i )
	{
		const mt::mat3f & wm = getWorldMatrix();
		
		mt::vec2f b;
		mt::vec2f e;

		mt::mul_v2_m3( b, m_poligon[ i ], wm );
		mt::mul_v2_m3( e, m_poligon[ (i+1) % size ], wm );

		renderEng->drawLine( b, e, 2, 0xffff00ff );
	}
}