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
	Allocator::loader(_xml);

	XML_FOR_EACH_TREE( _xml )
	{
		XML_CHECK_NODE("Point")
		{
			mt::vec2f point;
			XML_VALUE_ATTRIBUTE("Value", point);
			addPoint(point);
		}
	}
};
//////////////////////////////////////////////////////////////////////////
#	include "RenderEngine.h"
//////////////////////////////////////////////////////////////////////////
void HotSpot::_debugRender()
{
	RenderEngine *renderEng = Keeper<RenderEngine>::hostage();

	size_t size = m_poligon.num_points();

	for( size_t i = 0; i < size; ++i )
	{
		renderEng->drawLine(m_poligon[i],m_poligon[ (i+1) % size ] ,2,0xffff00ff);
	}
};
